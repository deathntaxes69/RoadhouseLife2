class RL_BasePreviewShop : ChimeraMenuBase
{
    protected RplId m_entityRplId;
    protected SCR_ChimeraCharacter m_character;
    protected RL_VehicleSystemComponent m_spawnComponent;

	protected Widget m_wRoot;
    protected TextWidget m_wShopTitle;
    protected SCR_ListBoxComponent m_shopListBox;
    protected int m_selectedCategory = 1;
    protected SCR_InputButtonComponent m_buyButton;

    protected RL_PreviewComponent m_previewManager;
    protected IEntity m_previewEntity;

    protected ref array<ref RL_BaseShopItem> m_shopData;
    protected ref array<ref RL_BaseShopItem> m_filteredShopData;

	
	override void OnMenuOpen() 
	{ 
        super.OnMenuOpen();
        
        
        m_wRoot = GetRootWidget();
        m_wShopTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("shopTitle"));
        m_previewManager = RL_PreviewComponent.Cast(m_wRoot.FindAnyWidget("ItemPreviewContainer").FindHandler(RL_PreviewComponent));

        Widget shopListWidget = Widget.Cast(m_wRoot.FindAnyWidget("shopList"));
        if(!shopListWidget) return;
        m_shopListBox = SCR_ListBoxComponent.Cast(shopListWidget.FindHandler(SCR_ListBoxComponent));
        m_shopListBox.m_OnChanged.Insert(OnSelectedItemChanged);
        ButtonWidget buyButtonWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("PurchaseButton"));
		if (buyButtonWidget)
		{
			m_buyButton = SCR_InputButtonComponent.Cast(buyButtonWidget.FindHandler(SCR_InputButtonComponent));
			if (m_buyButton)
			{
				m_buyButton.m_OnActivated.Insert(OnPurchase);
			}
		}
        
        m_previewManager.Initialize(m_character);

        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
    }
    
    override void OnMenuUpdate(float tDelta) {
        m_previewManager.SetCameraMovementEnabled(true);
		m_previewManager.Update(tDelta);
	}
    void OnSelectedItemChanged(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {

    }
    void OnPurchase(SCR_InputButtonComponent btn)
    {

    }
    void OnShopLoaded(bool success, string results)
    {
        Print("OnShopLoaded");
        RL_BaseShopArray castedResults = new RL_BaseShopArray();
		castedResults.ExpandFromRAW(results);
        m_shopData = castedResults.data;
        FillShop();
    }
    void FillShop()
    {
        m_filteredShopData = {};
        m_shopListBox.Clear();

        if (!m_character)
            return;

        // Add shop items to list box
        foreach (RL_BaseShopItem shopItem : m_shopData)
		{
            Print("[RL_BasePreviewShop] FillShop foreach");

            if (m_character.IsPolice() && (shopItem.GetRequiredRank() > m_character.GetPoliceRank() || shopItem.GetRequiredDept() > m_character.GetPoliceDept()))
                continue;

            string displayName = RL_ShopUtils.GetEntityDisplayName(shopItem.GetPrefab());
            if(!displayName)
                continue;
            // Filter catrogory if it exists
            if(m_selectedCategory && m_selectedCategory != shopItem.GetCategory())
                continue;
            m_shopListBox.AddItem(displayName);
            m_filteredShopData.Insert(shopItem);
        }
    }
    void SetEntities(IEntity owner, IEntity player)
	{
        Print("------SetEntities");
        Print(player);
        m_entityRplId = EPF_NetworkUtils.GetRplId(owner);
        m_character = SCR_ChimeraCharacter.Cast(player);
	}
    void CloseMenu()
	{
        //m_previewManager.DeletePreview();
		GetGame().GetMenuManager().CloseMenu(this);
        if(m_previewEntity)
            SCR_EntityHelper.DeleteEntityAndChildren( m_previewEntity );
	}

}