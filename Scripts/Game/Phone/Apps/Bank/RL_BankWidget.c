sealed class RL_BankWidget : RL_AppBaseWidget
{
    protected Widget m_contentLayout;
	
    protected TextWidget m_bankValueText;
	protected TextWidget m_gangTitleText;
    protected TextWidget m_gangValueText;

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
        if(!m_wRoot || !m_character) return;
        
        m_bankValueText = TextWidget.Cast(m_wRoot.FindAnyWidget("bankBalanceValue"));
        if(!m_bankValueText)
            return;

		m_gangTitleText = TextWidget.Cast(m_wRoot.FindAnyWidget("gangTitle"));
        if(!m_gangTitleText)
            return;

        m_gangValueText = TextWidget.Cast(m_wRoot.FindAnyWidget("gangBalanceValue"));
        if(!m_gangValueText)
            return;
    

       	if (m_character.GetGangId() != -1)
	   		m_character.GetGangBalance(this);
       
	  	m_bankValueText.SetText(RL_Utils.FormatMoney(m_character.GetBank()));
		
	}
    
    void OnGangBalanceReceived(int balance)
    {
       Print("OnGangBalanceReceived");
        m_gangValueText.SetText(RL_Utils.FormatMoney(balance));
		m_gangTitleText.SetVisible(true);
		m_gangValueText.SetVisible(true);


    }
    
}