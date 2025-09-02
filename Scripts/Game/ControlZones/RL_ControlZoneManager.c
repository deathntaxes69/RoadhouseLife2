class RL_ControlZoneManager
{
	protected static ref RL_ControlZoneManager s_Instance;
	protected ref array<RL_ControlZoneEntity> m_aControlZones;
	
	static RL_ControlZoneManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new RL_ControlZoneManager();
		return s_Instance;
	}
	
	void RL_ControlZoneManager()
	{
		m_aControlZones = new array<RL_ControlZoneEntity>;
		Print("[RL_ControlZoneManager] Initialized");
	}
	
	void RegisterControlZone(RL_ControlZoneEntity controlZone)
	{
		if (!controlZone || m_aControlZones.Contains(controlZone))
			return;
		
		m_aControlZones.Insert(controlZone);
		Print(string.Format("[RL_ControlZoneManager] Registered control zone: %1 (ID: %2)", controlZone.GetZoneName(), controlZone.GetZoneId()));
	}
	
	void UnregisterControlZone(RL_ControlZoneEntity controlZone)
	{
		if (!controlZone)
			return;
		
		int index = m_aControlZones.Find(controlZone);
		if (index != -1)
		{
			m_aControlZones.Remove(index);
			Print(string.Format("[RL_ControlZoneManager] Unregistered control zone: %1 (ID: %2)", controlZone.GetZoneName(), controlZone.GetZoneId()));
		}
	}
	
	RL_ControlZoneEntity FindControlZoneById(int zoneId)
	{
		foreach (RL_ControlZoneEntity controlZone : m_aControlZones)
		{
			Print("[RL_ControlZoneManager] FindControlZoneById foreach loop");
			if (!controlZone)
				continue;
				
			if (controlZone.GetZoneId() == zoneId)
				return controlZone;
		}
		return null;
	}
} 