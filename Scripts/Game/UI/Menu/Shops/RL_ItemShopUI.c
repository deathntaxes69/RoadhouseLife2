class RL_ItemShopUI : ChimeraMenuBase
{
    protected RplId m_entityRplId;
    protected SCR_ChimeraCharacter m_character;

	protected Widget m_wRoot;
    protected TextWidget m_wShopTitle;
    protected SCR_SortHeaderComponent m_sortHeader;
    protected RL_ItemShopListBoxComponent m_shopListBox;
    protected int m_selectedCategory = 1;
    protected SCR_EditBoxComponent m_wQuantityInput;


    protected ref array<ref RL_BaseShopItem> m_shopData;
    protected ref array<ref RL_BaseShopItem> m_filteredShopData;

	
	override void OnMenuOpen() 
	{ 
        super.OnMenuOpen();
        
        
        m_wRoot = GetRootWidget();
        m_wShopTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("shopTitle"));

        Widget shopListWidget = Widget.Cast(m_wRoot.FindAnyWidget("shopList"));
        if(!shopListWidget) return;
        m_shopListBox = RL_ItemShopListBoxComponent.Cast(shopListWidget.FindHandler(RL_ItemShopListBoxComponent));
        m_shopListBox.m_OnChanged.Insert(OnSelectedItemChanged);

        m_wQuantityInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("QuantityEditBox").FindHandler(SCR_EditBoxComponent));

        SCR_InputButtonComponent buyButton = SCR_InputButtonComponent.GetInputButtonComponent("PurchaseButton", m_wRoot);
        if (buyButton)
            buyButton.m_OnActivated.Insert(OnPurchase);

        SCR_InputButtonComponent sellButton = SCR_InputButtonComponent.GetInputButtonComponent("SellButton", m_wRoot);
        if (sellButton)
            sellButton.m_OnActivated.Insert(OnSell);

        SCR_InputButtonComponent sellAllButton = SCR_InputButtonComponent.GetInputButtonComponent("SellAllButton", m_wRoot);
        if (sellAllButton)
            sellAllButton.m_OnActivated.Insert(OnSellAll);

        SCR_InputButtonComponent quantityDownButton = SCR_InputButtonComponent.GetInputButtonComponent("QuantityDownButton", m_wRoot);
        if (quantityDownButton)
            quantityDownButton.m_OnActivated.Insert(OnQuantityDown);

        SCR_InputButtonComponent quantityUpButton = SCR_InputButtonComponent.GetInputButtonComponent("QuantityUpButton", m_wRoot);
        if (quantityUpButton)
            quantityUpButton.m_OnActivated.Insert(OnQuantityUp);


        Widget sortWidget = m_wRoot.FindAnyWidget("SortHeader");
		if (!sortWidget) return;
		m_sortHeader = SCR_SortHeaderComponent.Cast(sortWidget.FindHandler(SCR_SortHeaderComponent));
		if (m_sortHeader)
            m_sortHeader.m_OnChanged.Insert(FillShop);
	
        

        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
    }
    
    void OnSelectedItemChanged(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {

    }
    void OnPurchase(SCR_InputButtonComponent btn)
    {
        if(!m_character || m_character.IsSpamming()) return;

        RL_BaseShopItem shopItem = GetSelectedShopItem();
        int quantity = GetQuantity();
        if (!shopItem || !quantity) return;

        int price = shopItem.GetPrice();
        if(!price) return;

        int totalPrice = quantity * price;
   
        m_character.PurchaseItems(shopItem.GetPrefab(), quantity, totalPrice, m_character.GetCharacterId());
    }
    void OnSell(SCR_InputButtonComponent btn)
    {
        if(!m_character || m_character.IsSpamming()) return;

        RL_BaseShopItem shopItem = GetSelectedShopItem();
        int quantity = GetQuantity();
        if (!shopItem || !quantity) return;
        
        int sellPrice = shopItem.GetSellPrice();
        if(!sellPrice) return;

        m_character.SellItems(shopItem.GetPrefab(), quantity, sellPrice, m_character.GetCharacterId());


    }
    void OnSellAll(SCR_InputButtonComponent btn)
    {
        if (!m_character)
        {
            Print("ItemShop OnSellAll character Not found");
            return;
        }
        if (m_character.IsSpamming()) 
        {
            Print("ItemShop OnSellAll isSpamming");
            return;
        }
        
        RL_BaseShopItem shopItem = GetSelectedShopItem();
        if (!shopItem)
        {
            Print("ItemShop OnSellAll shopItem Not found");
            return;
        }

        int sellPrice = shopItem.GetSellPrice();
        if(!sellPrice)
        {
            Print("ItemShop OnSellAll sellPrice Not found");
            return;
        }

        m_character.SellAllItems(shopItem.GetPrefab(), sellPrice, m_character.GetCharacterId());
    }
    RL_BaseShopItem GetSelectedShopItem()
    {
        int selectedRow =  m_shopListBox.GetSelectedItem();
        
        if (selectedRow == -1 || (selectedRow+1 > m_filteredShopData.Count()))
        {
            RL_Utils.Notify("No item selected.", "SHOP");
            return null;
        }
        return m_filteredShopData[selectedRow];
    }
    void OnQuantityUp(SCR_InputButtonComponent btn)
    {
        int quantity = m_wQuantityInput.GetValue().ToInt();
        m_wQuantityInput.SetValue(string.Format("%1", quantity+1));
    }
    void OnQuantityDown(SCR_InputButtonComponent btn)
    {
        int quantity = m_wQuantityInput.GetValue().ToInt();
        if(quantity < 2)
            quantity = 2;
        m_wQuantityInput.SetValue(string.Format("%1", quantity-1));
    }
    int GetQuantity()
    {
        int quantity = m_wQuantityInput.GetValue().ToInt();

        if(quantity < 1 || quantity > 100)
        {
            RL_Utils.Notify("Quantity must be between 1 and 100.", "SHOP");
            return null;
        }
        return quantity;
    }
    void OnShopLoaded(bool success, string results)
    {
        RL_BaseShopArray castedResults = new RL_BaseShopArray();
		castedResults.ExpandFromRAW(results);
        m_shopData = castedResults.data;
        if(m_shopData && m_shopData.Count() > 0)
            FillShop();
    }
    void FillShop()
    {
        m_shopListBox.Clear();

        if(!m_sortHeader || !m_character)
            return;

        int sortColumn = m_sortHeader.GetSortElementId();
        bool ascending = m_sortHeader.GetSortOrderAscending();
    
        // Filter items first
        m_filteredShopData = {};
        foreach (RL_BaseShopItem shopItem : m_shopData)
        {
            Print("[RL_ItemShopUI] FillShop foreach");

            if (m_character.IsPolice() && (shopItem.GetRequiredRank() > m_character.GetPoliceRank() || shopItem.GetRequiredDept() > m_character.GetPoliceDept()))
                continue;

            string displayName = RL_ShopUtils.GetEntityDisplayName(shopItem.GetPrefab());
            if(!displayName)
                continue;
                
            // Filter category if it exists
            if(m_selectedCategory && m_selectedCategory != shopItem.GetCategory())
                continue;

            m_filteredShopData.Insert(shopItem);
        }

        // spagetti code from chat
        int count = m_filteredShopData.Count();
        for(int i = 0; i < count - 1; i++)
        {
            Print("[RL_ItemShopUI] FillShop for");
            for(int j = 0; j < count - i - 1; j++)
            {
                Print("[RL_ItemShopUI] FillShop for 2");
                bool shouldSwap = false;
                
                if(sortColumn == 0) // Name
                {
                    				string nameA = SCR_StringHelper.Translate(RL_ShopUtils.GetEntityDisplayName(m_filteredShopData[j].GetPrefab()));
				string nameB = SCR_StringHelper.Translate(RL_ShopUtils.GetEntityDisplayName(m_filteredShopData[j+1].GetPrefab()));
                    
                    if(ascending)
                        shouldSwap = (nameA.Compare(nameB) > 0);
                    else
                        shouldSwap = (nameA.Compare(nameB) < 0);
                }
                else if(sortColumn == 1) // Price
                {
                    int priceA = m_filteredShopData[j].GetPrice();
                    int priceB = m_filteredShopData[j+1].GetPrice();
                    
                    if(ascending)
                        shouldSwap = (priceA > priceB);
                    else
                        shouldSwap = (priceA < priceB);
                }
                else if(sortColumn == 2) // Sell Price
                {
                    int sellPriceA = m_filteredShopData[j].GetSellPrice();
                    int sellPriceB = m_filteredShopData[j+1].GetSellPrice();
                    
                    if(ascending)
                        shouldSwap = (sellPriceA > sellPriceB);
                    else
                        shouldSwap = (sellPriceA < sellPriceB);
                }
                
                if(shouldSwap)
                {
                    RL_BaseShopItem temp = m_filteredShopData[j];
                    m_filteredShopData[j] = m_filteredShopData[j+1];
                    m_filteredShopData[j+1] = temp;
                }
            }
        }

        // Add sorted items to list
        foreach (RL_BaseShopItem shopItem : m_filteredShopData)
        {
            Print("[RL_ItemShopUI] FillShop foreach 2");
            string displayName = SCR_StringHelper.Translate(RL_ShopUtils.GetEntityDisplayName(shopItem.GetPrefab()));
            m_shopListBox.AddMultiColumnItem(displayName, RL_Utils.FormatMoney(shopItem.GetPrice()), RL_Utils.FormatMoney(shopItem.GetSellPrice()));
        }
        m_shopListBox.SetItemSelected(0, true);
        m_shopListBox.SetFocusOnFirstItem();
    }
    void SetEntities(IEntity owner, IEntity player)
	{
        m_entityRplId = EPF_NetworkUtils.GetRplId(owner);
        m_character = SCR_ChimeraCharacter.Cast(player);
	}
    void CloseMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}

}