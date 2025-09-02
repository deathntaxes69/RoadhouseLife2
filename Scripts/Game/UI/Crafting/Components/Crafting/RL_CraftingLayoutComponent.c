class RL_CraftingLayoutComponent : ScriptedWidgetComponent
{
	
	[Attribute("previewArea")]
	protected string m_sPreviewAreaName;
	
	[Attribute("ItemPreview")]
	protected string m_sItemPreviewName;
	
	[Attribute("itemName")]
	protected string m_sItemNameText;
	
	[Attribute("itemXpGain")]
	protected string m_sItemXpGainText;
	
	[Attribute("itemCraftingTime")]
	protected string m_sItemCraftingTimeText;
	
	[Attribute("ProgressBar")]
	protected string m_sProgressBarName;
	
	[Attribute("requiredIngredientsList")]
	protected string m_sRequiredIngredientsList;
	
	[Attribute("requiredIngredientsLabel")]
	protected string m_sRequiredIngredientsListLabel;
	
	[Attribute("requiredSkillsList")]
	protected string m_sRequiredSkillsList;
	
	[Attribute("requiredSkillsLabel")]
	protected string m_sRequiredSkillsListLabel;
	
	[Attribute("recipeList")]
	protected string m_sRecipeListName;
	
	[Attribute("requiredIngredientsList")]
	protected string m_sIngredientsListName;
	
	[Attribute("requiredSkillsList")]
	protected string m_sSkillsListName;
	
	[Attribute("quantity")]
	protected string m_sQuantity;
	
	[Attribute("CategoryCombo")]
	protected string m_sCategoryCombo;
	
	[Attribute("Title")]
	protected string m_sMainWindowTitle;
	
	[Attribute("CraftButton")]
	protected string m_sCraftButton;
	
	
	[Attribute("", desc: "Recipe Config List")]
	protected ResourceName RL_CraftingRecipeListPath;

	private const string SKILL_ICONS_IMAGESET = "{F5A85B3176986520}UI/Imagesets/PhoneIcons/phoneAppIcons.imageset";

	protected SCR_ChimeraCharacter m_character;
	
	protected Widget m_wPreviewArea;
	protected ItemPreviewWidget m_wItemPreview;
	protected TextWidget m_wItemName;
	protected TextWidget m_wItemXpGain;
	protected TextWidget m_wItemCraftingTime;
	protected Widget m_wProgressBar;
	protected SCR_WLibProgressBarComponent m_ProgressBarComponent;
	protected OverlayWidget m_wRequiredIngredientsList;
	protected OverlayWidget m_wRequiredSkillsList;
	protected Widget m_wRequiredSkillsListLabel;
	protected Widget m_wRequiredIngredientsListLabel;
	protected Widget m_wCategoryCombo;
	protected Widget m_wQuantity;
	protected SCR_EditBoxComponent m_EditBox;
	protected SCR_ComboBoxComponent m_Combo;
	protected SCR_InputButtonComponent m_ButtonComponent;
	protected TextWidget m_wMainWindowTitle;
	protected array<ref string> m_Recipes;
	
	protected RL_CraftingRecipeListBoxComponent m_RecipeListBox;
	protected Widget m_wRecipeList;
	
	protected RL_CraftingRecipeListBoxComponent m_IngredientsListBox;
	protected Widget m_wIngredientsList;
	
	protected RL_CraftingRecipeListBoxComponent m_SkillsListBox;
	protected Widget m_wSkillsList;

	static ItemPreviewManagerEntity m_previewManager;
	
	// Available categories
	protected ref array<string> m_AvailableCategories = new array<string>();
	
	// Tracking variables
	protected int m_iCurrentQuantity = 1;
	protected RL_CraftingRecipe m_CurrentRecipe;
	protected ref array<ref RL_CraftingIngredient> m_CurrentIngredients = new array<ref RL_CraftingIngredient>();
	protected int m_iCurrentXPGain = 0;
	protected string m_CraftingTableType = "";
	protected ResourceName m_CurrentOutput;
	
	protected ref array<string> m_TempPrefabs = new array<string>();
	protected ref array<int> m_TempQuantities = new array<int>();
	
	// Crafting state variables
	protected bool m_bIsCrafting = false;
	protected int m_iCraftingItemsRemaining = 0;
	protected int m_iCurrentCraftingTime = 0;
	protected int m_iCraftingQuantity = 0;

	protected RL_CraftingRecipe m_CraftingSessionRecipe;
	protected ResourceName m_CraftingSessionOutput;
	
	// track if all remove item callbacks were succesful (there is probably a better way to do this)
	protected int m_RemovedItemsTotal;

	// Skills tracking
	protected ref array<ref RL_SkillData> m_PlayerSkills = new array<ref RL_SkillData>();
	protected bool m_bSkillsLoaded = false;

	
	//------------------------------------------------------------------------------------------------
	static ItemPreviewManagerEntity GetPreviewManager() 
	{
		if (!m_previewManager)
			m_previewManager = ChimeraWorld.CastFrom(GetGame().GetWorld()).GetItemPreviewManager();
		
		return m_previewManager;
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		m_character = RL_Utils.GetLocalCharacter();
		
		// Load player skills
		LoadPlayerSkills();
		
		// Find widgets
		m_wPreviewArea = w.FindAnyWidget(m_sPreviewAreaName);
		m_wItemPreview = ItemPreviewWidget.Cast(w.FindAnyWidget(m_sItemPreviewName));
		m_wItemName = TextWidget.Cast(w.FindAnyWidget(m_sItemNameText));
		m_wItemXpGain = TextWidget.Cast(w.FindAnyWidget(m_sItemXpGainText));
		m_wItemCraftingTime = TextWidget.Cast(w.FindAnyWidget(m_sItemCraftingTimeText));
		m_wProgressBar = w.FindAnyWidget(m_sProgressBarName);
		m_wRequiredIngredientsList = OverlayWidget.Cast(w.FindAnyWidget(m_sRequiredIngredientsList));
		m_wRequiredSkillsList = OverlayWidget.Cast(w.FindAnyWidget(m_sRequiredSkillsList));
		m_wRecipeList = w.FindAnyWidget(m_sRecipeListName);
		m_wSkillsList = w.FindAnyWidget( m_sSkillsListName);
		m_wIngredientsList = w.FindAnyWidget(m_sIngredientsListName);
		m_wRequiredIngredientsListLabel = w.FindAnyWidget(m_sRequiredIngredientsListLabel);
		m_wRequiredSkillsListLabel = w.FindAnyWidget(m_sRequiredSkillsListLabel);
		m_wQuantity = w.FindAnyWidget(m_sQuantity);
		m_wMainWindowTitle = TextWidget.Cast(w.FindAnyWidget(m_sMainWindowTitle));
		m_wCategoryCombo = w.FindAnyWidget(m_sCategoryCombo);
		
		if (m_wProgressBar)
		{
			m_ProgressBarComponent = SCR_WLibProgressBarComponent.Cast(m_wProgressBar.FindHandler(SCR_WLibProgressBarComponent));
			if (m_ProgressBarComponent)
			{
				m_ProgressBarComponent.SetMin(0);
				m_ProgressBarComponent.SetMax(100);
				m_ProgressBarComponent.SetValue(0, false);
				m_ProgressBarComponent.SetAnimationTime(0.1);
			}
		}
		
		if (m_wRecipeList)
		{
			m_RecipeListBox = RL_CraftingRecipeListBoxComponent.Cast(m_wRecipeList.FindHandler(RL_CraftingRecipeListBoxComponent));
			
			if (m_RecipeListBox)
			{
				m_RecipeListBox.m_OnChanged.Insert(OnRecipeSelectionChanged);
			}
		}
		
		//track quantity changes via invoker
		if (m_wQuantity)
		{
			m_EditBox = SCR_EditBoxComponent.Cast(m_wQuantity.FindHandler(SCR_EditBoxComponent));
			if (m_EditBox)
			{
				m_EditBox.m_OnConfirm.Insert(OnQuantityChange);
			}
		}
		

		//track category changes via invoker
		if (m_wCategoryCombo)
		{
			m_Combo = SCR_ComboBoxComponent.Cast(m_wCategoryCombo.FindHandler(SCR_ComboBoxComponent));
			if (m_Combo)
			{
				m_Combo.m_OnChanged.Insert(UpdateRecipes);
			}
		}
		
		m_ButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent(m_sCraftButton, w);
        if (m_ButtonComponent)
        	m_ButtonComponent.m_OnActivated.Insert(Craft);
		
		// Initialize quantity to 1
		m_iCurrentQuantity = 1;
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadPlayerSkills()
	{
		if (!m_character)
			return;
			
		m_character.GetAllSkills(this, "OnSkillsLoaded");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSkillsLoaded(bool success, array<ref RL_SkillData> skills)
	{
		m_bSkillsLoaded = success;
		if (success && skills)
		{
			m_PlayerSkills = skills;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	int GetPlayerSkillLevel(RL_SkillsCategory category)
	{
		if (!m_bSkillsLoaded || !m_PlayerSkills)
			return 0;
			
		foreach (RL_SkillData skill : m_PlayerSkills)
		{
			Print("[RL_CraftingLayoutComp] GetPlayerSkillLevel foreach");
			if (skill.GetCategory() == category)
			{
				return RL_SkillsUtils.GetLevelFromXP(skill.GetAmount());
			}
		}
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CheckSkillRequirements(RL_CraftingRecipe recipe)
	{
		if (!recipe.skills || recipe.skills.IsEmpty())
			return true;
			
		foreach (RL_CraftingSkillRequirement skillReq : recipe.skills)
		{
			Print("[RL_CraftingLayoutComp] CheckSkillRequirements foreach");
			int playerLevel = GetPlayerSkillLevel(skillReq.skillCategory);
			if (playerLevel < skillReq.skillLevel)
			{
				string skillName = RL_SkillsUtils.CategoryIdToName(skillReq.skillCategory);
				RL_Utils.Notify(string.Format("Insufficient %1 level. Required: %2, Current: %3", skillName, skillReq.skillLevel, playerLevel), "CRAFTING");
				return false;
			}
		}
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CheckRecipeRequirements(RL_CraftingRecipe recipe)
	{
		if (!m_character)
			return false;
			
		if (recipe.recipeId == -1)
			return true;
			
		if (!m_character.HasRecipe(recipe.recipeId.ToString()))
		{
			RL_Utils.Notify(string.Format("You do not know the recipe: %1", recipe.recipeName), "CRAFTING");
			return false;
		}
		
		return true;
	}
	
	void Craft()
	{
		if (!m_character || m_character.IsSpamming() || m_bIsCrafting || !m_CurrentRecipe)
		{
			return;
		}
		
		if (!CheckRecipeRequirements(m_CurrentRecipe))
		{
			return;
		}
		
		if (!CheckSkillRequirements(m_CurrentRecipe))
		{
			return;
		}
		
		m_CraftingSessionRecipe = m_CurrentRecipe;
		m_CraftingSessionOutput = m_CurrentOutput;
		m_bIsCrafting = true;
		m_iCraftingItemsRemaining = m_iCurrentQuantity;
		m_iCraftingQuantity = m_iCurrentQuantity;
		m_iCurrentCraftingTime = GetModifiedCraftingTime(m_CurrentRecipe.craftingTime) * m_iCraftingQuantity;
		
		// Disable inputs during crafting
		m_ButtonComponent.SetEnabled(false);
		m_wRecipeList.SetEnabled(false);
		m_Combo.SetEnabled(false);
		m_wQuantity.SetEnabled(false);
		
		StartCraft();
	}
	
	void StartCraft()
	{
		if (!m_character || !m_CraftingSessionRecipe || m_iCraftingQuantity <= 0)
		{
			EndCrafting();
			return;
		}
		
		if (!HasAllRequiredItems())
		{
			RL_Utils.Notify("You do not have the required items", "CRAFTING");
			EndCrafting();
			return;
		}
		
		StartCraftingProgressBar();
	}
	
	void StartCraftingProgressBar()
	{
		if (!m_ProgressBarComponent)
		{
			CompleteCraft();
			return;
		}
		
		m_ProgressBarComponent.SetValue(0, false);
		
		float progressPerSecond = 100.0 / m_iCurrentCraftingTime;
		float updateInterval = 0.1; // Update every 100ms
		float progressIncrement = progressPerSecond * updateInterval;
		
		GetGame().GetCallqueue().CallLater(UpdateCraftingProgress, updateInterval * 1000, true, progressIncrement);
		GetGame().GetCallqueue().CallLater(CompleteCraft, m_iCurrentCraftingTime * 1000);
	}
	
	void UpdateCraftingProgress(float increment)
	{
		if (!m_ProgressBarComponent || !m_bIsCrafting)
		{
			GetGame().GetCallqueue().Remove(UpdateCraftingProgress);
			return;
		}
		
		float currentValue = m_ProgressBarComponent.GetValue();
		float newValue = currentValue + increment;
		
		if (newValue >= 100)
		{
			m_ProgressBarComponent.SetValue(100, true);
			GetGame().GetCallqueue().Remove(UpdateCraftingProgress);
		}
		else
		{
			m_ProgressBarComponent.SetValue(newValue, true);
		}
	}
	
	void CompleteCraft()
	{
		GetGame().GetCallqueue().Remove(UpdateCraftingProgress);
		GetGame().GetCallqueue().Remove(CompleteCraft);
		
		if (!m_character)
		{
			EndCrafting();
			return;
		}
		
		array<string> removePrefabs = {};
		array<int> removeQuantities = {};
		
		foreach (RL_CraftingIngredient ingredient : m_CraftingSessionRecipe.ingredients)
		{
			removePrefabs.Insert(ingredient.GetPrefab());
			removeQuantities.Insert(ingredient.GetQuantity() * m_iCraftingQuantity);
		}
		m_character.RemoveItems(removePrefabs, removeQuantities, this, "OnCraftingItemsRemoved", true);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnCraftingItemsRemoved(bool success)
	{
		if (!m_character)
		{
			EndCrafting();
			return;
		}
		
		if (!success)
		{
			RL_Utils.Notify("Could not remove required ingredients from inventory", "CRAFTING");
			EndCrafting();
			return;
		}
		
		if (m_CraftingSessionOutput)
		{
			int totalOutputQuantity = m_CraftingSessionRecipe.outputQuantity * m_iCraftingQuantity;
			m_character.AddAmount(m_CraftingSessionOutput, totalOutputQuantity, this, "AddAmountCallback", true);
		}
		else
		{
			EndCrafting();
		}
	}
	
	void AddAmountCallback(int amountAdded)
	{
		if (!m_character)
		{
			EndCrafting();
			return;
		}
		
		int totalXpGain = m_CraftingSessionRecipe.xpGain * m_iCraftingQuantity;
		m_character.AddXp((int)RL_SkillsCategory.CRAFTING, totalXpGain);
		
		string quantityText = "";
		if (m_iCraftingQuantity > 1)
			quantityText = string.Format(" (x%1)", m_iCraftingQuantity);
		
		RL_Utils.Notify(string.Format("Items Crafted%1", quantityText), "CRAFTING");
		
		EndCrafting();
	}
	
	void CraftingLog()
	{
		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("characterId", m_character.GetCharacterId());
		jsonData.WriteValue("characterName", m_character.GetCharacterName());
		jsonData.WriteValue("craftingTableType", m_CraftingTableType);
		jsonData.WriteValue("location", m_character.GetOrigin().ToString());
		jsonData.WriteValue("outputItem", m_CraftingSessionOutput.GetPath());
		jsonData.WriteValue("outputQuantity", m_CraftingSessionRecipe.outputQuantity * m_iCraftingQuantity);
		m_character.PlayerLog("Crafting Log", jsonData.ExportToString());
	}
	
	
	void EndCrafting()
	{
		if (m_CraftingSessionRecipe && m_CraftingSessionOutput)
		{
			CraftingLog();
		}
		
		// clean up timers
		GetGame().GetCallqueue().Remove(UpdateCraftingProgress);
		GetGame().GetCallqueue().Remove(CompleteCraft);
		
		m_bIsCrafting = false;
		m_iCraftingItemsRemaining = 0;
		m_CraftingSessionRecipe = null;
		m_CraftingSessionOutput = "";
		
		if (m_ProgressBarComponent)
		{
			m_ProgressBarComponent.SetValue(0, false);
		}
		if(m_ButtonComponent)
			m_ButtonComponent.SetEnabled(true);
		if(m_wRecipeList)
			m_wRecipeList.SetEnabled(true);
		if(m_Combo)
			m_Combo.SetEnabled(true);
		if(m_wQuantity)
			m_wQuantity.SetEnabled(true);
		
		// reload skills after crafting
		LoadPlayerSkills();
	}
	
	void UpdateRecipes()
	{
		string category = m_Combo.GetCurrentItem();
		SetAvailableRecipes(category);
	}
	
	void SetAvailableRecipes(string category_filter = "")
	{
		
		RL_CraftingRecipeList result = SCR_ConfigHelperT<RL_CraftingRecipeList>.GetConfigObject(RL_CraftingRecipeListPath);
		if (!result)
			return;
		
		if (!m_wRecipeList)
			return;
		
		RL_CraftingRecipeListBoxComponent listBox = RL_CraftingRecipeListBoxComponent.Cast(m_wRecipeList.FindHandler(RL_CraftingRecipeListBoxComponent));
		if (!listBox)
			return;
		
		listBox.Clear();
		
		foreach (RL_CraftingRecipe recipe : result.Recipes)
		{
			RL_CraftListBoxData data = new RL_CraftListBoxData;
			data.recipe = recipe;
			if (recipe.craftingStations.Contains(m_CraftingTableType))
			{
				if (category_filter == "")
				{
					AddToList(m_wRecipeList, data);
				}
				else
				{
					if (recipe.category == category_filter)
						AddToList(m_wRecipeList, data);
				}
			}
		}
	}
	
	void SetAvailableCategories(array<string> categories)
	{
		m_AvailableCategories = categories;
		foreach (string category : categories)
		{
			Print("[RL_CraftingLayoutComp] SetAvailableCategories foreach");
			m_Combo.AddItem(category, false);
		}
	}
	
	void SetCraftingTableType(string name)
	{
		m_CraftingTableType = name;
		
		if (m_wMainWindowTitle)
		{
			m_wMainWindowTitle.SetText(name);
		}
	}
	
	protected void AddToList(Widget list, RL_CraftListBoxData data)
	{
		if (!list)
			return;
			
		RL_CraftingRecipeListBoxComponent listBox = RL_CraftingRecipeListBoxComponent.Cast(list.FindHandler(RL_CraftingRecipeListBoxComponent));
		if (!listBox)
			return;
			
		listBox.AddItem(data, data);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnQuantityChange(SCR_EditBoxComponent source, string value)
	{
		if (!value || value == "")
		{
			value = "1"; // Default to "1"
		}
		
		int newQuantity = value.ToInt();
		if (newQuantity < 1) 
			newQuantity = 1;
		
		m_iCurrentQuantity = newQuantity;
		
		if (m_CurrentRecipe)
		{
			UpdatePreview(m_CurrentRecipe);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnRecipeSelectionChanged(RL_CraftingListBoxComponent comp, int item, bool selected)
	{
		if (!selected)
			return;
		
		if (m_bIsCrafting)
			return;
		
		// Get the recipe data
		RL_CraftingRecipe recipe = RL_CraftingRecipe.Cast(comp.GetItemData(item));
		
		if (!recipe)
			return;

		m_CurrentRecipe = recipe;
		
		UpdatePreview(recipe);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdatePreview(RL_CraftingRecipe recipe)
	{
		// Update item preview
		if (m_wItemPreview)
		{
			ItemPreviewManagerEntity previewManager = GetPreviewManager();
			if (previewManager && !recipe.outputItem.IsEmpty())
			{
				previewManager.SetPreviewItemFromPrefab(m_wItemPreview, recipe.outputItem);
			}
		}
		
		// Update text fields
		if (m_wItemName)
		{
			m_wItemName.SetText(recipe.name);
		}
		
		// Calculate XP gain with quantity
		m_iCurrentXPGain = recipe.xpGain * m_iCurrentQuantity;
		
		if (m_wItemXpGain)
		{
			m_wItemXpGain.SetText(string.Format("+%1xp", m_iCurrentXPGain));
		}
		
		// Update crafting time display
		if (m_wItemCraftingTime)
		{
			int modifiedCraftingTime = GetModifiedCraftingTime(recipe.craftingTime);
			int totalCraftingTime = modifiedCraftingTime * m_iCurrentQuantity;
			if (totalCraftingTime > 0)
			{
				string timeText = string.Format("Crafting Time: %1s", totalCraftingTime);
				m_wItemCraftingTime.SetText(timeText);
			}
			else
			{
				m_wItemCraftingTime.SetText("");
			}
		}
		
		m_CurrentOutput = recipe.outputItem;
		
		// Update ingredients and skills lists
		UpdateIngredientsList(recipe);
		UpdateSkillsList(recipe);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateIngredientsList(RL_CraftingRecipe recipe)
	{
		if (!m_wRequiredIngredientsList)
			return;
		
		RL_CraftingRecipeListBoxComponent listBox = RL_CraftingRecipeListBoxComponent.Cast(m_wRequiredIngredientsList.FindHandler(RL_CraftingRecipeListBoxComponent));
		if (!listBox)
			return;
		
		listBox.Clear();
		
		// Clear tracked ingredients list
		m_CurrentIngredients.Clear();
		
		// No ingredients to display
		if (!recipe.ingredients || recipe.ingredients.IsEmpty())
		{
			if (m_wRequiredIngredientsListLabel)
			{
				m_wRequiredIngredientsListLabel.SetVisible(false);
			}
			return;
		}
		else
		{
			m_wRequiredIngredientsListLabel.SetVisible(true);
		}
		
		// Add ingredients with multiplied quantities
		foreach (RL_CraftingIngredient ingredient : recipe.ingredients)
		{
			Print("[RL_CraftingLayoutComp] UpdateIngredientsList foreach");
			// Create a copy of the ingredient with quantity adjusted by current selected quantity
			int totalQuantity = ingredient.GetQuantity() * m_iCurrentQuantity;
			RL_CraftingIngredient adjustedIngredient = RL_CraftingIngredient.Create(ingredient.GetName(), ingredient.GetPrefab(), totalQuantity);

			// Add to tracking list
			m_CurrentIngredients.Insert(adjustedIngredient);
			
			// Add to UI
			AddIngredientItem(adjustedIngredient);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSkillsList(RL_CraftingRecipe recipe)
	{
		if (!m_wRequiredSkillsList)
			return;
		
		RL_CraftingRecipeListBoxComponent listBox = RL_CraftingRecipeListBoxComponent.Cast(m_wRequiredSkillsList.FindHandler(RL_CraftingRecipeListBoxComponent));
		if (!listBox)
			return;
		
		listBox.Clear();
		
		bool hasRequirements = false;
		if (recipe.recipeId != -1)
		{
			AddRecipeItem(recipe);
			hasRequirements = true;
		}
			
		// Add skills
		if (recipe.skills && !recipe.skills.IsEmpty())
		{
		foreach (RL_CraftingSkillRequirement skill : recipe.skills)
		{
			Print("[RL_CraftingLayoutComp] UpdateSkillsList foreach");
			AddSkillItem(skill);
			}
			hasRequirements = true;
		}
		
		if (m_wRequiredSkillsListLabel)
		{
			m_wRequiredSkillsListLabel.SetVisible(hasRequirements);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AddIngredientItem(RL_CraftingIngredient ingredient)
	{
		
		if (!m_wRequiredIngredientsList)
			return;
		
		RL_CraftListBoxData data = new RL_CraftListBoxData;
		data.text = ingredient.GetName();
		data.quantity = ingredient.GetQuantity();
		data.preview = ingredient.GetPrefab();
		
		AddToList(m_wRequiredIngredientsList, data);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AddSkillItem(RL_CraftingSkillRequirement skill)
	{
		if (!m_wRequiredSkillsList)
			return;
		
		RL_CraftListBoxData data = new RL_CraftListBoxData;
		data.text = RL_SkillsUtils.CategoryIdToName(skill.skillCategory);
		data.quantity = skill.skillLevel;
		data.skillCategory = skill.skillCategory;
		data.icon = "skill_icon";

		AddToList(m_wRequiredSkillsList, data);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AddRecipeItem(RL_CraftingRecipe recipe)
	{
		if (!m_wRequiredSkillsList)
			return;
		
		RL_CraftListBoxData data = new RL_CraftListBoxData;
		data.text = recipe.recipeName;
		data.quantity = 1;
		data.skillCategory = RL_SkillsCategory.CRAFTING;
		data.icon = "skill_icon";

		AddToList(m_wRequiredSkillsList, data);
	}
	
	//------------------------------------------------------------------------------------------------
	// Getter methods for tracked data don't think they are needed though
	//------------------------------------------------------------------------------------------------
	
	RL_CraftingRecipe GetCurrentRecipe()
	{
		return m_CurrentRecipe;
	}
	
	array<ref RL_CraftingIngredient> GetCurrentIngredients()
	{
		return m_CurrentIngredients;
	}
	
	int GetCurrentXPGain()
	{
		return m_iCurrentXPGain;
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		// Clean up any active timers when the widget is destroyed
		EndCrafting();
		super.HandlerDeattached(w);
	}
	
	//------------------------------------------------------------------------------------------------
	int GetModifiedCraftingTime(int baseCraftingTime)
	{
		if (!m_character)
			return baseCraftingTime;
		
		float timeMultiplier = 1.0;
		
		if (m_character.HasPerk("crafting_efficiency_ii"))
		{
			timeMultiplier = 0.8; // 20% reduction
		}
		else if (m_character.HasPerk("crafting_efficiency"))
		{
			timeMultiplier = 0.9; // 10% reduction
		}
		
		return Math.Round(baseCraftingTime * timeMultiplier);
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasAllRequiredItems()
	{
		if (!m_character || !m_CraftingSessionRecipe)
			return false;
			
		InventoryStorageManagerComponent inventoryManager = RL_InventoryUtils.GetResponsibleStorageManager(m_character);
		if (!inventoryManager)
			return false;
		
		m_TempPrefabs.Clear();
		m_TempQuantities.Clear();
		
		foreach (RL_CraftingIngredient ingredient : m_CraftingSessionRecipe.ingredients)
		{
			string prefab = ingredient.GetPrefab();
			int quantity = ingredient.GetQuantity() * m_iCraftingQuantity;

			int existingIndex = m_TempPrefabs.Find(prefab);
			if (existingIndex != -1)
			{
				m_TempQuantities[existingIndex] = m_TempQuantities[existingIndex] + quantity;
			}
			else
			{
				m_TempPrefabs.Insert(prefab);
				m_TempQuantities.Insert(quantity);
			}
		}
		
		for (int i = 0; i < m_TempPrefabs.Count(); i++)
		{
			string requiredPrefab = m_TempPrefabs[i];
			int requiredCount = m_TempQuantities[i];
			int availableCount = RL_InventoryUtils.GetAmount(inventoryManager, requiredPrefab, true);
			
			if (availableCount < requiredCount)
				return false;
		}
		
		return true;
	}
	
} 