
class RL_OpenStorageBoxAction : SCR_InventoryAction
{
	RL_StorageBox_Entity m_storageBox;
	SCR_ItemAttributeCollection m_itemAttributes;

	//------------------------------------------------------------------------------------------------	
	override bool CanBePerformedScript(IEntity user)
 	{
		//Alawys allow admins
		//if(SCR_Global.IsAdmin(PlayerID))
		//	return true;
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		
		if (!character)
			return false;

		if (!m_storageBox)
		{
			SetCannotPerformReason("Storage box not found");
			return false;
		}
		
		if(m_storageBox.GetOwnerId() == character.GetCharacterId())
		{
			return true;
		}
		SetCannotPerformReason("Locked");
		return false;
 	}
	//------------------------------------------------------------------------------------------------	
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = "#AR-Inventory_OpenStorage";
		return true;
	}	
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity,pManagerComponent);
		
		m_storageBox = RL_StorageBox_Entity.Cast(pOwnerEntity);

		auto storgeComp = SCR_UniversalInventoryStorageComponent.Cast(pOwnerEntity.FindComponent( SCR_UniversalInventoryStorageComponent ));
		m_itemAttributes = SCR_ItemAttributeCollection.Cast(storgeComp.GetAttributes());
		
		if(m_itemAttributes)
			m_itemAttributes.SetVisible(false);	
				
	}
	//------------------------------------------------------------------------------------------------
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if(RL_Utils.IsDedicatedServer())
			return;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character || character.IsSpamming())
			return;
			
		RplComponent rplComp = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		if (!rplComp)
			return;
			
		RplId storageRplId = rplComp.Id();
		if (storageRplId == Replication.INVALID_ID)
			return;
		
		character.RequestOpenStorageBox(storageRplId);
	}
	
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
 	{

		SCR_InventoryStorageManagerComponent manager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!manager)
			return;
			
		PerformActionInternal(manager, pOwnerEntity, pUserEntity);
 	}	
	
};

modded class SCR_ItemAttributeCollection: ItemAttributeCollection
{
	void SetVisible(bool state = true)
	{
		m_bVisible = state;
	}
};
