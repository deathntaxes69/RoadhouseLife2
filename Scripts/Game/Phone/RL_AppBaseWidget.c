class RL_AppBaseWidget : ScriptedWidgetComponent
{

    protected Widget m_wRoot;
    protected RL_PhoneUI m_phoneMenu;
    protected SCR_InputButtonComponent m_phoneBackButton;


    protected SCR_ChimeraCharacter m_character;

    override protected void HandlerAttached(Widget w)
	{
		m_wRoot = w;
        m_character = RL_Utils.GetLocalCharacter();
        
		
	}
    void SetPhoneMenu(RL_PhoneUI phoneMenu)
	{	
		m_phoneMenu = phoneMenu;
        m_phoneBackButton = m_phoneMenu.GetBackButton();
        if (m_phoneBackButton)
            m_phoneBackButton.m_OnActivated.Insert(OnBack);
	}
    void OnBack()
    {
        if (m_phoneBackButton)
            m_phoneBackButton.m_OnActivated.Remove(OnBack);
        
        if(m_phoneMenu)
            m_phoneMenu.BackToHome();

    }
    override void HandlerDeattached(Widget w)
	{
        if (m_phoneBackButton)
            m_phoneBackButton.m_OnActivated.Remove(OnBack);
	}
}