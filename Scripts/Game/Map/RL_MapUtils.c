class RL_MapUtils
{
	static IEntity CreateMarkerClient(vector position, string text, string iconName, Color color, float expireTimeMs = 300000, RL_MARKER_TYPE markerType = RL_MARKER_TYPE.Generic)
	{
		if (Replication.IsServer())
			return null;
		
		IEntity markerEntity = CreateMarker(position, text, iconName, color, expireTimeMs, true, markerType);
		
		return markerEntity;
	}

	static IEntity CreateMarkerServer(vector position, string text, string iconName, Color color, float expireTimeMs = 300000, RL_MARKER_TYPE markerType = RL_MARKER_TYPE.Generic)
	{
		if (!Replication.IsServer())
			return null;
		
		IEntity markerEntity = CreateMarker(position, text, iconName, color, expireTimeMs, false, markerType);

		return markerEntity;
	}

	static IEntity CreateMarkerFromPrefabClient(vector position, string prefabPath, string markerText, float expireTimeMs = 300000)
	{
		if (Replication.IsServer())
			return null;
		
		IEntity markerEntity = CreateMarkerFromPrefab(position, prefabPath, markerText, expireTimeMs);
		return markerEntity;
	}

	static IEntity CreateMarkerFromPrefabServer(vector position, string prefabPath, string markerText, float expireTimeMs = 300000)
	{
		if (!Replication.IsServer())
			return null;
		
		IEntity markerEntity = CreateMarkerFromPrefab(position, prefabPath, markerText, expireTimeMs);
		return markerEntity;
	}

	static IEntity CreateMarker(vector position, string text, string iconName, Color color, float expireTimeMs = 300000, bool clientOnly = false, RL_MARKER_TYPE markerType = RL_MARKER_TYPE.Generic)
	{
		
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = position;
		
		Resource markerResource = Resource.Load("{E595423ADC263333}Prefabs/World/Locations/Common/RL_MapMarker.et");
		if(clientOnly)
			markerResource = Resource.Load("{98D609EE4EF899D0}Prefabs/World/Locations/Common/RL_MapMarkerClientOnly.et");

		if (!markerResource)
			return null;
		
		GenericEntity markerEntity = GenericEntity.Cast(GetGame().SpawnEntityPrefab(markerResource, GetGame().GetWorld(), spawnParams));
		if (!markerEntity)
			return null;
		
		RL_MapMarker markerComponent = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
		if (markerComponent)
		{
			markerComponent.SetMarkerProperties(text, iconName, color);
			markerComponent.SetMarkerType(markerType);
		}
		
		if (expireTimeMs > 0)
		{
			GetGame().GetCallqueue().CallLater(DeleteMarkerEntityByRplId, expireTimeMs, false, EPF_NetworkUtils.GetRplId(markerEntity));
		}
		
		return markerEntity;
	}
	
	static IEntity CreateMarkerFromPrefab(vector position, string prefabPath, string markerText, float expireTimeMs = 300000)
	{
		
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = position;
		
		Resource markerResource = Resource.Load(prefabPath);
		if (!markerResource)
			return null;
		
		GenericEntity markerEntity = GenericEntity.Cast(GetGame().SpawnEntityPrefab(markerResource, GetGame().GetWorld(), spawnParams));
		if (!markerEntity)
			return null;
		
		RL_MapMarker markerComponent = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
		if (markerComponent)
		{
			markerComponent.SetText(markerText);
		}
		
		if (expireTimeMs > 0)
		{
			
			GetGame().GetCallqueue().CallLater(DeleteMarkerEntityByRplId, expireTimeMs, false, EPF_NetworkUtils.GetRplId(markerEntity));
		}
		
		return markerEntity;
	}
	
	static void DeleteMarkerEntityByRplId(int markerRplId)
	{
		IEntity markerEntity = EPF_NetworkUtils.FindEntityByRplId(markerRplId);
		if (!markerEntity)
			return;
		
		SCR_EntityHelper.DeleteEntityAndChildren(markerEntity);
	}

	static void DeleteMarkerEntity(IEntity markerEntity)
	{
		if (!markerEntity)
			return;
		
		SCR_EntityHelper.DeleteEntityAndChildren(markerEntity);
	}
	
	static void CancelMarkerDeletion(IEntity markerEntity)
	{
		if (markerEntity)
		{
			GetGame().GetCallqueue().Remove(DeleteMarkerEntity);
		}
	}
    static float CalculateZoomBasedSize(float currentZoom, int minSize, int maxSize)
    {
        if(currentZoom == 0.0) return minSize;
        float zoomRatio = (currentZoom - 0.25) / (1.75);
        float sizeIncrease = (zoomRatio * (maxSize - minSize));
        
        return Math.Max(minSize, Math.Min(maxSize, minSize+sizeIncrease));
    }
    static void GetRadiusMarkerCenter(vector ownerOrgin, WorkspaceWidget workspace, int radius, out int posX, out int posY, out int diameter)
    {
        float centerX, centerY, edgeX, edgeY;
        SCR_MapEntity.GetMapInstance().WorldToScreen(
            ownerOrgin[0],
            ownerOrgin[2],
            centerX,
            centerY,
            true
        );
        SCR_MapEntity.GetMapInstance().WorldToScreen(
            ownerOrgin[0] + radius, 
            ownerOrgin[2] + radius, 
            edgeX,
            edgeY,
            true
        );
        
        diameter = workspace.DPIUnscale((edgeX - centerX) * 2);

        float unscaledCenterX = workspace.DPIUnscale(centerX);
        float unscaledCenterY = workspace.DPIUnscale(centerY);
        int unscaledRadius = (diameter / 2);
        posX = unscaledCenterX - unscaledRadius;
        posY = unscaledCenterY - unscaledRadius;

    }
    static bool ShouldMarkerBeVisible(RL_MapMarker marker, SCR_ChimeraCharacter localPlayer) {
        switch (marker.GetMarkerType()) {
            case RL_MARKER_TYPE.Player:
                return RL_MapUtils.ShouldPlayerMarkerBeVisible(marker, localPlayer);
            case RL_MARKER_TYPE.PoliceEmergency:
                return localPlayer.IsPolice();
            case RL_MARKER_TYPE.EmsEmergency:
                return localPlayer.IsMedic();
            case RL_MARKER_TYPE.CivOnly:
                return !localPlayer.IsPolice() && !localPlayer.IsMedic();
            case RL_MARKER_TYPE.GangOnly:
                return marker.GetGangId() > 0 && localPlayer.GetGangId() == marker.GetGangId();
        }
        
        // Default case: show all other marker types
        return true;
    }
    static bool ShouldPlayerMarkerBeVisible(RL_MapMarker marker, SCR_ChimeraCharacter localPlayer) {
        SCR_ChimeraCharacter markerOwner = marker.GetOwnerCharacter();
        if (!markerOwner || !markerOwner.GetCharacterController()) return false;
        
        // Don't show dead players
        bool isOwnerDead = markerOwner.GetCharacterController().GetLifeState() == ECharacterLifeState.DEAD;
        if (isOwnerDead) return false;
        
        // Always show own marker
        if (localPlayer == markerOwner) return true;
        
        bool isOwnerWaitingForCpr = marker.IsOwnerWaitingForCpr();
        bool areInSameGang = RL_MapUtils.AreBothInSameGang(localPlayer, markerOwner);
        bool areBothEmergencyPersonnel = RL_MapUtils.AreBothEmergencyPersonnel(localPlayer, markerOwner);
        
        // Show to medics if waiting for CPR
        if (isOwnerWaitingForCpr && localPlayer.IsMedic()) return true;
        
        // Show to gang members or emergency personnel (if not waiting for CPR)
        if (!isOwnerWaitingForCpr && (areInSameGang || areBothEmergencyPersonnel)) return true;
        
        return false;
    }
    static bool AreBothInSameGang(SCR_ChimeraCharacter player1, SCR_ChimeraCharacter player2) {
        int gangId = player1.GetGangId();
        return gangId > 0 && gangId == player2.GetGangId();
    }
    static bool AreBothEmergencyPersonnel(SCR_ChimeraCharacter player1, SCR_ChimeraCharacter player2) {
        bool isPlayer1Emergency = player1.IsPolice() || player1.IsMedic();
        bool isPlayer2Emergency = player2.IsPolice() || player2.IsMedic();
        return isPlayer1Emergency && isPlayer2Emergency;
    }
    static bool ShouldIncludeMarkerInSearch(RL_MapMarker marker, string searchFilter) {
        if (!marker) return false;
        
        // Exclude player markers from search
        if (marker.GetMarkerType() == RL_MARKER_TYPE.Player) return false;
        
        string markerText = marker.GetText();
        if (markerText.IsEmpty()) return false;
        
        // Include all markers if no filter is applied
        if (searchFilter.IsEmpty()) return true;
        
        // Check if marker text matches filter
        string translatedMarkerText = WidgetManager.Translate(markerText);
        translatedMarkerText.ToLower();
        return translatedMarkerText.Contains(searchFilter);
    }
    static int FindIndexOfSmallestDistance(array<float> distances) {
        int smallestIndex = 0;
        float smallestDistance = distances[0];
        
        for (int i = 1; i < distances.Count(); i++) {
            //Print("[RL_MapUtils] FindIndexOfSmallestDistance for");
            if (distances[i] < smallestDistance) {
                smallestDistance = distances[i];
                smallestIndex = i;
            }
        }
        
        return smallestIndex;
    }
}