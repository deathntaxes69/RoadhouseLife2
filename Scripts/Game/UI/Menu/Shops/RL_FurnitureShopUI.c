class RL_FurnitureShopUI : RL_BasePreviewShop
{

	
	override void OnMenuOpen() 
	{ 
        super.OnMenuOpen();
    }

    override void OnSelectedItemChanged(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {
        if(selectedRow+1 > m_filteredShopData.Count() ) return;
        RL_FurnitureShopItem furniture = RL_FurnitureShopItem.Cast(m_filteredShopData[selectedRow]);
        if(!furniture) return;

        m_buyButton.SetLabel(string.Format("Purchase (%1)", RL_Utils.FormatMoney(furniture.GetPrice())));

        if(m_previewEntity)
            SCR_EntityHelper.DeleteEntityAndChildren( m_previewEntity );
        
        BaseWorld world = GetGame().GetWorld();
        string previewPrefab = furniture.GetPreviewPrefab();
        if(previewPrefab.IsEmpty())
            previewPrefab = furniture.GetPrefab();
            
        Resource loadedResource = Resource.Load(previewPrefab);
        if(!loadedResource) return;
        m_previewEntity = GetGame().SpawnEntityPrefabLocal(loadedResource, world, null);
        if(!m_previewEntity) return;
        m_previewEntity.GetPhysics().SetInteractionLayer(EPhysicsLayerDefs.CharNoCollide);

        m_previewManager.ConfigurePreview(RL_PreviewDisplayMode.ITEM_VIEW, m_previewEntity);
        m_previewManager.SetCameraMovementEnabled(true);
    }
    
    override void OnPurchase(SCR_InputButtonComponent btn)
    {
        if(!m_character || m_character.IsSpamming()) return;

        Print("OnPurchaseFurniture");
        int selectedRow =  m_shopListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_filteredShopData.Count() ) return;
        RL_FurnitureShopItem furniture = RL_FurnitureShopItem.Cast(m_filteredShopData[selectedRow]);
        if(!furniture) return;
        
        int price = furniture.GetPrice();
        if(!price) return;

        m_character.PurchaseFurniture(furniture.GetPrefab(), price, m_character.GetCharacterId());
        
        CloseMenu();
    }
    
    override void OnShopLoaded(bool success, string results)
    {
        Print("OnFurnitureShopLoaded");
        RL_FurnitureShopArray castedResults = new RL_FurnitureShopArray();
		castedResults.ExpandFromRAW(results);
        
        m_shopData = {};
        foreach (RL_FurnitureShopItem furnitureItem : castedResults.data)
        {
            Print("[RL_FurnitureShopUI] OnShopLoaded foreach");
            m_shopData.Insert(furnitureItem);
        }
        
        FillShop();
    }
    

    
    override void FillShop()
    {
        super.FillShop();
        if(m_filteredShopData && (0 < m_filteredShopData.Count()) ) 
        {
            m_shopListBox.SetItemSelected(0, true);
        }
    }

}