class RL_MapUIComponent : SCR_MapUIBaseComponent {
    
    // Constants
    private const float UPDATE_INTERVAL_MS = 200;
    private const float DEFAULT_ZOOM_LEVEL = 0.75;
    private const ResourceName SEARCH_LAYOUT_RESOURCE = "{322F32D733F2BFB9}UI/Layouts/Map/NewMarkerSearch.layout";
    
    // State variables
    protected bool isMapCurrentlyOpen = false;
    protected float lastUpdateTimestamp = 0;
    protected float previousZoomLevel = -1;
    
    // UI Configuration
    [Attribute("0")]
    private bool shouldHideMarkerText;
    
    [Attribute("0")]
    private bool isMarkerSearchDisabled;
    
    // Marker management
    static private ref array<RL_MapMarker> globalMarkerRegistry = {};
    
    // Search UI components
    private Widget searchWidget;
    private RL_MapMarkerSearchWidgetComponent searchWidgetComp;
    
    //------------------------------------------------------------------------------------------------
    // Initialization
    //------------------------------------------------------------------------------------------------
    override void Init() {
        super.Init();

        SCR_ChimeraCharacter localPlayer = RL_Utils.GetLocalCharacter();
        if (!localPlayer) return;

        // Sort markers by distance from player
        ref array<RL_MapMarker> unsortedMarkers = {};
        array<float> markerDistances = {};
        ref array<RL_MapMarker> sortedMarkerArray = {};
        
        // Calculate distances for all markers
        foreach (RL_MapMarker marker : globalMarkerRegistry) {
            //Print("[RL_MapUIComponent] Init foreach loop");
            if (!marker) continue;
            
            unsortedMarkers.Insert(marker);
            float distance = vector.Distance(marker.GetOwnerOrigin(), localPlayer.GetOrigin());
            markerDistances.Insert(distance);
        }
        
        // Sort markers by distance using selection sort
        while (markerDistances.Count() > 0) {
            //Print("[RL_MapUIComponent] Init while loop");
            int nearestMarkerIndex = RL_MapUtils.FindIndexOfSmallestDistance(markerDistances);
            
            sortedMarkerArray.Insert(unsortedMarkers[nearestMarkerIndex]);
            
            // Remove processed marker and distance
            unsortedMarkers.Remove(nearestMarkerIndex);
            markerDistances.Remove(nearestMarkerIndex);
        }
        
        globalMarkerRegistry = sortedMarkerArray;
    }
    //------------------------------------------------------------------------------------------------
    // Map Event Handlers
    //------------------------------------------------------------------------------------------------
    private void HandleMapPanEvent(float x, float y, bool adjustedPan) {
        RefreshAllMarkerPositions();
    }
    
    private void HandleMapZoomEvent(float zoomValue) {
        RefreshAllMarkerPositions();
    }
    
    override protected void OnMapOpen(MapConfiguration config) {
        isMapCurrentlyOpen = true;
        if (!IsMapEntityAndRootWidgetValid()) return;
        
        RegisterMapEventListeners();
        InitializeMapDisplay();
        SetupPeriodicUpdates();
    }
    
    override protected void OnMapClose(MapConfiguration config) {
        isMapCurrentlyOpen = false;
        CleanupPeriodicUpdates();
        
        if (!m_MapEntity) return;
        UnregisterMapEventListeners();
        RemoveAllMarkersFromDisplay();
    }
    
    private bool IsMapEntityAndRootWidgetValid() {
        return m_MapEntity && m_RootWidget;
    }
    
