class RL_GiveMoneyUI : ChimeraMenuBase
{
	protected string m_sOtherCharacter;
	protected SCR_ChimeraCharacter m_character;
	protected SCR_ChimeraCharacter m_otherCharacter;
	protected Widget m_wRoot;
	protected TextWidget m_cashBalanceValue;
	protected SCR_EditBoxComponent m_amountInput;
	protected SCR_InputButtonComponent m_giveMoneyButton;

	override void OnMenuOpen()
	{
		m_wRoot = GetRootWidget();
		m_cashBalanceValue = TextWidget.Cast(m_wRoot.FindAnyWidget("cashBalanceValue"));
		m_amountInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("AmountEditBox").FindHandler(SCR_EditBoxComponent));
		m_giveMoneyButton = SCR_InputButtonComponent.GetInputButtonComponent("GiveMoneyButton", m_wRoot);
		if (m_giveMoneyButton)
			m_giveMoneyButton.m_OnActivated.Insert(OnTransaction);

		m_character = RL_Utils.GetLocalCharacter();
		if (!m_character)
			return;

		m_character.GetOnAccountUpdated().Insert(OnAccountUpdated);
		m_character.TransactMoney();

		UpdateCashBalance(m_character.GetCash());

		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
	}

	void OnAccountUpdated()
	{
		UpdateCashBalance(m_character.GetCash());
	}

	void UpdateCashBalance(int cashBalance)
	{
		m_cashBalanceValue.SetText(RL_Utils.FormatMoney(cashBalance));
	}

	void OnTransaction(SCR_InputButtonComponent w)
	{
		if (!m_otherCharacter || !m_sOtherCharacter || m_sOtherCharacter == "") {
			RL_Utils.Notify("Somethings broken, Contact developers: GMOT", "Give Money");
			CloseMenu();
			return;
		}

		if (m_character.IsSpamming())
			return;

		int currentCash = m_character.GetCash();
		int inputAmount = m_amountInput.GetValue().ToInt();
		if ((!inputAmount || inputAmount < 1) && (w == m_giveMoneyButton))
		{
			RL_Utils.Notify("Transaction amount must be greater than $0.", "Give Money");
			return;
		}

		if (w == m_giveMoneyButton) {
			if (inputAmount > currentCash) {
				RL_Utils.Notify("You dont have that much cash!", "Give Money");
				return;
			}

			if (!RL_Utils.WithinDistance(m_character, m_otherCharacter, 3)) {
				RL_Utils.Notify("Canceled, they walked too far away.", "Give Money");
				CloseMenu();
				return;
			}

			m_character.GaveMoneySvr(m_character.GetCharacterId(), m_otherCharacter.GetCharacterId(), inputAmount);

			CloseMenu();
			//m_character.PlayCommonSoundLocally("SOUND_ATMBEEP"); // todo find a sound
			return;
		}

		return;
	}

	void SetReceivingCharacterId(string receivingCid = "")
	{
		m_sOtherCharacter = receivingCid;
		m_otherCharacter = RL_Utils.GetCharacterByCharacterIdLocal(m_sOtherCharacter);
		if (!m_otherCharacter || m_sOtherCharacter == "")
			RL_Utils.Notify("Somethings broken, Contact developers: SRCID-" + m_sOtherCharacter, "Give Money");
	}

	void ~RL_GiveMoneyUI()
	{
		// Unsubscribe from event
		if (m_character)
			m_character.GetOnAccountUpdated().Remove(OnAccountUpdated);
	}

	void CloseMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}

}
