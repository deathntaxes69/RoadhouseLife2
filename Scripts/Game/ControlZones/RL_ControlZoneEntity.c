[EntityEditorProps(category: "GameScripted/ControlZones", description: "Gang control zone trigger for territory control system")]
class RL_ControlZoneEntityClass: SCR_BaseTriggerEntityClass
{
}

class RL_ControlZoneEntity : SCR_BaseTriggerEntity
{
	[Attribute("1", desc: "Unique zone ID")]
	protected int m_iZoneId;
	
	[Attribute("Control Zone", desc: "Name of this control zone")]
	protected string m_sZoneName;
	
	[Attribute("30", desc: "Time in seconds required to capture this zone")]
	protected int m_iCaptureTime;
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected int m_iControllingGangId = -1;
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected int m_iCapturingGangId = -1;
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected bool m_bCaptureInProgress = false;
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected float m_fCaptureStartTime = 0;
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected string m_sControllingGangName = "";
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected string m_sCapturingGangName = "";
	
	[RplProp(onRplName: "OnControlStateChanged")]
	protected float m_fRemainingCaptureTime = 0;
	
	protected int m_iCaptureCheckInterval = 1000;
	protected RL_MapMarker m_mapMarker;
	protected IEntity m_ownerEntity;
	protected ref RL_ControlZoneDbHelper m_dbHelper;
	protected ref RL_GangDbHelper m_gangDbHelper;
	protected ref map<string, string> m_gangNameCache;
	protected bool m_bPendingCaptureNotification = false;
	
	int GetZoneId() { return m_iZoneId; }
	string GetZoneName() { return m_sZoneName; }
	int GetCaptureTime() { return m_iCaptureTime; }
	int GetControllingGangId() { return m_iControllingGangId; }
	int GetCapturingGangId() { return m_iCapturingGangId; }
	bool IsCaptureInProgress() { return m_bCaptureInProgress; }
	string GetControllingGangName() { return m_sControllingGangName; }
	
	//------------------------------------------------------------------------------------------------
	void OnControlStateChanged()
	{
		if (m_mapMarker)
		{
			UpdateMapMarker();
		}
	}

	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_ownerEntity = owner;
		
		if (!GetGame().InPlayMode()) 
			return;
		
		AddClassType(SCR_ChimeraCharacter);
		CreateMapMarker();
		UpdateMapMarker();
		
		RL_ControlZoneManager manager = RL_ControlZoneManager.GetInstance();
		if (manager)
			manager.RegisterControlZone(this);
			
