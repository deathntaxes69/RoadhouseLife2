
/*modded class EPF_BaseInventoryStorageComponentSaveData
{
    // SEE "CUSTOM CONTINUE INSTEAD OF RETURNING ERROR"
    // THIS IS THE ONLY PART CHANGED SO THE REST OF INVENTORY IS LOADED WHEN ONE ITEM INVALID
	//------------------------------------------------------------------------------------------------
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(component);
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(owner.FindComponent(InventoryStorageManagerComponent));
		if (!storageManager) storageManager = EPF_GlobalInventoryStorageManagerComponent.GetInstance();

		bool isNotBaked = !EPF_BitFlags.CheckFlags(EPF_Component<EPF_PersistenceComponent>.Find(owner).GetFlags(), EPF_EPersistenceFlags.BAKED);
		set<int> processedSlots;
		if (isNotBaked)
			processedSlots = new set<int>();

		foreach (EPF_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			if (isNotBaked)
				processedSlots.Insert(slot.m_iSlotIndex);

			IEntity slotEntity = storageComponent.Get(slot.m_iSlotIndex);

			// Found matching entity, no need to spawn, just apply save-data
			if (slot.m_pEntity &&
				slotEntity &&
				EPF_Utils.GetPrefabGUID(EPF_Utils.GetPrefabName(slotEntity)) == EPF_Utils.GetPrefabGUID(slot.m_pEntity.m_rPrefab))
			{
				EPF_PersistenceComponent slotPersistence = EPF_Component<EPF_PersistenceComponent>.Find(slotEntity);
				if (slotPersistence && !slotPersistence.Load(slot.m_pEntity, false))
					return EPF_EApplyResult.ERROR;

				continue;
			}

			EPF_PersistenceComponent persistence = EPF_Component<EPF_PersistenceComponent>.Find(owner);
			if (EPF_BitFlags.CheckFlags(persistence.GetFlags(), EPF_EPersistenceFlags.BAKED))
			{
				EPF_BakedStorageChange change();
				if (slotEntity)
				{
					EPF_PersistenceComponent slotPersistence = EPF_Component<EPF_PersistenceComponent>.Find(slotEntity);
					if (!slotPersistence)
					{
						// Non persistence slot removals can only be overridden by dynamic entity placed into them
						change.m_sRemovedItemId = "PERMANENT_BAKED_REMOVAL";
					}
					else
					{
						// Remember slot removals
						change.m_sRemovedItemId = slotPersistence.GetPersistentId();
					}

				}
				else
				{
					// Remember slot replacement on baked entities
					change.m_bReplaced = true;
				}

				EPF_BakedStorageChange.Set(storageComponent, slot.m_iSlotIndex, change);
			}

			// Slot did not match save-data, delete current entity on it
			storageManager.TryDeleteItem(slotEntity);

			if (!slot.m_pEntity)
				continue;

			// Spawn new entity and attach it
			slotEntity = slot.m_pEntity.Spawn(false);
			if (!slotEntity)
				continue; //CUSTOM CONTINUE INSTEAD OF RETURNING ERROR

			// Teleport to target position so it is within valid range and if insert fails becomes visible overflow
			EPF_WorldUtils.Teleport(slotEntity, owner.GetOrigin(), owner.GetYawPitchRoll()[0]);

			// Unable to add it to the storage parent, so put it on the ground at the parent origin
			storageManager.TryInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotIndex);
		}

		// Delte any items not found in the storage data for non bakes that always save all slots
		if (isNotBaked)
		{
			for (int nSlot = 0, count = storageComponent.GetSlotsCount(); nSlot < count; nSlot++)
			{
				if (!processedSlots.Contains(nSlot))
				{
					IEntity slotEntity = storageComponent.Get(nSlot);
					if (slotEntity && slotEntity.FindComponent(EPF_PersistenceComponent))
						storageManager.TryDeleteItem(slotEntity);
				}
			}
		}

		return EPF_EApplyResult.OK;
	}

};*/
