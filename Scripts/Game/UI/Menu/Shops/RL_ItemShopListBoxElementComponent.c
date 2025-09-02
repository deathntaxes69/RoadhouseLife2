class RL_ItemShopListBoxElementComponent : SCR_ListBoxElementComponent
{
	ref array<string> m_textArray = {};
	
	void SetText(string text, string text1 = "", string text2 = "", string text3 = "")
	{
		m_textArray = {text, text1, text2, text3};

		TextWidget w = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetTextName));
		if (w)
			w.SetText(text);

        TextWidget w1 = TextWidget.Cast(m_wRoot.FindAnyWidget("Text1"));
		if (w1)
			w1.SetText(text1);
        
        TextWidget w2 = TextWidget.Cast(m_wRoot.FindAnyWidget("Text2"));
		if (w2)
			w2.SetText(text2);

        TextWidget w3 = TextWidget.Cast(m_wRoot.FindAnyWidget("Text3"));
		if (w3)
			w3.SetText(text3);
	}
}
