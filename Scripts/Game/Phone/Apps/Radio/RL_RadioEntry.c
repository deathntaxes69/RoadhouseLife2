class RL_RadioEntry : ScriptedWidgetComponent
{
    protected Widget m_wRoot;
    protected SCR_ChimeraCharacter m_character;
    protected SCR_EditBoxComponent m_channelEditBox;

    protected int m_iChannel;
    protected BaseTransceiver m_tsv;

    override protected void HandlerAttached(Widget w)
    {
        m_wRoot = w;
        m_character = RL_Utils.GetLocalCharacter();
        
        m_channelEditBox = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("EditBoxRoot0").FindHandler(SCR_EditBoxComponent));
        if(!m_channelEditBox)
            return; 

    }
    
    void SetChannel(int channel)
    {
        m_iChannel = channel;
        m_channelEditBox.SetLabel(string.Format("Channel %1", channel+1));
    }
    
    void SetTransceiver(BaseTransceiver tsv)
    {
        m_tsv = tsv;
        float currentFreq = RL_RadioUtils.HzToKhz(m_tsv.GetFrequency());
        string freqString = currentFreq.ToString();
        if(m_channelEditBox)
            m_channelEditBox.SetValue(freqString);
    }
    
    void HandleFreqInputChange() 
    {
        Print("HandleFreqInputChange");
        if (!m_tsv || !m_channelEditBox)
            return;
            
        string newFreq = m_channelEditBox.GetValue();
        float khzFloat = newFreq.ToFloat();
        float hzFloat = RL_RadioUtils.KhzToHz(khzFloat);

        Print(hzFloat);
        
        // Clamp frequency to valid range
        if (hzFloat > m_tsv.GetMaxFrequency())
        {
            hzFloat = m_tsv.GetMaxFrequency();
            m_channelEditBox.SetValue(RL_RadioUtils.HzToKhz(hzFloat).ToString());
        }
        else if (hzFloat < m_tsv.GetMinFrequency())
        {
            hzFloat = m_tsv.GetMinFrequency();
            m_channelEditBox.SetValue(RL_RadioUtils.HzToKhz(hzFloat).ToString());
        }
        if(hzFloat == m_tsv.GetFrequency()) return;
        // Set the frequency on the character
        if (m_character)
        {
            Print("AskSetFrequency");
            m_character.AskSetFrequency(m_iChannel, hzFloat);
        }
    }
    
    override void HandlerDeattached(Widget w)
    {
        if (m_channelEditBox)
        {
            m_channelEditBox.m_OnChanged.Remove(HandleFreqInputChange);
        }
    
    }
}