		if (Replication.IsServer())
			LoadControlZoneState();
		else
		{
			GetGame().GetCallqueue().CallLater(UpdateMapMarker, 1000, false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_ControlZoneEntity()
	{
		RL_ControlZoneManager manager = RL_ControlZoneManager.GetInstance();
		if (manager)
			manager.UnregisterControlZone(this);
			
		if (m_mapMarker && m_mapMarker.GetOwner())
			delete m_mapMarker.GetOwner();
	}
	
	//------------------------------------------------------------------------------------------------
	void StartCapture(int gangId, SCR_ChimeraCharacter initiator)
	{
		if (!Replication.IsServer())
		{
			return;
		}
		
		Print(string.Format("[RL_ControlZoneEntity] StartCapture called - Gang: %1, Current controlling gang: %2, Capture in progress: %3", 
			gangId, m_iControllingGangId, m_bCaptureInProgress));
		
		if (m_bCaptureInProgress)
		{
			Print(string.Format("[RL_ControlZoneEntity] StartCapture blocked - capture already in progress"));
			return;
		}
			
		if (m_iControllingGangId == gangId)
		{
			Print(string.Format("[RL_ControlZoneEntity] StartCapture blocked - gang %1 already controls zone", gangId));
			return;
		}
		
		m_iCapturingGangId = gangId;
		m_bCaptureInProgress = true;
		m_fCaptureStartTime = GetGame().GetWorld().GetWorldTime() / 1000;
		m_fRemainingCaptureTime = m_iCaptureTime;
		
		// Get gang name from cache or fetch it
		if (m_gangNameCache && m_gangNameCache.Contains(gangId.ToString()))
		{
			m_sCapturingGangName = m_gangNameCache.Get(gangId.ToString());
			string gangName = m_sCapturingGangName;
			NotifyPlayersInZone(string.Format("%1 started capturing %2", gangName, m_sZoneName));
			Print(string.Format("[RL_ControlZoneEntity] Gang %1 (%2) started capturing zone %3", gangId, gangName, m_sZoneName));
		}
		else
		{
			m_sCapturingGangName = string.Format("Gang %1", gangId);
			m_bPendingCaptureNotification = true;
			FetchGangName(gangId);
			Print(string.Format("[RL_ControlZoneEntity] Gang %1 started capturing zone %2 (fetching gang name...)", gangId, m_sZoneName));
		}
		
		Replication.BumpMe();
		
		// Update map marker on all clients
		CallUpdateMapMarkerRPC();
		
		GetGame().GetCallqueue().CallLater(CheckCaptureProgress, m_iCaptureCheckInterval, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void CheckCaptureProgress()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bCaptureInProgress)
		{
			GetGame().GetCallqueue().Remove(CheckCaptureProgress);
			return;
		}
		
		// Check if at least one gang member is still in zone and alive
		array<IEntity> entitiesInside = {};
		GetEntitiesInside(entitiesInside);
		
		bool hasValidGangMember = false;
		foreach (IEntity entity : entitiesInside)
		{
			Print("[RL_ControlZoneEntity] CheckCaptureProgress foreach loop");
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
			if (!character)
				continue;
				
			if (character.GetGangId() != m_iCapturingGangId)
				continue;
				
			if (character.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
				continue;
				
			hasValidGangMember = true;
			break;
		}
		
		if (!hasValidGangMember)
		{
			FailCapture();
			return;
		}
		
		// Check if capture time has elapsed
		float currentTime = GetGame().GetWorld().GetWorldTime() / 1000;
		float elapsedTime = currentTime - m_fCaptureStartTime;
		m_fRemainingCaptureTime = Math.Max(0, m_iCaptureTime - elapsedTime);
		
		if (elapsedTime >= m_iCaptureTime)
		{
			CompleteCapture();
		}
		else
		{
			Replication.BumpMe();
			CallUpdateMapMarkerRPC();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void FailCapture()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bCaptureInProgress)
			return;
			
		string gangName = m_sCapturingGangName;
		
		NotifyPlayersInZone(string.Format("%1 failed to capture %2", gangName, m_sZoneName));
		
		Print(string.Format("[RL_ControlZoneEntity] Gang %1 (%2) failed to capture zone %3", m_iCapturingGangId, gangName, m_sZoneName));
		
		m_bCaptureInProgress = false;
		m_iCapturingGangId = -1;
		m_sCapturingGangName = "";
		m_fCaptureStartTime = 0;
		m_fRemainingCaptureTime = 0;
		m_bPendingCaptureNotification = false;
		
		Replication.BumpMe();
		
		GetGame().GetCallqueue().Remove(CheckCaptureProgress);
		
		// Update map marker on all clients
		CallUpdateMapMarkerRPC();
	}
	
	//------------------------------------------------------------------------------------------------
	void CompleteCapture()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bCaptureInProgress)
			return;
			
		string gangName = m_sCapturingGangName;
		
		NotifyPlayersInZone(string.Format("%1 successfully captured %2", gangName, m_sZoneName));
		
		Print(string.Format("[RL_ControlZoneEntity] Gang %1 (%2) successfully captured zone %3", m_iCapturingGangId, gangName, m_sZoneName));
		
		m_iControllingGangId = m_iCapturingGangId;
		m_sControllingGangName = m_sCapturingGangName;
		m_bCaptureInProgress = false;
		m_iCapturingGangId = -1;
		m_sCapturingGangName = "";
		m_fCaptureStartTime = 0;
		m_fRemainingCaptureTime = 0;
		m_bPendingCaptureNotification = false;

		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_ControlZoneFeesManager feesManager = RL_ControlZoneFeesManager.Cast(gameMode.FindComponent(RL_ControlZoneFeesManager));
		if (feesManager)
			feesManager.ClearDeniedFeesForZone(m_iZoneId);
		
		Replication.BumpMe();
		
		GetGame().GetCallqueue().Remove(CheckCaptureProgress);
		
		// Update map marker on all clients
		CallUpdateMapMarkerRPC();
		
		SaveControlZoneState();
	}
	
	//------------------------------------------------------------------------------------------------
	void NotifyPlayersInZone(string message)
	{
		array<IEntity> entitiesInside = {};
		GetEntitiesInside(entitiesInside);
		
		foreach (IEntity entity : entitiesInside)
		{
			Print("[RL_ControlZoneEntity] NotifyPlayersInZone foreach loop");
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
			if (!character)
				continue;
				
			character.Notify(message, "CONTROL ZONE");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	string GetGangName(int gangId)
	{
		if (gangId == -1)
			return "Unknown";
		
		if (m_gangNameCache && m_gangNameCache.Contains(gangId.ToString()))
			return m_gangNameCache.Get(gangId.ToString());
		
		if (Replication.IsServer())
		{
			FetchGangName(gangId);
		}
		
		return string.Format("Gang %1", gangId);
	}
	
	//------------------------------------------------------------------------------------------------
	void FetchGangName(int gangId)
	{
		if (!m_gangDbHelper)
			m_gangDbHelper = new RL_GangDbHelper();
		
		m_gangDbHelper.GetGang(gangId, this, "OnGangNameFetched");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnGangNameFetched(bool success, string results)
	{
		if (!success)
		{
			Print(string.Format("[RL_ControlZoneEntity] Failed to fetch gang name"));
			return;
		}
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		
		ref RL_GangData gangData = new RL_GangData();
		loadContext.ReadValue("data", gangData);
		
		if (gangData && gangData.GetGangId() > 0)
		{
			if (!m_gangNameCache)
				m_gangNameCache = new map<string, string>();
			
			string gangIdStr = gangData.GetGangId().ToString();
			string gangName = gangData.GetName();
			m_gangNameCache.Set(gangIdStr, gangName);
			
			// Update replicated gang names
			if (m_iCapturingGangId == gangData.GetGangId())
			{
				m_sCapturingGangName = gangName;
			}
			if (m_iControllingGangId == gangData.GetGangId())
			{
				m_sControllingGangName = gangName;
			}
			
			if (m_bPendingCaptureNotification && m_bCaptureInProgress && m_iCapturingGangId == gangData.GetGangId())
			{
				NotifyPlayersInZone(string.Format("%1 started capturing %2", gangName, m_sZoneName));
				Print(string.Format("[RL_ControlZoneEntity] Gang %1 (%2) started capturing zone %3", gangData.GetGangId(), gangName, m_sZoneName));
				m_bPendingCaptureNotification = false;
			}
			
			Replication.BumpMe();
			
			// Update map marker on all clients
			CallUpdateMapMarkerRPC();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateMapMarker()
	{
		if (!m_ownerEntity)
			return;
			
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		m_ownerEntity.GetTransform(spawnParams.Transform);
		
		GenericEntity markerEntity = GenericEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load("{E595423ADC263333}Prefabs/World/Locations/Common/RL_MapMarker.et"), m_ownerEntity.GetWorld(), spawnParams));
		if (!markerEntity)
			return;
			
		m_mapMarker = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
		if (!m_mapMarker)
		{
			delete markerEntity;
			return;
		}
		
		float sphereRadius = GetSphereRadius();
		
		Color markerColor = new Color(0.5, 0.5, 0.5, 0.2); // Gray for neutral
		m_mapMarker.SetMarkerProperties(GetMarkerText(), "OBJECTIVE", markerColor);
		
		m_mapMarker.SetUseControlZoneLayout(true);
		m_mapMarker.SetRadius(Math.Round(sphereRadius));
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateMapMarker()
	{
		if (!m_mapMarker)
			return;
			
		string text = GetMarkerText();
		m_mapMarker.SetText(text);
		
		Color markerColor;
		if (m_bCaptureInProgress)
		{
			markerColor = new Color(1.0, 0.5, 0.0, 0.3); // Dark orange for capturing
		}
		else if (m_iControllingGangId != -1)
		{
			markerColor = new Color(1.0, 0.0, 0.0, 0.3); // Red for controlled
		}
		else
		{
			markerColor = new Color(0.5, 0.5, 0.5, 0.2); // Gray for neutral
		}
		
		m_mapMarker.SetMarkerProperties(text, "OBJECTIVE", markerColor);
		
		// Always black text
		if (m_mapMarker.GetWidget())
		{
			RichTextWidget textWidget = RichTextWidget.Cast(m_mapMarker.GetWidget().FindAnyWidget("Text0"));
			if (textWidget)
			{
				textWidget.SetColor(new Color(0.0, 0.0, 0.0, 1.0));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	string GetMarkerText()
	{
		if (m_bCaptureInProgress)
		{
			string gangName = m_sCapturingGangName;
			if (gangName.IsEmpty())
				gangName = string.Format("Gang %1", m_iCapturingGangId);
			
			string formattedTime = RL_Utils.FormatTimeDisplay(m_fRemainingCaptureTime);
			return string.Format("%1: %2 capturing: %3", m_sZoneName, gangName, formattedTime);
		}
		else if (m_iControllingGangId != -1)
		{
			string gangName = m_sControllingGangName;
			if (gangName.IsEmpty())
				gangName = string.Format("Gang %1", m_iControllingGangId);
			
			return string.Format("%1: controlled by %2", m_sZoneName, gangName);
		}
		else
		{
			return string.Format("%1: neutral", m_sZoneName);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanStartCapture(SCR_ChimeraCharacter character)
	{
		if (!character)
			return false;
			
		if (m_bCaptureInProgress)
			return false;
			
		int gangId = character.GetGangId();
		if (gangId == -1)
			return false; // Not in a gang
			
		if (m_iControllingGangId == gangId)
			return false; // Already controlled by this gang
			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadControlZoneState()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_dbHelper) m_dbHelper = new RL_ControlZoneDbHelper();
		string zoneIdStr = m_iZoneId.ToString();
		m_dbHelper.GetControlZone(zoneIdStr, this, "OnControlZoneLoaded");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnControlZoneLoaded(bool success, string results)
	{
		if (!success)
		{
			return;
		}
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		
		int gangIdControlling = -1;
		loadContext.ReadValue("gangIdControlling", gangIdControlling);
		
		if (gangIdControlling != -1)
		{
			m_iControllingGangId = gangIdControlling;
			m_sControllingGangName = string.Format("Gang %1", gangIdControlling);
			
			FetchGangName(m_iControllingGangId);
		}
		else
		{
			m_iControllingGangId = -1;
			m_sControllingGangName = "";
		}
		
		Replication.BumpMe();
		
		// Update map marker on all clients
		CallUpdateMapMarkerRPC();
	}
	
	//------------------------------------------------------------------------------------------------
	void SaveControlZoneState()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_dbHelper)
			m_dbHelper = new RL_ControlZoneDbHelper();
			
		string zoneIdStr = m_iZoneId.ToString();
		int gangIdToSave;
		if (m_iControllingGangId == -1)
		{
			gangIdToSave = -1;
		}
		else
		{
			gangIdToSave = m_iControllingGangId;
		}
		
		m_dbHelper.UpsertControlZone(zoneIdStr, m_sZoneName, gangIdToSave, this, "OnControlZoneSaved");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnControlZoneSaved(bool success, string results)
	{
		if (success)
		{
			Print(string.Format("[RL_ControlZoneEntity] Successfully saved control zone %1 to database", m_iZoneId));
		}
		else
		{
			Print(string.Format("[RL_ControlZoneEntity] Failed to save control zone %1 to database", m_iZoneId));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_UpdateMapMarker()
	{
		Print("RpcDo_UpdateMapMarker");
		UpdateMapMarker();
	}
	
	//------------------------------------------------------------------------------------------------
	void CallUpdateMapMarkerRPC()
	{
		Rpc(RpcDo_UpdateMapMarker);
	}
}
