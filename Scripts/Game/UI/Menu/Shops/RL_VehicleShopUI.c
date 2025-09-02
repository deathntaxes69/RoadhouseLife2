class RL_VehicleShopUI : RL_BasePreviewShop
{

	
	override void OnMenuOpen() 
	{ 
        super.OnMenuOpen();
        

    }

    override void OnSelectedItemChanged(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {
        if(selectedRow+1 > m_shopData.Count() ) return;
        RL_BaseShopItem vehicle = m_shopData[selectedRow];

        m_buyButton.SetLabel(string.Format("Purchase (%1)", RL_Utils.FormatMoney(vehicle.GetPrice())));

        if(m_previewEntity)
            SCR_EntityHelper.DeleteEntityAndChildren( m_previewEntity );
        
        BaseWorld world = GetGame().GetWorld();
        Resource loadedResource = Resource.Load(vehicle.GetPrefab());
        if(!loadedResource) return;
        m_previewEntity = GetGame().SpawnEntityPrefabLocal(loadedResource, world, null);
        if(!m_previewEntity) return;
        m_previewEntity.GetPhysics().SetInteractionLayer(EPhysicsLayerDefs.CharNoCollide);

        m_previewManager.ConfigurePreview(RL_PreviewDisplayMode.ITEM_VIEW, m_previewEntity);
        m_previewManager.SetCameraMovementEnabled(true);
    }
    override void OnPurchase(SCR_InputButtonComponent btn)
    {
        Print("OnSelectCharacter");
        if (!m_character || m_character.IsSpamming())
            return;

        int selectedRow =  m_shopListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_shopData.Count() ) return;
        RL_BaseShopItem vehicle = m_shopData[selectedRow];
        
        if(!m_spawnComponent) 
        {
            RL_Utils.Notify("Something went wrong spawning the vehicle, please make a bug report.", "SHOP");
            return;
        }
        m_character.PurchaseVehicle(m_entityRplId, vehicle.GetPrefab(), vehicle.GetPrice(), m_character.GetCharacterId(), m_character.GetCharacterName());
        
        CloseMenu();

    }
    override void FillShop()
    {
        super.FillShop();
        if(m_filteredShopData && (0 > m_filteredShopData.Count()) ) return;
        // Select first item
        m_shopListBox.SetItemSelected(0, true);
    }
    override void SetEntities(IEntity owner, IEntity player)
	{
		super.SetEntities(owner, player);
        m_spawnComponent = RL_VehicleSystemComponent.Cast(owner.FindComponent(RL_VehicleSystemComponent));
	}

}