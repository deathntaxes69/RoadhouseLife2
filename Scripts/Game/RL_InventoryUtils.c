modded class RL_InventoryUtils
{
	//------------------------------------------------------------------------------------------------
	//! Returns the total amount or -1 on faulty opertations.
	static int GetAmount(InventoryStorageManagerComponent target, ResourceName prefab, bool includeEquipped = false)
	{
		if (!target) return -1;

		if (includeEquipped)
		{
			array<IEntity> foundItems = {};
			SCR_PrefabNamePredicate prefabNamePredicate = new SCR_PrefabNamePredicate();
			prefabNamePredicate.prefabName = prefab;
			target.FindItems(foundItems, prefabNamePredicate, EStoragePurpose.PURPOSE_ANY);
			return foundItems.Count();
		}
		else
		{
			return target.GetDepositItemCountByResource(target.GetOwner(), prefab);
		}
	}

	static int GetAmount(IEntity target, ResourceName prefab, bool includeEquipped = false)
	{
		InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(target);
		return GetAmount(invManager, prefab, includeEquipped);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns actual amount added or 0 on faulty operations.
	static int AddAmount(InventoryStorageManagerComponent storageManager, ResourceName prefab, int amount, bool dropOverflow = false)
	{
		if (!storageManager || amount <= 0) return 0;

		int remainingAmount = amount;


		while (remainingAmount > 0)
		{
			//Print("AddAmount while loop");
			IEntity item = EL_Utils.SpawnEntityPrefab(prefab, storageManager.GetOwner().GetOrigin());
			if (!item || (!storageManager.TryInsertItem(item) && !dropOverflow))
			{
				RplComponent.DeleteRplEntity(item, false);
				break;
			}

			remainingAmount--;
		}

		return amount - remainingAmount;
	}
	static int AddAmount(IEntity target, ResourceName prefab, int amount, bool dropOverflow = false)
	{
		InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(target);
		return AddAmount(invManager, prefab, amount, dropOverflow);
	}

	//------------------------------------------------------------------------------------------------
	// Maybe we just add this parameter to AddAmount to not duplicate code but idk
	static int AddAmountWithIds(InventoryStorageManagerComponent storageManager, ResourceName prefab, int amount, out array<RplId> addedItemIds, bool dropOverflow = false)
	{
		addedItemIds = {};
		if (!storageManager || amount <= 0) return 0;

		int remainingAmount = amount;

		while (remainingAmount > 0)
		{
			//Print("AddAmountWithIds while loop");
			IEntity item = EL_Utils.SpawnEntityPrefab(prefab, storageManager.GetOwner().GetOrigin());
			if (!item || (!storageManager.TryInsertItem(item) && !dropOverflow))
			{
				RplComponent.DeleteRplEntity(item, false);
				break;
			}

			RplId itemId = EPF_NetworkUtils.GetRplId(item);
			if (itemId.IsValid())
				addedItemIds.Insert(itemId);

			remainingAmount--;
		}

		return amount - remainingAmount;
	}
	
	static int AddAmountWithIds(IEntity target, ResourceName prefab, int amount, out array<RplId> addedItemIds, bool dropOverflow = false)
	{
		InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(target);
		return AddAmountWithIds(invManager, prefab, amount, addedItemIds, dropOverflow);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns acutal amount removed or 0 on faulty opertations.
	static int RemoveAmount(InventoryStorageManagerComponent storageManager, ResourceName prefab, int amount, bool includeEquipped = false)
	{
		if (!storageManager || amount <= 0) return 0;
		int remainingAmount = amount;

		array<IEntity> prefabItems = FindItemsByPrefab(storageManager, prefab, includeEquipped);
		if(prefabItems.Count() != amount)
		{
			PrintFormat("RemoveAmount - Quantity mismatch, Requested: %1 , Available: %2 , Prefab: %3", amount, prefabItems.Count(), prefab);
		}
	
		foreach (IEntity item : prefabItems)
		{
			//Print("RemoveAmount foreach loop");
			storageManager.TryDeleteItem(item);
			if (--remainingAmount <= 0) return amount;
		}
		

		return amount - remainingAmount;
	}
	
	static int RemoveAmount(IEntity target, ResourceName prefab, int amount, bool includeEquipped = false)
	{
		InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(target);
		return RemoveAmount(invManager, prefab, amount, includeEquipped);
	}

	//------------------------------------------------------------------------------------------------
	static IEntity FindItemByReplicationId(InventoryStorageManagerComponent storageManager, RplId replicationId)
	{
		if (!storageManager) return null;

		array<IEntity> allItems();
		InventorySearchPredicate predicate();
		storageManager.FindItems(allItems, predicate);
		
		foreach (IEntity item : allItems)
		{
			//Print("FindItemByReplicationId foreach loop");
			if (!item) continue;
			RplId itemRplId = EPF_NetworkUtils.GetRplId(item);
			if (itemRplId == replicationId)
				return item;
		}
		
		return null;
	}

	//------------------------------------------------------------------------------------------------
	static IEntity FindItemByReplicationId(IEntity target, RplId replicationId)
	{
		InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(target);
		return FindItemByReplicationId(invManager, replicationId);
	}
}
