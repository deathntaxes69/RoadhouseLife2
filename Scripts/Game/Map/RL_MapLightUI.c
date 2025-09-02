modded class SCR_MapLightUI
{
	override protected void UpdateTime()
	{
		if (!m_wLightOverlay || !m_wLightCone)
			return;
		
		if (!m_TimeMgr)
			return;
		
		float time = m_TimeMgr.GetTimeOfTheDay();
				
		m_bIsDark = false;
		m_wLightOverlay.SetVisible(m_bIsDark);
	}
}