class RL_PayFeesUI : ChimeraMenuBase
{
	protected Widget m_wRoot;
	protected TextWidget m_wFeesAmount;
	protected TextWidget m_wProcesserName;
	protected TextWidget m_wControlZoneName;
	protected SCR_InputButtonComponent m_wPayButton;
	protected SCR_InputButtonComponent m_wDenyButton;
	
	protected RL_ProcessAction m_processAction;
	protected SCR_ChimeraCharacter m_character;
	protected ref ScriptInvoker m_OnFeesDecision;
	protected int m_controllingGangId = -1;
	protected string m_controllingGangName = "";
	protected bool m_bFeesAlreadyPaid = false;

	override void OnMenuOpen() 
	{ 
        super.OnMenuOpen();
        m_wRoot = GetRootWidget();
		
		m_wFeesAmount = TextWidget.Cast(m_wRoot.FindAnyWidget("Text0"));
		m_wProcesserName = TextWidget.Cast(m_wRoot.FindAnyWidget("gangName"));
		m_wControlZoneName = TextWidget.Cast(m_wRoot.FindAnyWidget("gangName"));
		
		SCR_InputButtonComponent payButton = SCR_InputButtonComponent.GetInputButtonComponent("payFees", m_wRoot);
		if (payButton)
		{
			m_wPayButton = payButton;
			m_wPayButton.m_OnActivated.Insert(OnPayFees);
		}
		
		SCR_InputButtonComponent denyButton = SCR_InputButtonComponent.GetInputButtonComponent("denyFees", m_wRoot);
		if (denyButton)
		{
			m_wDenyButton = denyButton;
			m_wDenyButton.m_OnActivated.Insert(OnDenyFees);
		}
		
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, OnDenyFees);
		
		if (!m_OnFeesDecision) m_OnFeesDecision = new ScriptInvoker();
    }
	
	void SetProcessAction(RL_ProcessAction processAction, SCR_ChimeraCharacter character, int gangId = -1, string gangName = "")
	{
		m_processAction = processAction;
		m_character = character;
		m_controllingGangId = gangId;
		m_controllingGangName = gangName;
		m_bFeesAlreadyPaid = false;
		UpdateUI();
	}
	
	void UpdateUI()
	{
		if (!m_processAction) return;
		
		if (m_wFeesAmount)
			m_wFeesAmount.SetText(RL_Utils.FormatMoney(m_processAction.GetProcessFee()));
		
		if (m_wProcesserName)
		{
			string displayName = m_controllingGangName;
			if (!displayName || displayName.IsEmpty())
				displayName = string.Format("Control Zone %1", m_processAction.GetControlZoneId());
			m_wProcesserName.SetText(displayName);
		}
	}
	
	void OnPayFees()
	{
		if (!m_processAction || !m_character) return;
		
		float fee = m_processAction.GetProcessFee();
		if (m_character.GetCash() < fee)
		{
			RL_Utils.Notify("Insufficient funds", "FEES");
			return;
		}
		
		if (m_controllingGangId == -1)
		{
			RL_Utils.Notify("No controlling gang found", "FEES");
			return;
		}
	
		m_character.GangTransaction(-fee, 0, fee, m_controllingGangId);	
		m_bFeesAlreadyPaid = true;
		m_OnFeesDecision.Invoke(true);
		CloseMenu();
	}
	
	void OnDenyFees()
	{
		if (!m_processAction || !m_character) 
		{
			m_OnFeesDecision.Invoke(false);
			CloseMenu();
			return;
		}

		if (m_bFeesAlreadyPaid)
		{
			m_OnFeesDecision.Invoke(false);
			CloseMenu();
			return;
		}

		string characterId = m_character.GetCharacterId();
		int controlZoneId = m_processAction.GetControlZoneId();
		m_character.AddDeniedFee(controlZoneId, characterId);
		
		if (m_controllingGangId != -1)
		{
			string processerName = m_processAction.GetProcesserName();
			if (!processerName || processerName.IsEmpty())
				processerName = string.Format("Control Zone %1", m_processAction.GetControlZoneId());
				
			string message = string.Format("%1 denied fees at %2", m_character.GetCharacterName(), processerName);
			m_character.NotifyGangMembers(m_controllingGangId, message);
		}
		
		m_OnFeesDecision.Invoke(false);
		CloseMenu();
	}
	
	ScriptInvoker GetOnFeesDecision()
	{
		return m_OnFeesDecision;
	}
	
    void CloseMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
}