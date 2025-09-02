[ComponentEditorProps(category: "RL/MajorCrime", description: "Major Crime Manager Component")]
class RL_MajorCrimeManagerComponentClass: ScriptComponentClass
{
}

class RL_MajorCrimeManagerComponent: ScriptComponent
{
	[RplProp()]
	protected int m_lastMajorCrimeTime = 0;
	
	[RplProp()]
	protected int m_nextMajorCrimeAllowedTime = 0;
	
	protected static const int MAJOR_CRIME_COOLDOWN_TIME = 5400;
	protected static const int MAJOR_CRIME_SERVER_START_COOLDOWN = 1800;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!Replication.IsServer())
			return;

		int currentTime = System.GetUnixTime();
		int initialLockoutEndsAt = currentTime + MAJOR_CRIME_SERVER_START_COOLDOWN;

		if (m_nextMajorCrimeAllowedTime < initialLockoutEndsAt)
		{
			m_nextMajorCrimeAllowedTime = initialLockoutEndsAt;
			Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanAnyMajorCrimeBePerformed()
	{
		int currentTime = System.GetUnixTime();
		bool canPerform = currentTime > m_nextMajorCrimeAllowedTime;
		return canPerform;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMajorCrimeCooldownRemaining()
	{
		int currentTime = System.GetUnixTime();
		int remaining = Math.Max(0, m_nextMajorCrimeAllowedTime - currentTime);
		return remaining;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMajorCrimeCommitted()
	{
		if (!Replication.IsServer())
			return;
		
		m_lastMajorCrimeTime = System.GetUnixTime();
		m_nextMajorCrimeAllowedTime = m_lastMajorCrimeTime + MAJOR_CRIME_COOLDOWN_TIME;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMajorCrimeCooldownTime()
	{
		return MAJOR_CRIME_COOLDOWN_TIME;
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetMajorCrimeCooldown()
	{
		if (!Replication.IsServer())
			return;
			
		m_lastMajorCrimeTime = 0;
		m_nextMajorCrimeAllowedTime = 0;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetLastMajorCrimeTime()
	{
		return m_lastMajorCrimeTime;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetNextMajorCrimeAllowedTime()
	{
		return m_nextMajorCrimeAllowedTime;
	}
}
