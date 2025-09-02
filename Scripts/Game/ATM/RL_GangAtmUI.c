class RL_GangAtmUI : RL_AtmUI
{
    protected TextWidget m_gangBankBalanceValue;
    protected int m_iCachedGangBalance = 0;
    protected bool m_bGangBalanceLoaded = false;

    override void OnMenuOpen() 
    {
        super.OnMenuOpen();

        m_gangBankBalanceValue = TextWidget.Cast(m_wRoot.FindAnyWidget("bankBalanceValueGang"));
        if (!m_character || m_character.GetGangId() == -1)
        {
            RL_Utils.Notify("You must be in a gang to use this ATM.", "GANG ATM");
            CloseMenu();
            return;
        }
        
        m_character.GetGangBalance(this);
    }
    
    void OnGangBalanceReceived(int balance)
    {
        m_iCachedGangBalance = balance;
        UpdateGangBankBalance(balance);
        m_bGangBalanceLoaded = true;
    }
    
    void UpdateGangBankBalance(int gangBalance)
    {
        if (m_gangBankBalanceValue)
            m_gangBankBalanceValue.SetText(RL_Utils.FormatMoney(gangBalance));
    }
    
    override void OnTransaction(SCR_InputButtonComponent w)
    {
        if(!m_character || m_character.IsSpamming()) return;
        
        if (!m_bGangBalanceLoaded)
        {
            RL_Utils.Notify("Gang balance not loaded yet, please wait.", "GANG ATM");
            return;
        }
        
        int currentCash = m_character.GetCash();
        int currentGangBalance = m_iCachedGangBalance;
        int inputAmount = m_amountInput.GetValue().ToInt();
        int playerGangRank = m_character.GetGangRank();
        int playerGangId = m_character.GetGangId();
        
        if (playerGangId == -1)
        {
            RL_Utils.Notify("You are not in a gang.", "GANG ATM");
            CloseMenu();
            return;
        }
        
        if ((!inputAmount || inputAmount < 1) && (w == m_withdrawButton || w == m_depositButton))
        {
            RL_Utils.Notify("Transaction amount must be greater than $0.", "GANG ATM");
            return;
        }
        
        if (w == m_withdrawButton)
        {
            // Only allow withdrawals for rank 3 and up
            if (playerGangRank < EGangRank.CAPTAIN)
            {
                RL_Utils.Notify("Only Captains and above can withdraw from gang funds.", "GANG ATM");
                return;
            }
            
            if (inputAmount > currentGangBalance)
            {
                RL_Utils.Notify("Insufficient gang funds.", "GANG ATM");
                return;
            }
            
            m_character.GangTransaction(inputAmount, 0, -inputAmount, playerGangId);
            m_iCachedGangBalance -= inputAmount;
            UpdateGangBankBalance(m_iCachedGangBalance);
        }
        else if (w == m_depositButton)
        {
            if (inputAmount > currentCash)
            {
                RL_Utils.Notify("You don't have enough cash to deposit.", "GANG ATM");
                return;
            }
            
            m_character.GangTransaction(-inputAmount, 0, inputAmount, playerGangId);
            m_iCachedGangBalance += inputAmount;
            UpdateGangBankBalance(m_iCachedGangBalance);
        }
        else if (w == m_depositAllButton)
        {
            if (currentCash < 1)
            {
                RL_Utils.Notify("You don't have any cash to deposit.", "GANG ATM");
                return;
            }
            
            m_character.GangTransaction(-currentCash, 0, currentCash, playerGangId);
            m_iCachedGangBalance += currentCash;
            UpdateGangBankBalance(m_iCachedGangBalance);
        }
        else if (w == m_transferButton)
        {
            return;
        }
        
        m_character.PlayCommonSoundLocally("SOUND_ATMBEEP");
    }
} 