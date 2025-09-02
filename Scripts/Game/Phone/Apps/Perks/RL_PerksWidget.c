sealed class RL_PerksWidget : RL_AppBaseWidget
{
    protected Widget m_contentLayout;
    protected SCR_InputButtonComponent m_leftCategoryButton;
    protected SCR_InputButtonComponent m_rightCategoryButton;
    protected GridLayoutWidget m_perksGrid;
    protected TextWidget m_titleText;
    protected TextWidget m_selectedPerkName;
    protected TextWidget m_selectedPerkDescription;
    protected TextWidget m_perkCostText;
    protected TextWidget m_currentPointsText;
    protected SCR_InputButtonComponent m_purchaseButton;
    protected TextWidget m_prerequisitesTitle;
    protected Widget m_prerequisitesHorizontalLayout;

    private const string PERK_GRID_ITEM = "{9AC020A354E94CE6}UI/Layouts/Phone/Apps/Perks/PerkItem.layout";
    private const string PERK_ICONS_IMAGESET = "{F5A85B3176986520}UI/Imagesets/PhoneIcons/phoneAppIcons.imageset";
    private const string PERK_CONFIG_PATH = "{5AFE299F7D419846}Configs/Perks/PerkList.conf";
    private const string DEFAULT_ICON = "DEALER";
    
    private const int MAX_COLUMNS = 4;
    private const int PREREQ_FONT_SIZE = 14;
    
    private const string TEXT_SELECT_PERK = "Select a Perk";
    private const string TEXT_CHOOSE_PERK_DETAILS = "Choose a perk to see details";
    private const string TEXT_OWNED = "Owned";
    private const string TEXT_MISSING_POINTS = "Missing Points";
    private const string TEXT_MISSING_REQS = "Missing reqs";
    private const string TEXT_PURCHASE = "Purchase";
    private const string TEXT_NONE = "None";
    private const string TEXT_COST_PREFIX = "Cost: ";
    private const string TEXT_POINTS_PREFIX = "Points: ";
    private const string TEXT_LEVEL_PREFIX = " Level ";
    
    private const int COLOR_WHITE = 0xFFFFFFFF;
    private const int COLOR_DISABLED_ICON = 0xFF666666;
    private const int COLOR_DISABLED_TEXT = 0xFF888888;
    private const int COLOR_SUCCESS = 0xFF00AA00;
    private const int COLOR_ERROR = 0xFFAA0000;
    
    private ref array<ref RL_PerkDefinition> m_allPerks = {};
    private ref array<ref RL_PerkDefinition> m_currentCategoryPerks = {};
    private ref array<ref SCR_ModularButtonComponent> m_perkButtons = {};
    private ref array<ref RL_SkillData> m_playerSkills = {};
    
    private int m_selectedPerkIndex = -1;
    private int m_currentPerkPoints = 0;
    private RL_SkillsCategory m_currentCategory = RL_SkillsCategory.MINING;
    private bool m_dataLoaded = false;
    private bool m_skillsLoaded = false;
    private bool m_perkPointsLoaded = false;

	//------------------------------------------------------------------------------------------------
    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
        if(!ValidateBasicComponents()) 
			return;
            
        InitializeUIComponents();
        SetupEventListeners();
        LoadAllData();
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
		CleanupEventListeners();
	}
	
	//------------------------------------------------------------------------------------------------
    private bool ValidateBasicComponents()
    {
        return m_wRoot && m_character;
    }
    
    //------------------------------------------------------------------------------------------------
    private void InitializeUIComponents()
    {
        m_contentLayout = Widget.Cast(m_wRoot.FindAnyWidget("contentLayout"));
        if(!m_contentLayout) return;
            
        m_leftCategoryButton = SCR_InputButtonComponent.GetInputButtonComponent("LeftCategoryButton", m_wRoot);
        m_rightCategoryButton = SCR_InputButtonComponent.GetInputButtonComponent("RightCategoryButton", m_wRoot);
        
        m_perksGrid = GridLayoutWidget.Cast(m_wRoot.FindAnyWidget("perksGrid"));
        m_titleText = TextWidget.Cast(m_wRoot.FindAnyWidget("titleText"));
        m_selectedPerkName = TextWidget.Cast(m_wRoot.FindAnyWidget("selectedPerkName"));
        m_selectedPerkDescription = TextWidget.Cast(m_wRoot.FindAnyWidget("selectedPerkDescription"));
        m_perkCostText = TextWidget.Cast(m_wRoot.FindAnyWidget("perkCostText"));
        m_currentPointsText = TextWidget.Cast(m_wRoot.FindAnyWidget("currentPointsText"));
        
        m_purchaseButton = SCR_InputButtonComponent.GetInputButtonComponent("purchaseButton", m_wRoot);
        m_prerequisitesTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("prerequisitesLabel"));
        m_prerequisitesHorizontalLayout = Widget.Cast(m_wRoot.FindAnyWidget("thePrerequisites"));
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetupEventListeners()
    {
        if(m_leftCategoryButton)
            m_leftCategoryButton.m_OnActivated.Insert(OnLeftCategoryClicked);
            
        if(m_rightCategoryButton)
            m_rightCategoryButton.m_OnActivated.Insert(OnRightCategoryClicked);
            
        if(m_purchaseButton)
            m_purchaseButton.m_OnActivated.Insert(OnPurchaseClicked);
    }
    
    //------------------------------------------------------------------------------------------------
    private void CleanupEventListeners()
    {
        if(m_leftCategoryButton)
            m_leftCategoryButton.m_OnActivated.Remove(OnLeftCategoryClicked);
            
        if(m_rightCategoryButton)
            m_rightCategoryButton.m_OnActivated.Remove(OnRightCategoryClicked);
            
        if(m_purchaseButton)
            m_purchaseButton.m_OnActivated.Remove(OnPurchaseClicked);
    }
    
    //------------------------------------------------------------------------------------------------
    private void LoadAllData()
    {
        LoadPerkDefinitions();
        LoadPlayerSkills();
        LoadPerkPoints();
    }
	
	//------------------------------------------------------------------------------------------------
    void LoadPerkDefinitions()
    {
        Resource perkConfigResource = BaseContainerTools.LoadContainer(PERK_CONFIG_PATH);
        if(!perkConfigResource)
        {
            Print("[RL_PerksWidget] Failed to load perk config");
            return;
        }
        
        RL_PerksConfig perkConfig = RL_PerksConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(perkConfigResource.GetResource().ToBaseContainer()));
        if(!perkConfig || !perkConfig.Perks)
        {
            Print("[RL_PerksWidget] Failed to parse perk config");
            return;
        }
        
        m_allPerks = perkConfig.Perks;
        m_dataLoaded = true;
        CheckIfReadyToDisplay();
    }
    
    //------------------------------------------------------------------------------------------------
    void LoadPlayerSkills()
    {
        if(m_character)
            m_character.GetAllSkills(this, "OnSkillsLoaded");
    }
    
    //------------------------------------------------------------------------------------------------
    void OnSkillsLoaded(bool success, array<ref RL_SkillData> skills)
    {
        m_skillsLoaded = success;
        if(success && skills)
            m_playerSkills = skills;
        else
            m_playerSkills = new array<ref RL_SkillData>();
        CheckIfReadyToDisplay();
    }
    
    //------------------------------------------------------------------------------------------------
    void LoadPerkPoints()
    {
        if(m_character)
            m_character.GetAvailablePerkPoints(this);
    }
    
    //------------------------------------------------------------------------------------------------
    void OnPerkPointsLoaded(int perkPoints)
    {
        m_currentPerkPoints = perkPoints;
        m_perkPointsLoaded = true;
        UpdateCurrentPoints();
        CheckIfReadyToDisplay();
    }
    
    //------------------------------------------------------------------------------------------------
    void CheckIfReadyToDisplay()
    {
        if(m_dataLoaded && m_skillsLoaded && m_perkPointsLoaded)
        {
            EnsureAccessibleCategory();
            UpdateCategoryTitle();
            FillPerks();
        }
    }
    
    //------------------------------------------------------------------------------------------------
    private void EnsureAccessibleCategory()
    {
        if(!IsCategoryAccessible(m_currentCategory))
        {
            m_currentCategory = RL_SkillsCategory.CHARACTER;
        }
    }
    
    //------------------------------------------------------------------------------------------------
    private bool IsCategoryAccessible(RL_SkillsCategory category)
    {
        if(!m_character) return false;
            
        switch(category)
        {
            case RL_SkillsCategory.POLICE:
                return m_character.IsPolice();
            case RL_SkillsCategory.EMS:
                return m_character.IsMedic();
            default:
                return true;
        }
		
		return true;
    }
    
    //------------------------------------------------------------------------------------------------
    private array<RL_SkillsCategory> GetAccessibleCategories()
    {
        array<RL_SkillsCategory> accessibleCategories = {};
        array<int> categories = {};
        SCR_Enum.GetEnumValues(RL_SkillsCategory, categories);
        
        foreach(int categoryValue : categories)
        {
            Print("[RL_PerksWidget] GetAccessibleCategories foreach");
            RL_SkillsCategory category = categoryValue;
            if(IsCategoryAccessible(category))
                accessibleCategories.Insert(category);
        }
        
        return accessibleCategories;
    }
    
    //------------------------------------------------------------------------------------------------
    void FillPerks()
    {
        if(!m_perksGrid || !m_dataLoaded) return;
            
        FilterPerksByCategory();
        ClearPerkGrid();
        PopulatePerkGrid();
    }
    
    //------------------------------------------------------------------------------------------------
    private void FilterPerksByCategory()
    {
        m_currentCategoryPerks.Clear();
        foreach(RL_PerkDefinition perk : m_allPerks)
        {
            Print("[RL_PerksWidget] FilterPerksByCategory foreach");
            if(perk.category == m_currentCategory)
                m_currentCategoryPerks.Insert(perk);
        }
    }
    
    //------------------------------------------------------------------------------------------------
    private void ClearPerkGrid()
    {
        SCR_WidgetHelper.RemoveAllChildren(m_perksGrid);
        m_perkButtons.Clear();
    }
    
    //------------------------------------------------------------------------------------------------
    private void PopulatePerkGrid()
    {
        int currRow = 0;
        int currCol = 0;
        
        foreach(int index, RL_PerkDefinition perkDef : m_currentCategoryPerks) 
        {
            Print("[RL_PerksWidget] PopulatePerkGrid foreach");
            Widget perkWidget = CreatePerkWidget(currCol, currRow);
            SetupPerkWidget(perkWidget, perkDef, index);
            
            currCol++;
            if (currCol >= MAX_COLUMNS) {
                currRow++;
                currCol = 0;
            }
        }
    }
    
    //------------------------------------------------------------------------------------------------
    private Widget CreatePerkWidget(int col, int row)
    {
        Widget perkWidget = GetGame().GetWorkspace().CreateWidgets(PERK_GRID_ITEM, m_perksGrid);
        GridSlot.SetColumn(perkWidget, col);
        GridSlot.SetRow(perkWidget, row);
        return perkWidget;
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetupPerkWidget(Widget perkWidget, RL_PerkDefinition perkDef, int index)
    {
        SetupPerkIcon(perkWidget, perkDef);
        SetupPerkText(perkWidget, perkDef);
        SetupPerkButton(perkWidget);
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetupPerkIcon(Widget perkWidget, RL_PerkDefinition perkDef)
    {
        ImageWidget perkIconWidget = ImageWidget.Cast(perkWidget.FindAnyWidget("Image0"));
        if(!perkIconWidget) return;
        
        string iconName;
        if(perkDef.icon != "")
            iconName = perkDef.icon;
        else
            iconName = DEFAULT_ICON;
        perkIconWidget.LoadImageFromSet(0, PERK_ICONS_IMAGESET, iconName);
        
        bool isOwned = m_character && m_character.HasPerk(perkDef.perkId);
        SetPerkIconState(perkIconWidget, isOwned);
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetupPerkText(Widget perkWidget, RL_PerkDefinition perkDef)
    {
        TextWidget perkTxtWidget = TextWidget.Cast(perkWidget.FindAnyWidget("Text0"));
        if(!perkTxtWidget) return;
        
        perkTxtWidget.SetText(perkDef.name);
        
        bool isOwned = m_character && m_character.HasPerk(perkDef.perkId);
        SetPerkTextState(perkTxtWidget, isOwned);
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetupPerkButton(Widget perkWidget)
    {
        ButtonWidget perkButton = ButtonWidget.Cast(perkWidget.FindAnyWidget("Button0"));
        SCR_ModularButtonComponent perkButtonComp = SCR_ModularButtonComponent.FindComponent(perkButton);
        if(!perkButtonComp) return;
        
        perkButtonComp.m_OnClicked.Insert(OnPerkClicked);
        m_perkButtons.Insert(perkButtonComp);
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetPerkIconState(ImageWidget icon, bool isOwned)
    {
        if(isOwned)
        {
            icon.SetSaturation(1.0);
            icon.SetColor(Color.FromInt(COLOR_WHITE));
        }
        else
        {
            icon.SetSaturation(0.0);
            icon.SetColor(Color.FromInt(COLOR_DISABLED_ICON));
        }
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetPerkTextState(TextWidget text, bool isOwned)
    {
        int color;
        if(isOwned)
            color = COLOR_WHITE;
        else
            color = COLOR_DISABLED_TEXT;
        text.SetColor(Color.FromInt(color));
    }
    
    //------------------------------------------------------------------------------------------------
    private void OnPerkClicked(SCR_ModularButtonComponent clickedButton)
    {
        int clickedIndex = m_perkButtons.Find(clickedButton);
        if(clickedIndex == -1 || clickedIndex >= m_currentCategoryPerks.Count())
            return;
            
        m_selectedPerkIndex = clickedIndex;
        UpdateSelectedPerk();
    }
    
    //------------------------------------------------------------------------------------------------
    private void UpdateSelectedPerk()
    {
        if(!IsValidPerkSelection()) return;
            
        RL_PerkDefinition selectedPerk = m_currentCategoryPerks[m_selectedPerkIndex];
        
        UpdatePerkDetails(selectedPerk);
        UpdatePrerequisites(selectedPerk);
        UpdatePurchaseButton(selectedPerk);
    }
    
    //------------------------------------------------------------------------------------------------
    private bool IsValidPerkSelection()
    {
        return m_selectedPerkIndex != -1 && m_selectedPerkIndex < m_currentCategoryPerks.Count();
    }
    
    //------------------------------------------------------------------------------------------------
    private void UpdatePerkDetails(RL_PerkDefinition perk)
    {
        SetTextSafely(m_selectedPerkName, perk.name);
        SetTextSafely(m_selectedPerkDescription, perk.description);
        SetTextSafely(m_perkCostText, TEXT_COST_PREFIX + perk.perkPointCost.ToString());
    }
    
    //------------------------------------------------------------------------------------------------
    private void SetTextSafely(TextWidget widget, string text)
    {
        if(widget) widget.SetText(text);
    }
    
    //------------------------------------------------------------------------------------------------
    private void UpdatePurchaseButton(RL_PerkDefinition perkDef)
    {
        if(!m_purchaseButton || !m_character) return;
            
        bool isOwned = m_character.HasPerk(perkDef.perkId);
        bool canAfford = (m_currentPerkPoints >= perkDef.perkPointCost);
        bool meetsPrereqs = CheckPrerequisites(perkDef);
        
        string label;
        bool enabled;
        
        if(isOwned)
        {
            label = TEXT_OWNED;
            enabled = false;
        }
        else if(!canAfford)
        {
            label = TEXT_MISSING_POINTS;
            enabled = false;
        }
        else if(!meetsPrereqs)
        {
            label = TEXT_MISSING_REQS;
            enabled = false;
        }
        else
        {
            label = TEXT_PURCHASE;
            enabled = true;
        }
        
        m_purchaseButton.SetEnabled(enabled);
        m_purchaseButton.SetLabel(label);
    }
    
    //------------------------------------------------------------------------------------------------
    private bool CheckPrerequisites(RL_PerkDefinition perkDef)
    {
        if(!m_character) return false;
        
        return CheckPrerequisitePerks(perkDef) && CheckSkillRequirement(perkDef);
    }
    
    //------------------------------------------------------------------------------------------------
    private bool CheckPrerequisitePerks(RL_PerkDefinition perkDef)
    {
        if(!perkDef.prerequisitePerks) return true;
        
        foreach(string prereqPerk : perkDef.prerequisitePerks)
        {
            Print("[RL_PerksWidget] CheckPrerequisitePerks foreach");
            if(!m_character.HasPerk(prereqPerk))
                return false;
        }
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    private bool CheckSkillRequirement(RL_PerkDefinition perkDef)
    {
        if(perkDef.requiredSkillLevel <= 0) return true;
        
        int playerLevel = GetPlayerSkillLevel(perkDef.category);
        return playerLevel >= perkDef.requiredSkillLevel;
    }
    
    //------------------------------------------------------------------------------------------------
    private int GetPlayerSkillLevel(RL_SkillsCategory category)
    {
        if(!m_playerSkills) return 0;
            
        foreach(RL_SkillData skill : m_playerSkills)
        {
            Print("[RL_PerksWidget] GetPlayerSkillLevel foreach");
            if(skill.GetCategory() == category)
                return RL_SkillsUtils.GetLevelFromXP(skill.GetAmount());
        }
        return 0;
    }
    
    //------------------------------------------------------------------------------------------------
    private void UpdatePrerequisites(RL_PerkDefinition perkDef)
    {
        if(!m_prerequisitesHorizontalLayout) return;
            
        SCR_WidgetHelper.RemoveAllChildren(m_prerequisitesHorizontalLayout);
        
        bool hasPerksPrereqs = AddPrerequisitePerks(perkDef);
        bool hasSkillPrereqs = AddSkillRequirement(perkDef);
        bool hasPrereqs = hasPerksPrereqs || hasSkillPrereqs;
        
        if(!hasPrereqs)
            AddNoPrerequisitesText();
    }
    
    //------------------------------------------------------------------------------------------------
    private bool AddPrerequisitePerks(RL_PerkDefinition perkDef)
    {
        if(!perkDef.prerequisitePerks || perkDef.prerequisitePerks.Count() == 0)
            return false;
            
        foreach(string prereqPerkId : perkDef.prerequisitePerks)
        {
            Print("[RL_PerksWidget] AddPrerequisitePerks foreach");
            string prereqName = GetPerkNameById(prereqPerkId);
            if(prereqName == "") prereqName = prereqPerkId;
            
            bool hasPrereq = m_character && m_character.HasPerk(prereqPerkId);
            int color;
            if(hasPrereq)
                color = COLOR_SUCCESS;
            else
                color = COLOR_ERROR;
            CreatePrerequisiteText(prereqName, color);
        }
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    private bool AddSkillRequirement(RL_PerkDefinition perkDef)
    {
        if(perkDef.requiredSkillLevel <= 0) return false;
            
        string categoryName = RL_SkillsUtils.CategoryIdToName(perkDef.category);
        string skillText = categoryName + TEXT_LEVEL_PREFIX + perkDef.requiredSkillLevel.ToString();
        
        int playerLevel = GetPlayerSkillLevel(perkDef.category);
        bool hasLevel = playerLevel >= perkDef.requiredSkillLevel;
        int color;
        if(hasLevel)
            color = COLOR_SUCCESS;
        else
            color = COLOR_ERROR;
        
        CreatePrerequisiteText(skillText, color);
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    private void AddNoPrerequisitesText()
    {
        CreatePrerequisiteText(TEXT_NONE, COLOR_SUCCESS);
    }
    
    //------------------------------------------------------------------------------------------------
    private void CreatePrerequisiteText(string text, int color)
    {
        Widget textWidget = GetGame().GetWorkspace().CreateWidget(WidgetType.TextWidgetTypeID, WidgetFlags.VISIBLE, Color.FromInt(COLOR_WHITE), 0, m_prerequisitesHorizontalLayout);
        TextWidget prereqWidget = TextWidget.Cast(textWidget);
        if(!prereqWidget) return;
        
        prereqWidget.SetText(text);
        prereqWidget.SetColor(Color.FromInt(color));
        prereqWidget.SetExactFontSize(PREREQ_FONT_SIZE);
    }
    
    //------------------------------------------------------------------------------------------------
    private string GetPerkNameById(string perkId)
    {
        foreach(RL_PerkDefinition perk : m_allPerks)
        {
            Print("[RL_PerksWidget] GetPerkNameById foreach");
            if(perk.perkId == perkId)
                return perk.name;
        }
        return "";
    }
    
    //------------------------------------------------------------------------------------------------
    private void UpdateCurrentPoints()
    {
        SetTextSafely(m_currentPointsText, TEXT_POINTS_PREFIX + m_currentPerkPoints.ToString());
    }
    
    //------------------------------------------------------------------------------------------------
    private void UpdateCategoryTitle()
    {
        if(m_titleText)
        {
            string categoryName = RL_SkillsUtils.CategoryIdToName(m_currentCategory);
            m_titleText.SetText(categoryName);
        }
    }
	
	//------------------------------------------------------------------------------------------------
    protected void OnLeftCategoryClicked()
    {
        NavigateCategory(-1);
    }
    
    //------------------------------------------------------------------------------------------------
    protected void OnRightCategoryClicked()
    {
        NavigateCategory(1);
    }
    
    //------------------------------------------------------------------------------------------------
    private void NavigateCategory(int direction)
    {
        array<RL_SkillsCategory> accessibleCategories = GetAccessibleCategories();
        if(accessibleCategories.Count() <= 1) return;
        
        int currentIndex = accessibleCategories.Find(m_currentCategory);
        if(currentIndex == -1) currentIndex = 0;
        
        currentIndex += direction;
        if(currentIndex < 0)
            currentIndex = accessibleCategories.Count() - 1;
        else if(currentIndex >= accessibleCategories.Count())
            currentIndex = 0;
            
        m_currentCategory = accessibleCategories[currentIndex];
        RefreshCategoryView();
    }
    
    //------------------------------------------------------------------------------------------------
    private void RefreshCategoryView()
    {
        UpdateCategoryTitle();
        FillPerks();
        ClearPerkSelection();
    }
    
    //------------------------------------------------------------------------------------------------
    private void ClearPerkSelection()
    {
        m_selectedPerkIndex = -1;
        SetTextSafely(m_selectedPerkName, TEXT_SELECT_PERK);
        SetTextSafely(m_selectedPerkDescription, TEXT_CHOOSE_PERK_DETAILS);
    }
    
    //------------------------------------------------------------------------------------------------
    protected void OnPurchaseClicked()
    {
        if(!IsValidPerkSelection() || !m_character) return;
            
        RL_PerkDefinition selectedPerk = m_currentCategoryPerks[m_selectedPerkIndex];
        
        if(!ValidatePurchase(selectedPerk)) return;
        
        m_character.PurchasePerk(selectedPerk.perkId, selectedPerk.perkPointCost);
    }
    
    //------------------------------------------------------------------------------------------------
    private bool ValidatePurchase(RL_PerkDefinition perk)
    {
        return CheckPrerequisites(perk) && m_currentPerkPoints >= perk.perkPointCost;
    }
    
    //------------------------------------------------------------------------------------------------
    void OnPerkPurchased(bool success, string perkId, int pointCost)
    {
        if(!success) return;
        
        LoadPerkPoints();
        FillPerks();
        
        if(IsValidPerkSelection())
        {
            RL_PerkDefinition selectedPerk = m_currentCategoryPerks[m_selectedPerkIndex];
            if(selectedPerk.perkId == perkId)
                UpdateSelectedPerk();
        }
    }
}