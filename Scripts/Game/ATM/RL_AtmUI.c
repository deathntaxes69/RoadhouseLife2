class RL_AtmUI : ChimeraMenuBase
{
    protected SCR_ChimeraCharacter m_character;
	protected Widget m_wRoot;
    protected TextWidget m_bankBalanceValue;
    protected SCR_EditBoxComponent m_amountInput;
    protected SCR_InputButtonComponent m_withdrawButton;
    protected SCR_InputButtonComponent m_depositButton;
    protected SCR_InputButtonComponent m_depositAllButton;
    protected SCR_InputButtonComponent m_transferButton;
    protected SCR_ComboBoxComponent m_playerListCombo;
    protected int m_iCachedBankBalance;
    protected ref array<int> m_playerList;
    protected ref array<string> m_nameList;

	override void OnMenuOpen() 
	{
        m_wRoot = GetRootWidget();
        m_bankBalanceValue = TextWidget.Cast(m_wRoot.FindAnyWidget("bankBalanceValue"));

        m_amountInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("AmountEditBox").FindHandler(SCR_EditBoxComponent));

        m_withdrawButton = SCR_InputButtonComponent.GetInputButtonComponent("WithdrawButton", m_wRoot);
        if (m_withdrawButton)
            m_withdrawButton.m_OnActivated.Insert(OnTransaction);
        
        m_depositButton = SCR_InputButtonComponent.GetInputButtonComponent("DepositButton", m_wRoot);
        if (m_depositButton)
            m_depositButton.m_OnActivated.Insert(OnTransaction);
        
        m_depositAllButton = SCR_InputButtonComponent.GetInputButtonComponent("DepositAllButton", m_wRoot);
        if (m_depositAllButton)
            m_depositAllButton.m_OnActivated.Insert(OnTransaction);
        
        m_transferButton = SCR_InputButtonComponent.GetInputButtonComponent("TransferButton", m_wRoot);
        if (m_transferButton)
            m_transferButton.m_OnActivated.Insert(OnTransaction);

        Widget playerListWidget = Widget.Cast(m_wRoot.FindAnyWidget("playerList"));
        if (playerListWidget)
        {
            m_playerListCombo = SCR_ComboBoxComponent.Cast(playerListWidget.FindHandler(SCR_ComboBoxComponent));
        }

        m_character = RL_Utils.GetLocalCharacter();
        if(!m_character)
            return;

		m_character.GetOnAccountUpdated().Insert(OnAccountUpdated);
        m_character.TransactMoney();

        TextWidget welcomeContent = TextWidget.Cast(m_wRoot.FindAnyWidget("welcomeContent"));
        welcomeContent.SetText(m_character.GetCharacterName());
        m_character.GetPlayerList(this, "FillPlayerList");

        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);

    }
    
    void FillPlayerList(array<int> playerList, array<string> nameList)
    {
        if (!m_playerListCombo)
            return;
            
        m_playerList = playerList;
        m_nameList = nameList;
        
        m_playerListCombo.ClearAll();
        
        foreach(string fullName : nameList) {
            Print("[RL_AtmUI] FillPlayerList foreach loop");
            m_playerListCombo.AddItem(fullName, false);
        }
    }
    
    void OnAccountUpdated()
    {
        UpdateBankBalance(m_character.GetBank());
    }
    void UpdateBankBalance(int bankBalance)
    {
        m_bankBalanceValue.SetText(RL_Utils.FormatMoney(bankBalance));
    }
    void OnTransaction(SCR_InputButtonComponent w)
	{
        if(!m_character || m_character.IsSpamming()) return;

        int currentCash = m_character.GetCash();
        int currentBank = m_character.GetBank();
        int inputAmount = m_amountInput.GetValue().ToInt();
        if((!inputAmount || inputAmount < 1) && (w == m_withdrawButton || w == m_depositButton || w == m_transferButton))
        {
            RL_Utils.Notify("Transaction amount must be greater than $0.", "ATM");
            return;
        }
        if ( w == m_withdrawButton )
        {
            if(inputAmount > currentBank)
            {
                RL_Utils.Notify("You dont have enough in your bank, get a job!", "ATM");
                return;
            }
            m_character.TransactMoney( inputAmount, -inputAmount);
        }
        if ( w == m_depositButton )
        {
            if(inputAmount > currentCash)
            {
                RL_Utils.Notify("You don't have enough cash to deposit.", "ATM");
                return;
            }
            m_character.TransactMoney( -inputAmount, inputAmount);
        }
        if ( w == m_depositAllButton )
        { 
            if(currentCash < 1)
            {
                RL_Utils.Notify("You don't have any cash to deposit.", "ATM");
                return;
            }
            m_character.TransactMoney( -currentCash, currentCash);
        }
        if ( w == m_transferButton )
        {
            if(!m_playerListCombo || !m_playerList || !m_nameList)
            {
                RL_Utils.Notify("Player list not loaded. Please try again.", "ATM");
                return;
            }
            
            int selectedIndex = m_playerListCombo.GetCurrentIndex();
            if(selectedIndex < 0 || selectedIndex >= m_playerList.Count())
            {
                RL_Utils.Notify("Please select a player to transfer to.", "ATM");
                return;
            }
            
            if(inputAmount > currentBank)
            {
                RL_Utils.Notify("You don't have enough in your bank to transfer.", "ATM");
                return;
            }
            
            int arrayIndex = selectedIndex;
            int targetPlayerId = m_playerList[arrayIndex];
            string targetPlayerName = m_nameList[arrayIndex];
            
            OnTransferMoney(targetPlayerId, targetPlayerName, inputAmount);
        }
		m_character.PlayCommonSoundLocally("SOUND_ATMBEEP");

        return;
    }
    
    void OnTransferMoney(int targetPlayerId, string targetPlayerName, int amount)
    {
        m_character.AtmTransferMoney(targetPlayerId, targetPlayerName, amount);
    }
    
    void SetEntities(IEntity owner, IEntity player)
	{
        m_character = SCR_ChimeraCharacter.Cast(player);
	}
    void ~RL_AtmUI()
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