    private void RegisterMapEventListeners() {
        SCR_MapEntity.GetOnMapPan().Insert(HandleMapPanEvent);
        SCR_MapEntity.GetOnMapZoom().Insert(HandleMapZoomEvent);
        GetGame().GetInputManager().AddActionListener("RL_PlaceWaypoint", EActionTrigger.DOWN, HandleWaypoint);
    }
    vector m_wayPointPos;
    IEntity m_waypointEntity;
    void HandleWaypoint()
    {
        if(!m_MapEntity)
            return;

        bool deletedMarker = DeleteMarkerUnderCursor();
        if(deletedMarker)
            return;
        		
		vector worldPos;
        float worldX, worldY;
        m_MapEntity.GetMapCursorWorldPosition(worldX, worldY);
        worldPos[0] = worldX;
        worldPos[2] = worldY;
        worldPos[1] = GetGame().GetWorld().GetSurfaceY(worldPos[0], worldPos[2]);
        m_wayPointPos = worldPos;


        CreateWaypointMarker();
    }
    private bool DeleteMarkerUnderCursor()
    {
        if(!m_waypointEntity) return false;
    
        RL_MapMarker mapMarker = RL_MapMarker.Cast(m_waypointEntity.FindComponent(RL_MapMarker));
        if(!mapMarker) return false;
        RL_MapMarkerWidgetComponent mapMarkerWidget = mapMarker.GetWidgetHandler();
        if(!mapMarkerWidget) return false;
        Print(mapMarkerWidget);
        array<Widget> tracedW = SCR_MapCursorModule.GetMapWidgetsUnderCursor();
        foreach (Widget w : tracedW)
        {
            //Print("[RL_MapUIComponent] DeleteMarkerUnderCursor foreach loop");
            if(w == mapMarkerWidget.GetTextWidget() || w == mapMarkerWidget.GetImageWidget())
            {
                DeleteWaypoint();
                return true;
            }
        }
        return false;

    }
    private void DeleteWaypoint()
    {
        if(m_waypointEntity)
        {
            RL_MapMarker mapMarker = RL_MapMarker.Cast(m_waypointEntity.FindComponent(RL_MapMarker));
            if(mapMarker)
                globalMarkerRegistry.RemoveItem(mapMarker);
            SCR_EntityHelper.DeleteEntityAndChildren(m_waypointEntity);
        }
        RL_Hud hud = RL_Hud.GetCurrentInstance();
        if(hud)
            hud.SetWaypointPos(vector.Zero);
    }
    private void CreateWaypointMarker()
    {
        if(!m_wayPointPos)
            return;

        DeleteWaypoint();

        RL_Hud hud = RL_Hud.GetCurrentInstance();
        if(hud)
            hud.SetWaypointPos(m_wayPointPos);

        EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.Transform[3] = m_wayPointPos;
		spawnParams.TransformMode = ETransformMode.WORLD;

        Resource loadedResource = Resource.Load("{FE27186B6EFAA152}Prefabs/World/Locations/Common/RL_MapMarker_Waypoint.et");
        m_waypointEntity = GetGame().SpawnEntityPrefabLocal(loadedResource, GetGame().GetWorld(), spawnParams);
        RL_MapMarker mapMarker = RL_MapMarker.Cast(m_waypointEntity.FindComponent(RL_MapMarker));

        CreateAndConfigureMarker(mapMarker);
        RefreshAllMarkerPositions();

    }
    
    private void UnregisterMapEventListeners() {
        SCR_MapEntity.GetOnMapPan().Remove(HandleMapPanEvent);
        SCR_MapEntity.GetOnMapZoom().Remove(HandleMapZoomEvent);
    }
    
    private void InitializeMapDisplay() {
        previousZoomLevel = -1;
        CreateAllVisibleMarkers();
        RefreshAllMarkerPositions();
        InitializeSearchInterface();
    }
    
    private void SetupPeriodicUpdates() {
        GetGame().GetCallqueue().CallLater(PerformPeriodicUpdate, UPDATE_INTERVAL_MS, true);
    }
    
    private void CleanupPeriodicUpdates() {
        GetGame().GetCallqueue().Remove(PerformPeriodicUpdate);
    }
    
