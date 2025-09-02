[EntityEditorProps(category: "GameScripted/Crafting", description: "")]
class RL_CraftingStationComponentClass : ScriptComponentClass
{
}

class RL_CraftingStationComponent : ScriptComponent
{
	[Attribute("", desc: "Crafting Table Type")]
	protected string m_sCraftingTableType;
	
	[Attribute("", UIWidgets.Auto, "Available recipe categories for this crafting station. If empty, all categories will be available.", params: "conf class=RL_CraftingCategoryList")]
	protected ref array<string> m_aAvailableCategories;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!m_aAvailableCategories)
			m_aAvailableCategories = new array<string>();
	}
	
	//------------------------------------------------------------------------------------------------
	string GetCraftingTableType()
	{
		return m_sCraftingTableType;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCraftingTableType(string craftingTableType)
	{
		m_sCraftingTableType = craftingTableType;
	}
	
	//------------------------------------------------------------------------------------------------
	array<string> GetAvailableCategories()
	{
		return m_aAvailableCategories;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAvailableCategories(array<string> categories)
	{
		m_aAvailableCategories = categories;
	}
	
	//------------------------------------------------------------------------------------------------
	void OpenCraftingMenu()
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		MenuBase menu = menuManager.FindMenuByPreset(ChimeraMenuPreset.RL_craftingMenu);
		if (!menu)
		{
			GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_craftingMenu);
			menu = menuManager.FindMenuByPreset(ChimeraMenuPreset.RL_craftingMenu);
		}
		GetGame().GetCallqueue().CallLater(SetupMenuData, 50, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetupMenuData()
	{
		MenuBase menu = GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.RL_craftingMenu);
		if (!menu) 
			return;

		Widget root = menu.GetRootWidget();
		RL_CraftingLayoutComponent craftingComponent = RL_CraftingLayoutComponent.Cast(root.FindWidget("MenuBase").FindHandler(RL_CraftingLayoutComponent));
			
		if (craftingComponent)
		{
			craftingComponent.SetCraftingTableType(m_sCraftingTableType);
			craftingComponent.SetAvailableCategories(m_aAvailableCategories);
			craftingComponent.SetAvailableRecipes();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasValidSettings()
	{
		return !m_sCraftingTableType.IsEmpty();
	}
}