sealed enum RL_MARKER_TYPE {
    Generic,
    Illegal = 1,
    Player = 2,
    PoliceEmergency = 3,
    EmsEmergency = 4,
    CivOnly = 5,
    GangOnly = 6,
    Waypoint = 7
}

sealed class RL_MapMarkerClass : GameComponentClass {} 

sealed class RL_MapMarker : GameComponent {
    // Constants for marker sizing and zoom
    private const int DEFAULT_MIN_ICON_SIZE = 42;
    private const int DEFAULT_MAX_ICON_SIZE = 65;
    private const int DEFAULT_MIN_TEXT_SIZE = 28;
    private const int DEFAULT_MAX_TEXT_SIZE = 32;
    
    // Layout resource paths
    private const ResourceName MARKER_IMAGESET = "{0A09C46D79C6D698}UI/Imagesets/MapMarkers/mapMarkerIcons.imageset";
    private const ResourceName MARKER_LAYOUT = "{0C19D3E03D310498}UI/Layouts/Map/NewMarker.layout";
    private const ResourceName MARKER_ALT_LAYOUT = "{0BFAD4FC90389C46}UI/Layouts/Map/NewMarkerRadius.layout";
    private const ResourceName CONTROL_ZONE_LAYOUT = "{0BFAD4FC90389C46}UI/Layouts/Map/NewMarkerRadius.layout";
    
    // Widget element names
    private const string IMAGE_WIDGET_NAME = "IconImage";
    private const string TEXT_WIDGET_NAME = "MarkerText";
    
    // Default marker configurations
    private const string UNCONSCIOUS_MARKER_ICON = "LIFE_ALERT";
    private static const ref Color POLICE_MARKER_COLOR = new Color(0.0, 0.0, 1.0, 1.0);
    private static const ref Color MEDIC_MARKER_COLOR = new Color(0.0, 1.0, 0.0, 1.0);
    private static const ref Color DEFAULT_MARKER_COLOR = new Color(0.0, 0.0, 0.0, 1.0);

    // Size constraints
    private int m_minIconSize = DEFAULT_MIN_ICON_SIZE;
    private int m_maxIconSize = DEFAULT_MAX_ICON_SIZE;
    private int m_minTextSize = DEFAULT_MIN_TEXT_SIZE;
    private int m_maxTextSize = DEFAULT_MAX_TEXT_SIZE;
    
