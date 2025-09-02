modded class SCR_UniversalInventoryStorageComponent {
	
	[RplProp(), Attribute(defvalue: "0", category: "Group", desc: "Enabled")]
    bool m_bStorageLocked = false;
	
	[RplProp()]
	string m_sLockPickingCharacterId = "";

	RL_VehicleManagerComponent m_vehicleManager;
	IEntity m_player;

	bool checkedForVehicleManager = false;
	bool IsStorageLocked() {
		if(!checkedForVehicleManager)
		{
			Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(GetOwner(), true));
			if(vehicle)
			{
				m_vehicleManager = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
				m_player = EntityUtils.GetPlayer();
			}
			checkedForVehicleManager = true;
		}
		//  FUCK IT JUST YOLO AND TRUST THE CLIENT
		//  GIMME THE FUCKING PLAYER OR EXPOSE THE DAMM CODE
		if(m_vehicleManager && RL_Utils.IsDedicatedServer())
			return false;

		SCR_ChimeraCharacter m_character = SCR_ChimeraCharacter.Cast(m_player);
		if (m_vehicleManager && m_character && m_character.IsPolice() && m_vehicleManager.HavePoliceSearched())
			return false;

		if(m_vehicleManager && m_player && m_vehicleManager.HasVehicleAccess(m_player))
			return false;
		
		return m_bStorageLocked;
	}
	void SetStorageLock(bool value){
		m_bStorageLocked = value;
		Replication.BumpMe();
	}
	
	void SetLockPickingCharacter(string characterId){
		m_sLockPickingCharacterId = characterId;
		Replication.BumpMe();
	}
	

	
	string GetLockPickingCharacterId()
	{
		return m_sLockPickingCharacterId;
	}

    override bool CanStoreItem(IEntity item, int slotID) {
        if (!super.CanStoreItem(item, slotID)) return false;
        if (IsStorageLocked()) return false;
        return true;
    }
	override bool CanRemoveItem(IEntity item) {
		if (!super.CanRemoveItem(item)) return false;
		if (IsStorageLocked()) return false;
		return true;
	}

 }
modded class SCR_InventorySlotUI {

	protected bool IsStorageLocked() {
		if(!m_pItem) return false;
		SCR_UniversalInventoryStorageComponent comp = SCR_UniversalInventoryStorageComponent.Cast(m_pItem.GetOwner().FindComponent(SCR_UniversalInventoryStorageComponent));
		return (comp && comp.IsStorageLocked());
	}
	
	override void UpdateReferencedComponent( InventoryItemComponent pComponent, SCR_ItemAttributeCollection attributes = null )
	{
		if ( m_widget )
			Destroy();
		m_pItem = pComponent;
		
		if (attributes)
			m_Attributes = attributes;
		
		if (m_pItem && m_pItem.GetAttributes())
			m_Attributes = SCR_ItemAttributeCollection.Cast( m_pItem.GetAttributes() );
		if (!m_Attributes)
			return;
		if (m_pItem && !m_Attributes.IsVisible(m_pItem))
			return;
		
		if (IsStorageLocked()) return;

		m_workspaceWidget = GetGame().GetWorkspace();
		Widget wGrid = m_pStorageUI.GetStorageGrid();
		
		if (!wGrid)
			return;
		
		m_widget = m_workspaceWidget.CreateWidgets( SetSlotSize(), wGrid );
		m_widget.AddHandler( this );
	}

 }
