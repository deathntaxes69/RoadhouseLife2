class RL_BaseAction : ScriptedUserAction
{
	protected const float CHECK_INTERVAL_MS = 1000.0;
	protected float m_fLastCanBeShownCheck = 0.0;
	protected float m_fLastCanBePerformedCheck = 0.0;
	protected bool m_bCachedCanBeShown = false;
	protected bool m_bCachedCanBePerformed = false;
	
	//------------------------------------------------------------------------------------------------
	override event bool CanBeShownScript(IEntity user)
	{
		float currentTime = System.GetTickCount();
		if (currentTime - m_fLastCanBeShownCheck >= CHECK_INTERVAL_MS)
		{
			m_fLastCanBeShownCheck = currentTime;
			m_bCachedCanBeShown = CanShowAction(user);
		}
		
		return m_bCachedCanBeShown;
	}
	
	//------------------------------------------------------------------------------------------------
	override event bool CanBePerformedScript(IEntity user)
	{
		float currentTime = System.GetTickCount();
		if (currentTime - m_fLastCanBePerformedCheck >= CHECK_INTERVAL_MS)
		{
			m_fLastCanBePerformedCheck = currentTime;
			m_bCachedCanBePerformed = CanPerformAction(user);
		}
		
		return m_bCachedCanBePerformed;
	}
	
	//------------------------------------------------------------------------------------------------
	// idk we prefer to just use super on the original method but i prefer this
	protected bool CanShowAction(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanPerformAction(IEntity user)
	{
		return true;
	}
}
