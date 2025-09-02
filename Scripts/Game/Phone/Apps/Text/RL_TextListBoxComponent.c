class RL_TextListBoxComponent : SCR_ListBoxComponent
{
	private ref Color m_sentTextColor = new Color(0.047, 0.518, 0.996, 1.0);
    private ref Color m_recievedTextColor = new Color(0.149, 0.145, 0.165, 1.0);

	array<SCR_ListBoxElementComponent> GetComps()
	{
		return m_aElementComponents;
	}
	int CreateTextEntry(string text, HorizontalFillOrigin fillOrgin)
	{
		ResourceName selectedLayout = m_sElementLayout;
		
		Widget newWidget = GetGame().GetWorkspace().CreateWidgets(selectedLayout, m_wList);
		HorizontalLayoutWidget layoutWidget = HorizontalLayoutWidget.Cast(newWidget.FindAnyWidget("HorizontalLayout"));
        if(layoutWidget)
			layoutWidget.SetFillOrigin(fillOrgin);
		ImageWidget backgroundImage = ImageWidget.Cast(newWidget.FindAnyWidget("Image0"));
		if(fillOrgin == HorizontalFillOrigin.LEFT)
			backgroundImage.SetColor(m_sentTextColor);
		else
			backgroundImage.SetColor(m_recievedTextColor);

		SCR_ListBoxElementComponent comp = SCR_ListBoxElementComponent.Cast(newWidget.FindHandler(SCR_ListBoxElementComponent));
		
		comp.SetText(text);
		//comp.SetToggleable(true);
		comp.SetData(null);

		// Pushback to internal arrays
		int id = m_aElementComponents.Insert(comp);
		
		// Set up explicit navigation rules for elements. Otherwise we can't navigate
		// Through separators when we are at the edge of scrolling if there is an element
		// directly above/below the list box which intercepts focus
		string widgetName = this.GetUniqueWidgetName();
		newWidget.SetName(widgetName);
		if (m_aElementComponents.Count() > 1)
		{
			Widget prevWidget = m_aElementComponents[m_aElementComponents.Count() - 2].GetRootWidget();
			prevWidget.SetNavigation(WidgetNavigationDirection.DOWN, WidgetNavigationRuleType.EXPLICIT, newWidget.GetName());
			newWidget.SetNavigation(WidgetNavigationDirection.UP, WidgetNavigationRuleType.EXPLICIT, prevWidget.GetName());
		}
		
		return id;
	}
	int AddConversation(string title, string text)
	{	
		// Create widget for this item
		// The layout can be provided either as argument or through attribute
		ResourceName selectedLayout = m_sElementLayout;
		
		Widget newWidget = GetGame().GetWorkspace().CreateWidgets(selectedLayout, m_wList);
		
		RL_ConversationListBoxElementComponent comp = RL_ConversationListBoxElementComponent.Cast(newWidget.FindHandler(RL_ConversationListBoxElementComponent));
		
		//comp.SetImage(image);
		comp.SetText(title, text);
		comp.SetToggleable(true);
		comp.SetData(null);
		
		// Pushback to internal arrays
		int id = m_aElementComponents.Insert(comp);

		// Setup event handlers
		comp.m_OnClicked.Insert(OnItemClick);
		
		// Set up explicit navigation rules for elements. Otherwise we can't navigate
		// Through separators when we are at the edge of scrolling if there is an element
		// directly above/below the list box which intercepts focus
		string widgetName = this.GetUniqueWidgetName();
		newWidget.SetName(widgetName);
		if (m_aElementComponents.Count() > 1)
		{
			Widget prevWidget = m_aElementComponents[m_aElementComponents.Count() - 2].GetRootWidget();
			prevWidget.SetNavigation(WidgetNavigationDirection.DOWN, WidgetNavigationRuleType.EXPLICIT, newWidget.GetName());
			newWidget.SetNavigation(WidgetNavigationDirection.UP, WidgetNavigationRuleType.EXPLICIT, prevWidget.GetName());
		}
		
		return id;
	}
	

}
