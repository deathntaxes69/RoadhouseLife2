[BaseContainerProps(), SCR_AvailableActionContextTitle()]
class RL_TowAvailableActionContext : SCR_AvailableActionContext
{
	protected float m_fUpdateInterval = 1;
	protected float m_fLastUpdateTime = 0;
	protected RL_TowComponent m_towComp;


	override string GetUIName()
	{
		if(GetTowComp())
		{


			if(GetTowComp() && GetTowComp().IsTrailerOrTrailerTruck())
			{ 
				if(GetTowComp().GetAttachedEntity())
					return "Detach Tow";
		
				return "Attach Tow";
			}
		}
		return "BUG";
	}
	RL_TowComponent GetTowComp()
	{
		if(m_towComp)
			return m_towComp;


		if ((System.GetUnixTime() - m_fLastUpdateTime) < m_fUpdateInterval)
			return null;
		m_fLastUpdateTime = System.GetUnixTime();
		
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if(!character)
			return null;
		IEntity userVeh = CompartmentAccessComponent.GetVehicleIn(character);
		if(!userVeh)
			return null;
		m_towComp = RL_TowComponent.Cast(userVeh.FindComponent(RL_TowComponent));
		return m_towComp;
	}
	override bool IsAvailable(SCR_AvailableActionsConditionData data, float timeSlice)
	{
		if (!super.IsAvailable(data, timeSlice))
			return false;

		if(GetTowComp() && GetTowComp().IsTrailerOrTrailerTruck())
		{ 
			return true;
		}
		
		return false;

	}
}