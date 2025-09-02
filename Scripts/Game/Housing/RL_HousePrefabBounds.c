class RL_HousePrefabBounds
{
	protected vector m_vBoundsMin;
	protected vector m_vBoundsMax;
	
	//------------------------------------------------------------------------------------------------
	void SetBounds(vector boundsMin, vector boundsMax)
	{
		m_vBoundsMin = boundsMin;
		m_vBoundsMax = boundsMax;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetBounds(out vector boundsMin, out vector boundsMax)
	{
		boundsMin = m_vBoundsMin;
		boundsMax = m_vBoundsMax;
		return true;
	}
}