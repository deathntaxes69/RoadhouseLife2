class RL_GangMemberWidget : ScriptedWidgetComponent
{
	
	private const string m_sMemberOptionsCombo = "ComboRoot0";
	protected SCR_ComboBoxComponent m_memberOptionsCombo;
	protected RL_GangWidget m_GangWidget;
	
	override protected void HandlerAttached(Widget w)
	{
		
		Widget parentWidget = w;
		while (!m_GangWidget && parentWidget)
		{
			Print("[RL_GangMemberWidget] HandlerAttached while loop");
			parentWidget = parentWidget.GetParent();
			if (parentWidget)
				m_GangWidget = RL_GangWidget.Cast(parentWidget.FindHandler(RL_GangWidget));
		}
		
		if(!m_GangWidget)
		{
			return;
		}
		
		
        Widget widgetOptionsCombo = Widget.Cast(w.FindAnyWidget(m_sMemberOptionsCombo));
		
		if(!widgetOptionsCombo)
		{
        	return;
		}
		
		m_memberOptionsCombo = SCR_ComboBoxComponent.Cast(widgetOptionsCombo.FindHandler(SCR_ComboBoxComponent));
		
		if (!m_memberOptionsCombo)
		{
			return;
		}
		
		m_memberOptionsCombo.m_OnChanged.Insert(m_GangWidget.MemberAction);
	}
	
}