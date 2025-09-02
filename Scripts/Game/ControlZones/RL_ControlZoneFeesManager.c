//------------------------------------------------------------------------------------------------
class RL_DeniedFeeEntry
{
	protected string m_sCharacterId;
	protected float m_fDenialTime;
	
	void RL_DeniedFeeEntry(string characterId, float denialTime)
	{
		m_sCharacterId = characterId;
		m_fDenialTime = denialTime;
	}
	
	string GetCharacterId() { return m_sCharacterId; }
	float GetDenialTime() { return m_fDenialTime; }
}

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/ControlZones", description: "Manages denied fees tracking for control zones")]
class RL_ControlZoneFeesManagerClass: ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class RL_ControlZoneFeesManager : ScriptComponent
{
	protected ref map<int, ref array<ref RL_DeniedFeeEntry>> m_mDeniedFees;
	protected static const float DENIED_FEE_TIMEOUT = 600000.0;
	protected static const int MARKER_UPDATE_TIME = 30000;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
			
		m_mDeniedFees = new map<int, ref array<ref RL_DeniedFeeEntry>>;

		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(CreateMarkersForAllDeniedFees, MARKER_UPDATE_TIME, true);
		}
		
		Print("[RL_ControlZoneFeesManager] Initialized");
	}

	//------------------------------------------------------------------------------------------------
	void CreateMarkersForAllDeniedFees()
	{
		if (!Replication.IsServer())
			return;
		
		foreach (int zoneId, array<ref RL_DeniedFeeEntry> deniedList : m_mDeniedFees)
		{
			Print("[RL_ControlZoneFeesManager] CreateMarkersForAllDeniedFees foreach loop");
			foreach (RL_DeniedFeeEntry entry : deniedList)
			{
				Print("[RL_ControlZoneFeesManager] CreateMarkersForAllDeniedFees foreach loop 2");
				CreateMarkerForEntry(zoneId, entry);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	void CreateMarkerForEntry(int zoneId, RL_DeniedFeeEntry entry)
	{
		if (!Replication.IsServer())
			return;
			
		RL_ControlZoneEntity controlZone = RL_ControlZoneManager.GetInstance().FindControlZoneById(zoneId);
		if (!controlZone)
		{
			PrintFormat("[RL_ControlZoneFeesManager] Control zone %1 not found for marker creation", zoneId);
			return;
		}
		
		int controllingGangId = controlZone.GetControllingGangId();
		if (controllingGangId <= 0)
		{
			PrintFormat("[RL_ControlZoneFeesManager] Zone %1 has no controlling gang, skipping marker creation", zoneId);
			return;
		}
		
		SCR_ChimeraCharacter character = RL_Utils.GetCharacterByCharacterIdLocal(entry.GetCharacterId());
		if (!character)
		{
			PrintFormat("[RL_ControlZoneFeesManager] Character %1 not found for marker creation", entry.GetCharacterId());
			return;
		}
		
		vector location = character.GetOrigin();
		string characterName = character.GetCharacterName();
		
		if (location == vector.Zero)
		{
			PrintFormat("[RL_ControlZoneFeesManager] Invalid position for character %1, skipping marker", entry.GetCharacterId());
			return;
		}
		
		IEntity markerEntity = RL_MapUtils.CreateMarkerServer(
			location, 
			string.Format("%1 Denied Fees", characterName), 
			"DEATH", 
			Color.Red, 
			MARKER_UPDATE_TIME,
			RL_MARKER_TYPE.GangOnly
		);
		
		if (markerEntity)
		{
			RL_MapMarker markerComponent = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
			if (markerComponent)
			{
				markerComponent.SetGangId(controllingGangId);
				PrintFormat("[RL_ControlZoneFeesManager] Successfully created gang only marker for character %1 (gang %2)", entry.GetCharacterId(), controllingGangId);
			}
			else
			{
				PrintFormat("[RL_ControlZoneFeesManager] Failed to get marker component for character %1", entry.GetCharacterId());
				RL_MapUtils.DeleteMarkerEntity(markerEntity);
			}
		}
		else
		{
			PrintFormat("[RL_ControlZoneFeesManager] Failed to create marker for character %1", entry.GetCharacterId());
		}
	}

	//------------------------------------------------------------------------------------------------
	void AddDeniedFee(int zoneId, string characterId)
	{
		if (!Replication.IsServer())
			return;

		float currentTime = GetGame().GetWorld().GetWorldTime() / 1000;
		
		if (!m_mDeniedFees.Contains(zoneId))
		{
			m_mDeniedFees.Set(zoneId, new array<ref RL_DeniedFeeEntry>);
		}
		
		array<ref RL_DeniedFeeEntry> deniedList = m_mDeniedFees.Get(zoneId);
		
		foreach (RL_DeniedFeeEntry entry : deniedList)
		{
			Print("[RL_ControlZoneFeesManager] AddDeniedFee foreach loop");
			if (entry.GetCharacterId() == characterId)
				return;
		}
		
		RL_DeniedFeeEntry newEntry = new RL_DeniedFeeEntry(characterId, currentTime);
		deniedList.Insert(newEntry);
		
		
		CreateMarkerForEntry(zoneId, newEntry);
		GetGame().GetCallqueue().CallLater(RemoveDeniedFee, DENIED_FEE_TIMEOUT, false, zoneId, characterId);
	}
	
	//------------------------------------------------------------------------------------------------
	void RemoveDeniedFee(int zoneId, string characterId)
	{
		if (!Replication.IsServer())
			return;

		if (!m_mDeniedFees.Contains(zoneId))
			return;
			
		array<ref RL_DeniedFeeEntry> deniedList = m_mDeniedFees.Get(zoneId);
		
		for (int i = deniedList.Count() - 1; i >= 0; i--)
		{
			Print("[RL_ControlZoneFeesManager] RemoveDeniedFee for loop");
			if (deniedList[i].GetCharacterId() == characterId)
			{
				deniedList.RemoveOrdered(i);
				break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearDeniedFeesForZone(int zoneId)
	{
		if (!Replication.IsServer())
			return;

		if (!m_mDeniedFees.Contains(zoneId))
			return;
			
		array<ref RL_DeniedFeeEntry> deniedList = m_mDeniedFees.Get(zoneId);
		int clearedCount = deniedList.Count();
		
		deniedList.Clear();
	}
	
} 