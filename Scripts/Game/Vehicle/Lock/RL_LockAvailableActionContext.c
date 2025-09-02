[BaseContainerProps(), SCR_AvailableActionContextTitle()]
class RL_LockAvailableActionContext : SCR_AvailableActionContext
{
	protected int m_fUpdateInterval = 2;
	protected int m_fLastUpdateTime = 0;
	protected RL_VehicleManagerComponent m_vehicleComp;
    protected SCR_ChimeraCharacter m_character;


	override string GetUIName()
	{
        if(!m_vehicleComp || !m_character)
            CacheComps();

		if(m_vehicleComp && m_character && m_vehicleComp.IsLocked())
		{
			return "Unlock Vehicle";
		} else {
            return "Lock Vehicle";
        }
		return "";
	}
	void CacheComps()
	{

		if ((System.GetUnixTime() - m_fLastUpdateTime) < m_fUpdateInterval)
			return;
		m_fLastUpdateTime = System.GetUnixTime();
		
		m_character = RL_Utils.GetLocalCharacter();
		if(!m_character)
			return;
			
		IEntity userVeh = CompartmentAccessComponent.GetVehicleIn(m_character);
		if(!userVeh)
			return;
	    
        m_vehicleComp = RL_VehicleManagerComponent.Cast(userVeh.FindComponent(RL_VehicleManagerComponent));;

    }
    bool m_checkedForOwner = false;
    bool m_isOwner = false;
	override bool IsAvailable(SCR_AvailableActionsConditionData data, float timeSlice)
	{
        if (!super.IsAvailable(data, timeSlice))
			return false;

        if(!m_vehicleComp || !m_character)
            CacheComps();
            
		if(m_vehicleComp && m_character && m_vehicleComp.IsOwner(m_character))
		{
            return true;
        }
        return false;

	}
}