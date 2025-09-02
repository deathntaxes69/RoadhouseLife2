class RL_ClothingShopUI : RL_BasePreviewShop
{
    protected SCR_SpinBoxComponent m_categorySelector;
    
    protected Widget m_cartLayout;
    private const string m_cartTextLayout = "{3132F64DB8B4CB44}UI/layouts/WidgetLibrary/TextWidgets/Text_Body.layout";
    
    protected ref array<ref RL_BaseShopItem> m_selectedShopData = {null, null, null, null, null, null, null};
    protected ref array<string> m_cartPrefabs = {};
    protected int m_cartTotal = 0;

	override void OnMenuOpen() 
	{ 
        super.OnMenuOpen();

        Widget categoryWidget = Widget.Cast(m_wRoot.FindAnyWidget("Category"));
        m_categorySelector = SCR_SpinBoxComponent.Cast(categoryWidget.FindHandler(SCR_SpinBoxComponent));
		
		if (!m_categorySelector) return;
		m_categorySelector.m_OnChanged.Insert(OnCategoryChanged);
      
        m_cartLayout = Widget.Cast(m_wRoot.FindAnyWidget("CartLayout"));


    }
    void SetupPreview()
    {
        
        if (RplSession.Mode() == RplMode.Dedicated && Replication.IsServer())
			return;
        Print("SetupPreview");
        ResourceName prefabName = RL_ShopUtils.ExtractEntityPrefabName(m_character);
        Resource loadedResource = Resource.Load(prefabName);
        m_previewEntity = GetGame().SpawnEntityPrefabLocal(loadedResource, GetGame().GetWorld(), null);
        m_previewEntity.GetPhysics().SetInteractionLayer(EPhysicsLayerDefs.CharNoCollide);
        m_previewManager.SetCameraMovementEnabled(true);
        CopyClothingToPreview();
        UpdatePreview();
    }
    override void OnSelectedItemChanged(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {
        if(selectedRow+1 > m_filteredShopData.Count() ) return;
        RL_BaseShopItem cloth = m_filteredShopData[selectedRow];
        
        m_selectedShopData.Set(m_categorySelector.GetCurrentIndex(), cloth);
        UpdateCart();
        
        RL_ShopUtils.SwapEntityClothing(m_previewEntity, cloth.GetPrefab());
        UpdatePreview();
    }
    void UpdateCart()
    {
        // Clear cart
        m_cartPrefabs = {};
        m_cartTotal = 0;
        Widget child = m_cartLayout.GetChildren();
		while (child)
		{
            Print("[RL_ClothingShopUI] UpdateCart while loop");
			Widget nextChild = child.GetSibling();
			m_cartLayout.RemoveChild(child);
			child = nextChild;
		}
        // Set cart text
        foreach (RL_BaseShopItem cloth : m_selectedShopData)
		{
            Print("[RL_ClothingShopUI] UpdateCart foreach loop");
            if(!cloth)
                continue;
            m_cartPrefabs.Insert(cloth.GetPrefab());
            Widget cartItemWidget = GetGame().GetWorkspace().CreateWidgets(m_cartTextLayout, m_cartLayout);
            TextWidget cartItem = TextWidget.Cast(cartItemWidget);
            		cartItem.SetText(RL_ShopUtils.GetEntityDisplayName(cloth.GetPrefab()));
            m_cartTotal = m_cartTotal + cloth.GetPrice();
        }
        // Update cart total
        Print(m_buyButton);

        m_buyButton.SetLabel(string.Format("Purchase (%1)", RL_Utils.FormatMoney(m_cartTotal)));
    }
    void OnCategoryChanged()
    {
        m_selectedCategory = m_categorySelector.GetCurrentIndex()+1;
        FillShop();
    }
    override void OnPurchase(SCR_InputButtonComponent btn)
    {
        Print("OnPurchase");
        if (!m_character || m_character.IsSpamming())
            return;

        if(!m_cartPrefabs || m_cartPrefabs.Count() < 1) 
        {
            RL_Utils.Notify("Your cart is empty.", "SHOP");
            return;
        }
        
        ref map<ResourceName, int> storedItems = StoreInventoryItems();
        m_character.PurchaseClothingWithItemTransfer(m_cartPrefabs, m_cartTotal, m_character.GetCharacterId(), storedItems);
        
        CloseMenu();

    }
    void CopyClothingToPreview()
    {
        if(!m_character)
        {
            GetGame().GetCallqueue().CallLater(CopyClothingToPreview, 100, false);
            return;
        }
        Print(m_character);
        array<IEntity> clothes = RL_ShopUtils.GetEntityClothing(m_character);
        // Loop prefabs from orginal chracter and add to preview
        foreach (IEntity cloth : clothes)
		{
            Print("[RL_BasePreviewShop] CopyClothingToPreview foreach");
            ResourceName prefabName = RL_ShopUtils.ExtractEntityPrefabName(cloth);
            
            if (prefabName.IsEmpty())
                continue;
            
            if(m_previewManager)
                RL_ShopUtils.SwapEntityClothing(m_previewEntity, prefabName);
                
        }
        UpdatePreview();
    }
    void UpdatePreview()
    {
        if(!m_previewEntity || !m_previewManager)
			return;
        m_previewManager.ConfigurePreview(RL_PreviewDisplayMode.ITEM_VIEW, m_previewEntity);
    }
    override void SetEntities(IEntity owner, IEntity player)
	{
		super.SetEntities(owner, player);
        SetupPreview();
	}
    
    ref map<ResourceName, int> StoreInventoryItems()
    {
        ref map<ResourceName, int> itemMap = new map<ResourceName, int>();
        
        InventoryStorageManagerComponent storageManager = RL_InventoryUtils.GetResponsibleStorageManager(m_character);
        if (!storageManager)
            return itemMap;
        
        array<BaseInventoryStorageComponent> storages = {};
        storageManager.GetStorages(storages);
        
        foreach (BaseInventoryStorageComponent storage : storages)
        {
            Print("[RL_BasePreviewShop] StoreInventoryItems foreach");
            if (storage.GetPurpose() != EStoragePurpose.PURPOSE_DEPOSIT)
                continue;
                
            array<IEntity> storageItems = {};
            storage.GetAll(storageItems);
            
            foreach (IEntity item : storageItems)
            {
                Print("[RL_BasePreviewShop] StoreInventoryItems foreach2");

                ResourceName prefab = RL_ShopUtils.ExtractEntityPrefabName(item);
                if (prefab.IsEmpty())
                    continue;
                    
                int currentCount = itemMap.Get(prefab);
                itemMap.Set(prefab, currentCount + 1);
            }
        }
        
        return itemMap;
    }

}