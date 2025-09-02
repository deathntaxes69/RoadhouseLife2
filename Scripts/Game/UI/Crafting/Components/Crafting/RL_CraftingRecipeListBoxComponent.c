class RL_CraftingRecipeListBoxComponent : RL_CraftingListBoxComponent
{
	private const string SKILL_ICONS_IMAGESET = "{F5A85B3176986520}UI/Imagesets/PhoneIcons/phoneAppIcons.imageset";
	
	// Add a new method to handle RL_CraftingRecipe directly
	int AddItem(RL_CraftListBoxData craftData, Managed data = null, ResourceName itemLayout = string.Empty)
	{
		RL_CraftingListBoxElementComponent comp;
		int id = _AddItem(craftData, data, comp, itemLayout);
		return id;
	}
	
	protected int _AddItem(RL_CraftListBoxData craftData, Managed data, out RL_CraftingListBoxElementComponent compOut, ResourceName itemLayout = string.Empty)
	{	
		// Create widget for this item
		// The layout can be provided either as argument or through attribute
		
		ResourceName selectedLayout = m_sElementLayout;
		if (!itemLayout.IsEmpty())
			selectedLayout = itemLayout;
		Widget newWidget = GetGame().GetWorkspace().CreateWidgets(selectedLayout, m_wList);
		
		RL_CraftingRecipeListBoxElementComponent comp = RL_CraftingRecipeListBoxElementComponent.Cast(newWidget.FindHandler(RL_CraftingRecipeListBoxElementComponent));
		
		if (craftData.recipe)
		{
			comp.SetText(craftData.recipe.name);
			comp.SetCategory(craftData.recipe.category);
			//PrintFormat("setdata %1", craftData.recipe);
			//PrintFormat("setdata name from recipe example %1", craftData.recipe.name);
			comp.SetData(craftData.recipe);
		}
		else
		{
			if (craftData.text)
			{
				comp.SetText(craftData.text);
			}
			if (craftData.preview)
			{
				comp.SetPreview(craftData.preview);
			}
			if (craftData.quantity >= 0)
			{
				comp.SetQuantity(craftData.quantity.ToString());
			}
			if (craftData.icon)
			{
				if (craftData.skillCategory != 0)
				{
					string iconName = RL_SkillsUtils.GetSkillCategoryIcon(craftData.skillCategory);
					comp.SetIconFromImageset(SKILL_ICONS_IMAGESET, iconName);
				}
				else
				{
					comp.SetIcon(craftData.icon);
				}
			}
		}
		
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
		
		compOut = comp;
		
		return id;
	}
	
	// Override the OnItemClick to ensure we select the recipe and notify listeners
	override protected void OnItemClick(RL_CraftingListBoxElementComponent comp)
	{
		int id = m_aElementComponents.Find(comp);
		if (id == -1)
			return;
		
		// Always select the item when clicked in this implementation
		// Unselect previous item if different
		if (id != m_iCurrentItem && m_iCurrentItem >= 0 && m_iCurrentItem < m_aElementComponents.Count())
			VisualizeSelection(m_iCurrentItem, false);
		
		// Select new item
		_SetItemSelected(id, true, true);
	}
	
}