modded class SCR_LootDeadBodyAction
{
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;

        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!controller)
			return false;
		
		// Comment out only looting while dead as we ahve whitelisted items now
		
		//if (controller.GetLifeState() == ECharacterLifeState.INCAPACITATED)
		//	return false;

		// Disallow looting when in vehicle
		IEntity userVeh = CompartmentAccessComponent.GetVehicleIn(ownerChar);
		if (userVeh)
			return false;

        // Allow police to search if target is handcuffed
        if (ownerChar.IsHandcuffed() && userChar.IsPolice())
            return true;
		
		// Allow civ to search if ziptied
		if (ownerChar.IsZiptied())
            return true;
		
		return false;

		return super.CanBePerformedScript(user);
	}
}
modded class SCR_InventoryStorageManagerComponent
{
	protected bool IsItemWhitelistForbidden(IEntity item)
	{
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComp)
			return false;
			
		ItemAttributeCollection itemAttributes = itemComp.GetAttributes();
		if (!itemAttributes)
			return false;
			
		SCR_ItemAttributeCollection scrAttributes = SCR_ItemAttributeCollection.Cast(itemAttributes);
		if (!scrAttributes)
			return false;
			
		return !scrAttributes.GetIsWhitelisted();
	}
	
	override protected bool ShouldForbidRemoveByInstigator(InventoryStorageManagerComponent instigatorManager, BaseInventoryStorageComponent fromStorage, IEntity item)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(instigatorManager.GetOwner());

		if (ownerChar && userChar && ownerChar.IsHandcuffed() && userChar.IsPolice())
            return false;
		
		if (ownerChar && userChar && ownerChar.IsZiptied())
            return false;
		
		IEntity storageOwner = fromStorage.GetOwner();
		IEntity storageOwnerParent = storageOwner.GetParent();
		
		PrintFormat("ShouldForbidRemoveByInstigatorCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | userChar %2\nDEBUG SEARCH | storageOwner %3\nDEBUG SEARCH | storageOwnerParent %4\n===============================================================", ownerChar, userChar, storageOwner, storageOwnerParent);

		// Check if the storage is directly owned by a character
		SCR_ChimeraCharacter fromStorageChar = SCR_ChimeraCharacter.Cast(storageOwner);
		if (fromStorageChar && fromStorageChar.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
		{
			return IsItemWhitelistForbidden(item);
		}
		
		// Check if the storage is worn by a character
		SCR_ChimeraCharacter wearingCharacter = SCR_ChimeraCharacter.Cast(storageOwnerParent);
		if (wearingCharacter && wearingCharacter.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
		{
			return IsItemWhitelistForbidden(item);
		}
		
		return super.ShouldForbidRemoveByInstigator(instigatorManager, fromStorage, item);
	}
	
	override void InsertItem( IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null  )
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		
		PrintFormat("InsertItemCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | pItem %2\nDEBUG SEARCH | pStorageTo %3\nDEBUG SEARCH | pStorageFrom %4\n===============================================================", ownerChar, pItem, pStorageTo, pStorageFrom);
		
		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
	}
	
	override bool TrySwapItems( IEntity pOwnerEntity, BaseInventoryStorageComponent pStorageTo, SCR_InvCallBack cb = null )
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		
		PrintFormat("TrySwapItemsCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | pOwnerEntity %2\nDEBUG SEARCH | pStorageTo %3\n===============================================================", ownerChar, pOwnerEntity, pStorageTo);
		
		return super.TrySwapItems(pOwnerEntity, pStorageTo, cb);
	}
	
	override void EquipWeapon( IEntity pOwnerEntity, SCR_InvCallBack cb = null, bool bFromVicinity = true )
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		
		PrintFormat("EquipWeaponCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | pOwnerEntity %2\nDEBUG SEARCH | bFromVicinity %3\n===============================================================", ownerChar, pOwnerEntity, bFromVicinity);
		
		super.EquipWeapon(pOwnerEntity, cb, bFromVicinity);
	}
	
	override bool EquipAny(BaseInventoryStorageComponent storage, IEntity item, int preferred = -1, SCR_InvCallBack cb = null)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		
		PrintFormat("EquipAnyCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | storage %2\nDEBUG SEARCH | item %3\nDEBUG SEARCH | preferred %4\n===============================================================", ownerChar, storage, item, preferred);
		
		return super.EquipAny(storage, item, preferred, cb);
	}
	
	override bool TryReplaceItem( BaseInventoryStorageComponent storage, IEntity item, int preferred, SCR_InvCallBack cb )
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		
		PrintFormat("TryReplaceItemCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | storage %2\nDEBUG SEARCH | item %3\nDEBUG SEARCH | preferred %4\n===============================================================", ownerChar, storage, item, preferred);
		
		return super.TryReplaceItem(storage, item, preferred, cb);
	}
	
	override void EquipItem( EquipedWeaponStorageComponent weaponStorage, IEntity weapon )
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		
		PrintFormat("EquipItemCalled===============================================================\nDEBUG SEARCH | ownerChar %1\nDEBUG SEARCH | weaponStorage %2\nDEBUG SEARCH | weapon %3\n===============================================================", ownerChar, weaponStorage, weapon);
		
		super.EquipItem(weaponStorage, weapon);
	}
}