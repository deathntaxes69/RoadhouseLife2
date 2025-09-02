class RL_ItemShopListBoxComponent : SCR_ListBoxComponent
{
	array<SCR_ListBoxElementComponent> GetComps()
	{
		return m_aElementComponents;
	}

	int AddMultiColumnItem(string text0, string text1 = "", string text2 = "", string text3 = "")
	{	
		// Create widget for this item
		// The layout can be provided either as argument or through attribute
		ResourceName selectedLayout = m_sElementLayout;
		
		Widget newWidget = GetGame().GetWorkspace().CreateWidgets(selectedLayout, m_wList);
		
		RL_ItemShopListBoxElementComponent comp = RL_ItemShopListBoxElementComponent.Cast(newWidget.FindHandler(RL_ItemShopListBoxElementComponent));
		
		comp.SetText(text0, text1, text2, text3);
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