    // Configuration attributes
    [Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_MARKER_TYPE)), RplProp()] 
    private RL_MARKER_TYPE m_markerType;
    
    [Attribute(""), RplProp()] 
    private string m_markerText;
    
    // TODO replicate this later with simple types then have a set method as we cannot replicate Color
    [Attribute("0 0 0 1")] 
    private ref Color m_markerColor;
    
    [Attribute(""), RplProp()] 
    private string m_markerIconName;
    
    [Attribute("-1")] 
    private int m_markerRadius;
    
    [Attribute("-1"), RplProp()]
    private int m_gangId;
    
    // Internal state
    private bool m_useControlZoneLayout = false;
    private IEntity m_ownerEntity;
    private Widget m_wMapMarker;
    private SCR_MapEntity m_mapInstance;
    private int m_currentIconSize = 0;
    
    private RL_MapMarkerWidgetComponent m_markerWidgetComp;

    //------------------------------------------------------------------------------------------------
    // Public API Methods
    //------------------------------------------------------------------------------------------------
    RL_MARKER_TYPE GetMarkerType() {
        return m_markerType;
    }
    SCR_ChimeraCharacter GetOwnerCharacter() {
        if (!m_ownerEntity) 
            return null;
        return SCR_ChimeraCharacter.Cast(m_ownerEntity);
    }
    string GetText() { 
        if (GetMarkerType() == RL_MARKER_TYPE.Player && m_ownerEntity) {
            SCR_ChimeraCharacter character = GetOwnerCharacter();
            if (character) 
                return character.GetCharacterName();
        }
        return m_markerText;
    }
    Color GetColor() {
        if (GetMarkerType() == RL_MARKER_TYPE.Player && m_ownerEntity) {
            SCR_ChimeraCharacter character = GetOwnerCharacter();
            if (character) {
                if (character.IsPolice()) 
                    return POLICE_MARKER_COLOR;
                if (character.IsMedic()) 
                    return MEDIC_MARKER_COLOR;
            }
        }
        if (m_markerColor)
            return m_markerColor;
        return DEFAULT_MARKER_COLOR;
    }
    string GetMarkerIconName() {
        if (IsOwnerWaitingForCpr())
            return UNCONSCIOUS_MARKER_ICON;
        return m_markerIconName;
    }
    
    int GetMarkerRadius() {
        return m_markerRadius;
    }
    
    int GetGangId() {
        return m_gangId;
    }
    
    void SetGangId(int gangId) {
        m_gangId = gangId;
        
        if (Replication.IsServer()) {
            Replication.BumpMe();
        }
    }
    
    IEntity GetOwner() {
        return m_ownerEntity; 
    }
    
    Widget GetWidget() {
        return m_wMapMarker;
    }
    RL_MapMarkerWidgetComponent GetWidgetHandler()
    {
        return m_markerWidgetComp;
    }

    vector GetOwnerOrigin() {
        if (!m_ownerEntity) 
            return vector.Zero;
        return m_ownerEntity.GetOrigin();
    }
    //------------------------------------------------------------------------------------------------
    // Configuration Methods
    //------------------------------------------------------------------------------------------------
    void SetIconSizeClamp(int minSize, int maxSize) {
        m_minIconSize = minSize;
        m_maxIconSize = maxSize;
    }
    
    void SetTextSizeClamp(int minSize, int maxSize) {
        m_minTextSize = minSize;
        m_maxTextSize = maxSize;
    }
    
    void SetMarkerProperties(string text, string iconName, Color color) {
        m_markerText = text;
        m_markerIconName = iconName;
        m_markerColor = color;
        
        if (Replication.IsServer()) {
            Replication.BumpMe();
        }
    }
    
    void SetMarkerType(RL_MARKER_TYPE markerType) {
        m_markerType = markerType;
        
        if (Replication.IsServer()) {
            Replication.BumpMe();
        }
    }
    
    void SetRadius(int radius) {
        m_markerRadius = radius;
        if (m_wMapMarker) {
            RecreateMarker();
        }
    }
    
    void SetUseControlZoneLayout(bool useControlZone) {
        m_useControlZoneLayout = useControlZone;
        if (m_wMapMarker) {
            RecreateMarker();
        }
    }
    //------------------------------------------------------------------------------------------------
    // Visibility and State Methods
    //------------------------------------------------------------------------------------------------
    
    void SetVisible(bool visible) {
        if (m_wMapMarker)
            m_wMapMarker.SetVisible(visible);
    }
    
    void SetText(string text) {
        m_markerText = text;
        if(m_markerWidgetComp)
            m_markerWidgetComp.SetupMarkerText(GetText(), GetColor());
    }
    //------------------------------------------------------------------------------------------------
    // Marker Lifecycle Methods
    //------------------------------------------------------------------------------------------------
    void CreateMarker(Widget parentWidget) {
        if (!parentWidget) return;
        
        ResourceName layoutToUse = DetermineLayoutToUse();
        m_wMapMarker = GetGame().GetWorkspace().CreateWidgets(layoutToUse, parentWidget);
        Widget overlayWidget = m_wMapMarker.FindWidget("MarkerOverlay");
        m_markerWidgetComp = RL_MapMarkerWidgetComponent.Cast(overlayWidget.FindHandler(RL_MapMarkerWidgetComponent));
        
        if (!m_wMapMarker) return;
        
        
        if (IsRadiusMarker()) {
            m_markerWidgetComp.SetupRadiusMarker(GetColor(), GetText(), m_useControlZoneLayout);
        } else {
            SetupStandardMarker();
        }
    }
    void UpdateMarker(bool skipResize = false) {
        if (!m_wMapMarker || !m_ownerEntity) {
            if (m_wMapMarker)
                m_wMapMarker.SetVisible(false);
            return;
        }

        UpdateMarkerPosition();
        
        if (!skipResize) {
            UpdateMarkerSizing();
        }
        
        if (GetMarkerType() == RL_MARKER_TYPE.Player) {
            UpdatePlayerMarkerRotation();
        }
        
        m_markerWidgetComp.SetupMarkerText(GetText(), GetColor());
    }
    void DeleteMarker() {
        if (m_wMapMarker) {
            delete m_wMapMarker;
            m_wMapMarker = null;
        }
    }
    bool IsOwnerWaitingForCpr() {
        SCR_ChimeraCharacter character = GetOwnerCharacter();
        if (!character) return false;
        
        SCR_CharacterDamageManagerComponent damageMgr = 
            SCR_CharacterDamageManagerComponent.Cast(character.FindComponent(SCR_CharacterDamageManagerComponent));
        
        return damageMgr && damageMgr.IsWaitingForCpr();
    }
    //------------------------------------------------------------------------------------------------
    // Private Helper Methods
    //------------------------------------------------------------------------------------------------
    private ResourceName DetermineLayoutToUse() {
        if (IsRadiusMarker()) {
            if (m_useControlZoneLayout)
                return CONTROL_ZONE_LAYOUT;
            return MARKER_ALT_LAYOUT;
        }
        return MARKER_LAYOUT;
    }
    private bool IsRadiusMarker() {
        return m_markerRadius > 0;
    }
    private void SetupStandardMarker() {
        m_markerWidgetComp.SetupMarkerIcon(GetMarkerIconPath(), GetMarkerIconName());
        m_markerWidgetComp.SetupMarkerText(GetText(), GetColor());
    }
    ResourceName GetMarkerIconPath() { 
        return MARKER_IMAGESET;
    }
    private void UpdateMarkerPosition() {
        vector ownerOrgin = GetOwnerOrigin();
        float screenX, screenY;
        SCR_MapEntity.GetMapInstance().WorldToScreen(
            ownerOrgin[0],
            ownerOrgin[2],
            screenX, screenY, true
        );
        
        WorkspaceWidget workspace = GetGame().GetWorkspace();
        
        if (IsRadiusMarker()) {
            UpdateRadiusMarkerPosition(ownerOrgin, workspace);
        } else {
            UpdateStandardMarkerPosition(screenX, screenY, workspace);
        }
    }
    private void UpdateRadiusMarkerPosition(vector ownerOrgin, WorkspaceWidget workspace) {
        int posX, posY, diameter;
        RL_MapUtils.GetRadiusMarkerCenter(
            ownerOrgin,
            workspace,
            m_markerRadius,
            posX,
            posY,
            diameter
        );
        FrameSlot.SetPos(m_wMapMarker, posX, posY);
        m_markerWidgetComp.SetIconSize(diameter);
    }
    private void UpdateStandardMarkerPosition(float screenX, float screenY, WorkspaceWidget workspace) {
        // Calculate icon center position by offsetting screen coordinates by half icon size
        int halfIconSize = m_currentIconSize / 2;
        int posX = workspace.DPIUnscale(screenX) - halfIconSize;
        int posY = workspace.DPIUnscale(screenY) - halfIconSize;
        FrameSlot.SetPos(m_wMapMarker, posX, posY);
    }
    private void UpdateMarkerSizing() {
        
        int iconSize = RL_MapUtils.CalculateZoomBasedSize(GetCurrentZoom(), m_minIconSize, m_maxIconSize);
        m_markerWidgetComp.SetIconSize(iconSize);
        m_currentIconSize = iconSize;
        
        int textSize = RL_MapUtils.CalculateZoomBasedSize(GetCurrentZoom(), m_minTextSize, m_maxTextSize);
        m_markerWidgetComp.SetTextSize(textSize);
    }
    private void UpdatePlayerMarkerRotation() {
        SCR_ChimeraCharacter character = GetOwnerCharacter();
        if (character) {
            vector yawPitchRoll = character.GetYawPitchRoll();
            m_markerWidgetComp.SetIconRotation(yawPitchRoll[0]);
        }
    }
    private float GetCurrentZoom() {
        if (!m_mapInstance) {
            m_mapInstance = SCR_MapEntity.GetMapInstance();
        }
        if (!m_mapInstance) return 0.0;
        return m_mapInstance.GetCurrentZoom();
    }
    private void RecreateMarker() {
        Widget parentWidget = m_wMapMarker.GetParent();
        DeleteMarker();
        CreateMarker(parentWidget);
    }
    //------------------------------------------------------------------------------------------------
    // Component Lifecycle
    //------------------------------------------------------------------------------------------------
    void RL_MapMarker(IEntityComponentSource src, IEntity ent, IEntity parent) {
        if (!GetGame().InPlayMode()) return;
        
        m_ownerEntity = ent;
        RL_MapUIComponent.RegisterMarker(this);
    }
    
    void ~RL_MapMarker() {
        RL_MapUIComponent.UnregisterMarker(this);
        DeleteMarker();
    }
}