    //------------------------------------------------------------------------------------------------
    // Marker Display Management
    //------------------------------------------------------------------------------------------------
    private void CreateAllVisibleMarkers() {
        SCR_ChimeraCharacter localPlayer = RL_Utils.GetLocalCharacter();
        
        foreach(RL_MapMarker marker: globalMarkerRegistry) {
            //Print("[RL_MapUIComponent] CreateAllVisibleMarkers foreach loop");
            if (!marker) continue;
            
            if (!RL_MapUtils.ShouldMarkerBeVisible(marker, localPlayer)) continue;
            
            CreateAndConfigureMarker(marker);
        }
    }
    
    
    private void CreateAndConfigureMarker(RL_MapMarker marker) {
        marker.CreateMarker(m_RootWidget);
        marker.UpdateMarker();
        
        if (shouldHideMarkerText) {
            marker.GetWidgetHandler().HideText();
        }
    }
    //------------------------------------------------------------------------------------------------
    // Marker Updates
    //------------------------------------------------------------------------------------------------
    void PerformPeriodicUpdate() {
        RefreshAllMarkerPositions(true);
    }
    private void RefreshAllMarkerPositions(bool isPeriodicUpdate = false) {
        float currentZoom = m_MapEntity.GetCurrentZoom();
        bool hasZoomChanged = !float.AlmostEqual(currentZoom, previousZoomLevel);
        previousZoomLevel = currentZoom;
        
        foreach(RL_MapMarker marker: globalMarkerRegistry) {
            //Print("[RL_MapUIComponent] RefreshAllMarkerPositions foreach loop");
            if (!marker) continue;
            
            // During periodic updates, only update player markers for performance
            if (isPeriodicUpdate && marker.GetMarkerType() != RL_MARKER_TYPE.Player) continue;
            
            marker.UpdateMarker(!hasZoomChanged);
        }
    }
    private void RemoveAllMarkersFromDisplay() {
        foreach(RL_MapMarker marker: globalMarkerRegistry) {
            //Print("[RL_MapUIComponent] RemoveAllMarkersFromDisplay foreach loop");
            if (!marker) continue;
            marker.DeleteMarker();
        }
    }
    //------------------------------------------------------------------------------------------------
    // Search Functionality
    //------------------------------------------------------------------------------------------------
    private void InitializeSearchInterface() {
        if (isMarkerSearchDisabled) return;
        
        searchWidget = GetGame().GetWorkspace().CreateWidgets(SEARCH_LAYOUT_RESOURCE, m_RootWidget);
        searchWidgetComp = RL_MapMarkerSearchWidgetComponent.Cast(searchWidget.FindHandler(RL_MapMarkerSearchWidgetComponent));
        searchWidgetComp.SetMapUiComp(this);
    }
    void FocusOnMarkerLocation(vector markerPosition) {
        if (!m_MapEntity) return;
        
        vector mapDimensions = m_MapEntity.GetMapWidget().GetSizeInUnits();
        m_MapEntity.ZoomPanSmooth(DEFAULT_ZOOM_LEVEL, markerPosition[0], markerPosition[2]);
    }
    ref array<RL_MapMarker> GetMarkerArray()
    {
        return globalMarkerRegistry;
    }
    //------------------------------------------------------------------------------------------------
    // Static Registry Management
    //------------------------------------------------------------------------------------------------
    static void RegisterMarker(RL_MapMarker marker) {
        if (globalMarkerRegistry.Contains(marker)) return;
        globalMarkerRegistry.Insert(marker);
    }
    
    static void UnregisterMarker(RL_MapMarker marker) {
        if (!globalMarkerRegistry.Contains(marker)) return;
        globalMarkerRegistry.RemoveItem(marker);
    }
    static void UnstreamInvsibleMarkers() {
        Print("UnstreamInvsibleMarkers");
        PlayerController pc = GetGame().GetPlayerController();
        if (!pc) return;
        SCR_ChimeraCharacter localPlayer = RL_Utils.GetLocalCharacter();
        if(!localPlayer) return;
        RplIdentity identity = pc.GetRplIdentity();
		if (!identity.IsValid()) return;
        foreach(RL_MapMarker marker: globalMarkerRegistry) {
            if (!marker) continue;
            RplComponent entityRpl = RplComponent.Cast(marker.FindComponent(RplComponent));
            if (entityRpl && !RL_MapUtils.ShouldMarkerBeVisible(marker, localPlayer))
            {
                Print("Unstreaming entity for marker");
                entityRpl.EnableStreamingConNode(identity, false);
            }
        }
        
    }
}