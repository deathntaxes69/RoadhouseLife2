class RL_ConversationListBoxElementComponent : SCR_ListBoxElementComponent
{
	ref array<string> m_textArray = {};
	
	void SetText(string titleText, string text = "")
	{
		m_textArray = {titleText, text};

		TextWidget title = TextWidget.Cast(m_wRoot.FindAnyWidget("TitleText"));
		if (title)
			title.SetText(titleText);

		TextWidget w = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetTextName));
		if (w)
			w.SetText(text);
	}
}
