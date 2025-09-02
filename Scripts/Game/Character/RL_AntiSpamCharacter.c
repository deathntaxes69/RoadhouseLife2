modded class SCR_ChimeraCharacter
{
    protected int m_iTimeBetweenActions = 1250; // in ms
	protected int m_iLastActionTime = -1;
	bool IsSpamming(bool silent = false)
	{
		int currentTime = System.GetTickCount();
		int diff;

		if (m_iLastActionTime == -1)
			diff = m_iTimeBetweenActions + 1; // Ensure first action isn't blocked
		else
			diff = currentTime - m_iLastActionTime;


		if (diff < m_iTimeBetweenActions)
		{
			if (!silent)
				RL_Utils.Notify("Canceled, slow down", "ACTION");
			return true;
		}

		m_iLastActionTime = currentTime;

		return false;
    }
}