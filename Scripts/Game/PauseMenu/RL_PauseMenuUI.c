// RL_RespawnSystemComponent disables respawn button
// This mod is to delay exit if alive and auto close the menu when waiting for CPR
modded class PauseMenuUI {
    protected int m_secondsToDelayExit = 15;
	protected float m_openTime = 0;
	protected float m_updateTimer = 0;
	SCR_ButtonTextComponent m_exitButton;

	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		m_openTime = 0;
		m_updateTimer = 0;
		m_exitButton = SCR_ButtonTextComponent.GetButtonText("Exit", m_wRoot);


        SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
        if(!character) return;
        SCR_CharacterDamageManagerComponent dmc = EL_Component<SCR_CharacterDamageManagerComponent>.Find(character);
        // Close pause menu if waiting for cpr
        if(dmc && dmc.IsWaitingForCpr() && !SCR_Global.IsAdmin(SCR_PlayerController.GetLocalPlayerId()))
        {
            Close();
            return;
        }
        // Set disabled to start
		if (m_exitButton)
		{
			m_exitButton.SetEnabled(false);

			m_exitButton.SetText(string.Format("Exit to main menu (%1.0s)", m_secondsToDelayExit));
		}
	}

	override void OnMenuUpdate(float tDelta)
	{
        super.OnMenuUpdate(tDelta);

		if (!m_exitButton || m_exitButton.IsEnabled())
			return;

		m_openTime += tDelta;
		m_updateTimer += tDelta;

		if (m_openTime >= m_secondsToDelayExit)
		{
			m_exitButton.SetEnabled(true);
			m_exitButton.SetText("Exit to main menu");
			return;
		}

		if (m_updateTimer >= 0.1)
		{
			m_updateTimer = 0;
            float timeLeft = m_secondsToDelayExit - m_openTime;
	        int whole = Math.Floor(timeLeft);
	        int decimal = Math.Floor((timeLeft - whole) * 10 + 0.5);
	        m_exitButton.SetText(string.Format("Exit to main menu (%1.%2s)", whole, decimal));
		}
	}
	
	override private void OnPlayers()
	{
		PlatformService platformService = GetGame().GetPlatformService();
        platformService.OpenBrowser("https://discord.gg/theroadhouse");
        Close();
	}
}