sealed class RL_EmergencyWidget : RL_AppBaseWidget
{
    protected Widget m_contentLayout;
	protected SCR_EditBoxComponent m_editBox;
	protected SCR_InputButtonComponent m_notifyPoliceButton;
	protected SCR_InputButtonComponent m_notifyEMSButton;
	
    private const string m_emergencyLayout = "{FE5C26F72EBB7FE3}UI/Layouts/Phone/Apps/Emergency/Emergency.layout";
	private const string m_sEditBox = "EditBox";
	private const string m_sNotifyPolice = "NotifyPolice";
	private const string m_sNotifyEMS = "NotifyEMS";

	//------------------------------------------------------------------------------------------------
    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
        if(!m_wRoot || !m_character) 
			return;
        
        m_contentLayout = Widget.Cast(m_wRoot.FindAnyWidget("contentLayout"));
        if(!m_contentLayout)
            return;
		
		Widget editBoxWidget = Widget.Cast(m_wRoot.FindAnyWidget(m_sEditBox));
		if(editBoxWidget)
		{
			m_editBox = SCR_EditBoxComponent.Cast(editBoxWidget.FindHandler(SCR_EditBoxComponent));
			if(m_editBox)
			{
				m_editBox.SetPlaceholderText("Enter your emergency message...");
				m_editBox.ActivateWriteMode(true);
			}
		}
		
		m_notifyPoliceButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sNotifyPolice, w);
        if (m_notifyPoliceButton)
        	m_notifyPoliceButton.m_OnActivated.Insert(OnNotifyPolice);

		m_notifyEMSButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sNotifyEMS, w);
        if (m_notifyEMSButton)
        	m_notifyEMSButton.m_OnActivated.Insert(OnNotifyEMS);

		TextWidget policeCountText = TextWidget.Cast(m_wRoot.FindAnyWidget("policeCountText"));
		TextWidget medicCountText = TextWidget.Cast(m_wRoot.FindAnyWidget("medicCountText"));
		policeCountText.SetText(string.Format("Police Count: %1", RL_Utils.GetPoliceOnlineCount()));
		medicCountText.SetText(string.Format("EMS Count: %1", RL_Utils.GetEMSOnlineCount()));


		

	}
	
	//------------------------------------------------------------------------------------------------
	void OnNotifyPolice(SCR_InputButtonComponent source)
	{
		if(!m_editBox || !m_character)
			return;

		if (m_character.IsSpamming())
			return;

		string message = m_editBox.GetValue();
		
		if(message.IsEmpty() || message.Trim().IsEmpty())
		{
			m_character.Notify("Please enter a message before sending.", "Emergency");
			return;
		}
		
		string formattedMessage = string.Format("EMERGENCY ALERT: %1\nReported by: %2", 
			message, m_character.GetCharacterName());
		
		string markerText = string.Format("EMERGENCY CALL\nFrom: %1\n%2", m_character.GetCharacterName(), message);
		m_character.SendEmergencyToPolice("911 Emergency Call", formattedMessage, markerText);
		
		m_editBox.SetValue("");
		m_character.Notify("Emergency alert sent to police.", "Emergency");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnNotifyEMS(SCR_InputButtonComponent source)
	{
		if(!m_editBox || !m_character)
		{
			return;
		}

		if (m_character.IsSpamming())
			return;

		string message = m_editBox.GetValue();
		
		if(message.IsEmpty() || message.Trim().IsEmpty())
		{
			m_character.Notify("Please enter a message before sending.", "Emergency");
			return;
		}
		
		string formattedMessage = string.Format("MEDICAL EMERGENCY: %1\nReported by: %2", 
			message, m_character.GetCharacterName());
		
		string markerText = string.Format("MEDICAL EMERGENCY\nFrom: %1", m_character.GetCharacterName());
		m_character.SendEmergencyToEMS("Medical Emergency Call", formattedMessage, markerText);
		
		m_editBox.SetValue("");
		m_character.Notify("Medical emergency alert sent to EMS.", "Emergency");
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
		
		if(m_notifyPoliceButton)
			m_notifyPoliceButton.m_OnActivated.Remove(OnNotifyPolice);
			
		if(m_notifyEMSButton)
			m_notifyEMSButton.m_OnActivated.Remove(OnNotifyEMS);
	}
}