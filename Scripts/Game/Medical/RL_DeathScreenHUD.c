
class RL_DeathScreenHUD : SCR_InfoDisplayExtended {
	
	protected static RL_DeathScreenHUD m_hud;
    protected Widget m_overlay;
	protected TextWidget m_BleedingOutTimer;

    protected SCR_ChimeraCharacter m_character;
	protected SCR_CharacterDamageManagerComponent m_DamageManager;
	protected CharacterControllerComponent m_Controller;

    protected SCR_InputButtonComponent m_respawnButton;
    protected TextWidget m_bleedingOutTimer;
	protected TextWidget m_killerCharacterId;
	protected TextWidget m_killedTime;

	protected float m_fLastTimeLeft;
	protected bool m_bInitialized = false;

	protected IEntity m_Character;

	override bool DisplayStartDrawInit(IEntity owner) {
		m_hud = this;
		PlayerController player = GetGame().GetPlayerController();
		return player != null;
	}

	static RL_DeathScreenHUD GetCurrentInstance()
	{
		return m_hud;
	}
	override protected void DisplayControlledEntityChanged(IEntity from, IEntity to) {
		Print("DisplayControlledEntityChanged");
		if (!m_wRoot)
			return;

		m_Character = to;
		Print("DisplayControlledEntityChanged 1");
		if (!m_Character)
			return;

		m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(m_Character.FindComponent(SCR_CharacterDamageManagerComponent));
		m_Controller = CharacterControllerComponent.Cast(m_Character.FindComponent(CharacterControllerComponent));

		m_overlay = Widget.Cast(m_wRoot.FindAnyWidget("infoOverlay"));
		m_BleedingOutTimer = TextWidget.Cast(m_wRoot.FindAnyWidget("BleedingOutTimer"));
		if (!m_overlay || !m_BleedingOutTimer)
			return;
		Print("DisplayControlledEntityChanged 2");
        m_killerCharacterId = TextWidget.Cast(m_wRoot.FindAnyWidget("KillerCharacterId"));
		m_killedTime = TextWidget.Cast(m_wRoot.FindAnyWidget("Time"));
		m_respawnButton = SCR_InputButtonComponent.GetInputButtonComponent("RespawnButton", m_wRoot);
        if (m_respawnButton)
            m_respawnButton.m_OnActivated.Insert(OnSelectRespawn);
		Print("DisplayControlledEntityChanged 3");

		ToggleRespawnButton(false);
		Print(m_overlay);
		m_overlay.SetVisible(false);

		m_bInitialized = (m_DamageManager && m_Controller && m_BleedingOutTimer);
	}

	override protected void DisplayUpdate(IEntity owner, float timeSlice) {
		if (!m_bInitialized || !GetGame().InPlayMode() || !m_overlay || !m_DamageManager)
			return;
		
		// Replicated cpr variable might be out of date, so we also check local respawn bool
		bool shouldBeVisible = m_DamageManager.IsWaitingForCpr() && !m_DamageManager.HasRespawned();

		if (m_overlay.IsVisible() != shouldBeVisible)
		{
			m_overlay.SetVisible(shouldBeVisible);
			if(!shouldBeVisible)
				ToggleRespawnButton(shouldBeVisible);

			//RL_Utils.GetLocalCharacter().ResetAnimationVars();
			//auto menuManager = GetGame().GetMenuManager();
			//if (menuManager && (menuManager.IsAnyMenuOpen() || menuManager.IsAnyDialogOpen()))
			//	menuManager.CloseAllMenus();
		}

		if (!shouldBeVisible)
			return;

		if(!m_Controller.IsUnconscious())
			m_DamageManager.ForceUnconsciousness();

		if(!m_Controller.IsUnconscious())
			Print("[RL_DeathScreenHUD] unconscious shit aint working");

		float timeLeft = m_DamageManager.GetTimeLeftToDie();

		// Throttle Updates
		if (Math.AbsFloat(timeLeft - m_fLastTimeLeft) >= 0.9) {
			string formattedTime = SCR_FormatHelper.GetTimeFormatting(timeLeft, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS);
			m_BleedingOutTimer.SetText(formattedTime);
			m_fLastTimeLeft = timeLeft;
		}
		if (m_DamageManager.GetTimeLeftToRespawn() == 0)
		{
			GetGame().GetInputManager().ActivateContext("MenuContext");
			ToggleRespawnButton(true);
		}
		if (timeLeft == 0)
		{
			m_DamageManager.LocalResetCprProps(true);
		}
	}
    void ToggleRespawnButton(bool state)
	{
		if (m_respawnButton.IsVisible() == state)
			return;

		m_respawnButton.SetVisible(state);
		m_respawnButton.SetEnabled(state);
	}
	void OnSelectRespawn(SCR_InputButtonComponent button)
	{
		if (m_character && m_character.IsSpamming())
			return;

		if(m_DamageManager.GetTimeLeftToRespawn() != 0)
			return;

		m_DamageManager.LocalResetCprProps(true);
	}
    void SetKillerProps(string killedByCharacterId)
	{
		if(!m_killerCharacterId || !m_killedTime)
			return;
		m_killerCharacterId.SetText(killedByCharacterId);
		m_killedTime.SetText(SCR_DateTimeHelper.GetDateTimeUTC());
	}

}
