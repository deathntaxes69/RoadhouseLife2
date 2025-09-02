[EntityEditorProps(category: "GameScripted/TruckEvent", description: "Component for storing truck event dropoff location")]
class RL_TruckEventComponentClass : ScriptComponentClass
{
}

class RL_TruckEventComponent : ScriptComponent
{
	[RplProp()]
	protected vector m_vDropoffPosition;
	
	[RplProp()]
	protected bool m_bHasDropoffLocation = false;
	
	//------------------------------------------------------------------------------------------------
	void SetDropoffLocation(vector position)
	{
		m_vDropoffPosition = position;
		m_bHasDropoffLocation = true;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetDropoffLocation()
	{
		return m_vDropoffPosition;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasDropoffLocation()
	{
		return m_bHasDropoffLocation;
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearDropoffLocation()
	{
		m_vDropoffPosition = vector.Zero;
		m_bHasDropoffLocation = false;
		Replication.BumpMe();
	}
}