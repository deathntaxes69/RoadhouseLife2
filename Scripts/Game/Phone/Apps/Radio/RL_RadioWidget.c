sealed class RL_RadioWidget : RL_AppBaseWidget
{
    protected Widget m_contentLayout;
	protected SCR_InputButtonComponent m_setButton;
    protected string m_sRadioEntryLayout = "{FB3CE4A76B25CE1B}UI/Layouts/Phone/Apps/Radio/RadioEntry.layout";

    protected ref array<BaseTransceiver> m_aTransceivers;
    protected ref array<RL_RadioEntry> m_aRadioEntries = {};

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
        if(!m_wRoot || !m_character) return;
        
        m_contentLayout = Widget.Cast(m_wRoot.FindAnyWidget("VerticalLayout0"));
        if(!m_contentLayout)
            return;

        m_setButton = SCR_InputButtonComponent.GetInputButtonComponent("SetButton", m_wRoot);
        if (m_setButton)
            m_setButton.m_OnActivated.Insert(OnSet);

        array<BaseTransceiver> m_aTransceivers;
        RL_RadioUtils.GetAllTransceivers(m_character, m_aTransceivers);
        Print(m_aTransceivers);
        int i = 0;
        foreach (BaseTransceiver tsv : m_aTransceivers)
		{
		    Widget appWidget = GetGame().GetWorkspace().CreateWidgets(m_sRadioEntryLayout, m_contentLayout);
            RL_RadioEntry radioEntry = RL_RadioEntry.Cast(appWidget.FindHandler(RL_RadioEntry));
            radioEntry.SetChannel(i);
            radioEntry.SetTransceiver(tsv);
            m_aRadioEntries.Insert(radioEntry);
            i++;

        }
	}
    void OnSet()
    {
        if(!m_character || m_character.IsSpamming()) return;
        foreach (RL_RadioEntry radioEntry : m_aRadioEntries)
		{
            radioEntry.HandleFreqInputChange();
        }
    }
    
}