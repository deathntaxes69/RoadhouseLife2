class RL_HouseUpgradesUI : ChimeraMenuBase
{
    protected RplId m_entityRplId;
    protected SCR_ChimeraCharacter m_character;
    protected RL_HouseComponent m_houseComponent;
    protected RL_ApartmentComponent m_apartmentComponent;
    protected bool m_bIsApartment = false;
    
    protected Widget m_wRoot;
    protected TextWidget m_houseUpgradesTitle;
    protected SCR_ListBoxComponent m_houseUpgradesListBox;
    protected ButtonWidget m_buyButton;
    
    protected ref RL_HouseUpgradesConfig m_upgradesConfig;
    protected ref array<ref RL_HouseUpgradeDefinition> m_availableUpgrades;

    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen() 
    { 
        m_character = RL_Utils.GetLocalCharacter();
        
        m_wRoot = GetRootWidget();
        m_houseUpgradesTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("houseUpgradesTitle"));
        Widget upgradesListWidget = Widget.Cast(m_wRoot.FindAnyWidget("houseUpgradesList"));
        
        if (!upgradesListWidget) 
            return;
        
        m_houseUpgradesListBox = SCR_ListBoxComponent.Cast(upgradesListWidget.FindHandler(SCR_ListBoxComponent));
        
        ButtonWidget buyWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("BuyButton"));
        if (buyWidget)
        {
            SCR_InputButtonComponent buyButton = SCR_InputButtonComponent.Cast(buyWidget.FindHandler(SCR_InputButtonComponent));
            if (buyButton)
            {
                buyButton.m_OnActivated.Insert(BuyUpgrade);
            }
        }
        
        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
        LoadUpgradesConfig();
    }
        
    //------------------------------------------------------------------------------------------------
    protected void LoadUpgradesConfig()
    {
        Resource upgradesResource = BaseContainerTools.LoadContainer("{A5D6E4F5E2B14C03}Configs/Housing/HouseUpgradesList.conf");
        if (upgradesResource)
        {
            BaseContainer upgradesContainer = upgradesResource.GetResource().ToBaseContainer();
            if (upgradesContainer)
            {
                m_upgradesConfig = RL_HouseUpgradesConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(upgradesContainer));
            }
        }
        
        if (!m_upgradesConfig)
        {
            m_upgradesConfig = new RL_HouseUpgradesConfig();
        }
    }
    
    //------------------------------------------------------------------------------------------------
    protected array<ref RL_HouseUpgradeSpawnPoint> GetUpgradeSpawnPoints()
    {
        if (m_bIsApartment && m_apartmentComponent)
            return m_apartmentComponent.GetUpgradeSpawnPoints();
        else if (m_houseComponent)
            return m_houseComponent.GetUpgradeSpawnPoints();
        return null;
    }
    
    //------------------------------------------------------------------------------------------------
    protected array<int> GetOwnedUpgradeTypes()
    {
        if (m_bIsApartment && m_apartmentComponent)
            return m_apartmentComponent.GetOwnedUpgradeTypes();
        else if (m_houseComponent)
            return m_houseComponent.GetOwnedUpgradeTypes();
        return null;
    }
    
    //------------------------------------------------------------------------------------------------
    protected bool IsUpgradeAlreadyOwned(HouseUpgrades upgradeType)
    {
        if (m_bIsApartment && m_apartmentComponent)
            return m_apartmentComponent.HasUpgradeType(upgradeType);
        else if (m_houseComponent)
            return m_houseComponent.HasUpgradeType(upgradeType);
        return false;
    }
    
    //------------------------------------------------------------------------------------------------
    protected string GetPropertyId()
    {
        if (m_bIsApartment && m_apartmentComponent)
            return m_apartmentComponent.GetOwnerCid().ToString();
        else if (m_houseComponent)
            return m_houseComponent.GetHouseId();
        return "";
    }
        
    //------------------------------------------------------------------------------------------------
    protected void PopulateUpgradesList()
    {
        if (!m_houseUpgradesListBox || !m_upgradesConfig)
            return;
        
        array<ref RL_HouseUpgradeDefinition> allUpgrades = m_upgradesConfig.GetAllUpgrades();
        
        if (!allUpgrades || allUpgrades.Count() == 0)
            return;
        
        m_houseUpgradesListBox.Clear();
        m_availableUpgrades = new array<ref RL_HouseUpgradeDefinition>();
        
        foreach (RL_HouseUpgradeDefinition upgrade : allUpgrades)
        {
            Print("[RL_HouseUpgradesUI] PopulateUpgradesList foreach");
            if (!upgrade)
                continue;
            
            if (HasSpawnPointForUpgradeType(upgrade.GetUpgradeType()) && !IsUpgradeAlreadyOwned(upgrade.GetUpgradeType()))
            {
                m_availableUpgrades.Insert(upgrade);
                
                string displayText = string.Format("%1 - $%2", upgrade.GetName(), upgrade.GetBuyPrice());
                int newItem = m_houseUpgradesListBox.AddItem(displayText);
                
                SCR_ListBoxElementComponent elementComp = m_houseUpgradesListBox.GetElementComponent(newItem);
                if (elementComp)
                {
                    elementComp.SetEnabled(true);
                }
            }
        }
        m_houseUpgradesListBox.SetItemSelected(0, true);
        m_houseUpgradesListBox.SetFocusOnFirstItem();
    }
        
    //------------------------------------------------------------------------------------------------
    protected bool HasSpawnPointForUpgradeType(HouseUpgrades upgradeType)
    {
        array<ref RL_HouseUpgradeSpawnPoint> spawnPoints = GetUpgradeSpawnPoints();
        if (!spawnPoints)
            return false;
        
        foreach (RL_HouseUpgradeSpawnPoint point : spawnPoints)
        {
            Print("[RL_HouseUpgradesUI] HasSpawnPointForUpgradeType foreach");
            if (point && point.GetUpgradeType() == upgradeType)
                return true;
        }
        
        return false;
    }
        
    //------------------------------------------------------------------------------------------------
    protected void BuyUpgrade(SCR_InputButtonComponent btn)
    {
		if (m_character.IsSpamming())
			return;

        if (!m_houseUpgradesListBox || !m_availableUpgrades)
            return;
        
        int selectedRow = m_houseUpgradesListBox.GetSelectedItem();
        
        if (selectedRow == -1)
            return;
        
        if (selectedRow >= m_availableUpgrades.Count())
            return;
        
        RL_HouseUpgradeDefinition selectedUpgrade = m_availableUpgrades[selectedRow];
        if (!selectedUpgrade)
            return;

        if (IsUpgradeAlreadyOwned(selectedUpgrade.GetUpgradeType()))
        {
            string propertyType;
            if (m_bIsApartment)
                propertyType = "APARTMENT";
            else
                propertyType = "HOUSING";
            RL_Utils.Notify("You already own this upgrade!", propertyType);
            return;
        }
        
        int upgradePrice = selectedUpgrade.GetBuyPrice();
        int playerBank = m_character.GetBank();
        
        if (playerBank < upgradePrice)
        {
            string reason = string.Format("Insufficient bank funds. Need %1, have %2", 
                RL_Utils.FormatMoney(upgradePrice), RL_Utils.FormatMoney(playerBank));
            string propertyType;
            if (m_bIsApartment)
                propertyType = "APARTMENT";
            else
                propertyType = "HOUSING";
            RL_Utils.Notify(reason, propertyType);
            return;
        }
        
        PurchaseUpgrade(selectedUpgrade);
    }
        
    //------------------------------------------------------------------------------------------------
    protected void PurchaseUpgrade(RL_HouseUpgradeDefinition upgrade)
    {
        if (!m_character)
            return;
        
        string propertyId = GetPropertyId();
        if (propertyId.IsEmpty())
            return;
        
        if (m_bIsApartment)
        {
            m_character.BuyApartmentUpgrade(
                propertyId,
                upgrade.GetUpgradeType(),
                upgrade.GetName(),
                upgrade.GetBuyPrice(),
                upgrade.GetResourceNamePrefab()
            );
        }
        else
        {
            m_character.BuyHouseUpgrade(
                propertyId,
                upgrade.GetUpgradeType(),
                upgrade.GetName(),
                upgrade.GetBuyPrice(),
                upgrade.GetResourceNamePrefab()
            );
        }
        
        CloseMenu();
    }
        
    //------------------------------------------------------------------------------------------------
    void SetHouseEntity(IEntity houseEntity)
    {
        if (!houseEntity)
            return;
        
        m_entityRplId = EPF_NetworkUtils.GetRplId(houseEntity);
        
        m_houseComponent = RL_HouseComponent.Cast(houseEntity.FindComponent(RL_HouseComponent));
        if (m_houseComponent)
        {
            m_bIsApartment = false;
            if (m_houseUpgradesTitle)
                m_houseUpgradesTitle.SetText("House Upgrades");
        }
        else
        {
            m_apartmentComponent = RL_ApartmentComponent.Cast(houseEntity.FindComponent(RL_ApartmentComponent));
            if (m_apartmentComponent)
            {
                m_bIsApartment = true;
                if (m_houseUpgradesTitle)
                    m_houseUpgradesTitle.SetText("Apartment Upgrades");
            }
        }
        
        if (m_houseComponent || m_apartmentComponent)
        {
            PopulateUpgradesList();
        }
    }
        
    //------------------------------------------------------------------------------------------------
    void CloseMenu()
    {
        GetGame().GetMenuManager().CloseMenu(this);
    }
} 