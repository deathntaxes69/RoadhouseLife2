class RL_GangInviteWidget : ScriptedWidgetComponent
{
	
	private const string m_sDeclineButton = "Decline";
	private const string m_sAcceptButton = "Accept";
	protected SCR_InputButtonComponent m_DeclineButton;
	protected SCR_InputButtonComponent m_AcceptButton;
	protected RL_GangWidget m_GangWidget;
	
	override protected void HandlerAttached(Widget w)
	{
		
		Widget parentWidget = w;
		while (!m_GangWidget && parentWidget)
		{
			Print("[RL_GangInviteWidget] HandlerAttached while loop");
			parentWidget = parentWidget.GetParent();
			if (parentWidget)
				m_GangWidget = RL_GangWidget.Cast(parentWidget.FindHandler(RL_GangWidget));
		}
		
		if(!m_GangWidget)
		{
			return;
		}
		
		m_AcceptButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sAcceptButton, w);
		m_DeclineButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sDeclineButton, w);
		
		if (m_AcceptButton)
			m_AcceptButton.m_OnActivated.Insert(m_GangWidget.AcceptInvite);
		
		if (m_DeclineButton)
			m_DeclineButton.m_OnActivated.Insert(m_GangWidget.DeclineInvite);
	}
	
}