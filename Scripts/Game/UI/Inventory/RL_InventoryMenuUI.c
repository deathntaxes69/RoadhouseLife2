modded class SCR_InventoryMenuUI
{	
	protected const string BUTTON_TRANSFER_ALL = "ButtonTransferAll";

	override void OnAction(SCR_InputButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	
		if (action == "Inventory_Transfer_All")
		{
			Action_TransferAll();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Action_TransferAll()
	{
		if (!ValidateTransferPrerequisites())
			return;
		
		InventoryItemComponent focusedItemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		IEntity focusedItem = focusedItemComp.GetOwner();
		EntityPrefabData focusedPrefabData = focusedItem.GetPrefabData();
		
		SCR_InventoryStorageBaseUI sourceStorageUI = m_pFocusedSlotUI.GetStorageUI();
		BaseInventoryStorageComponent sourceStorage = sourceStorageUI.GetCurrentNavigationStorage();
		
		array<IEntity> itemsToTransfer = GetItemsToTransfer(sourceStorage, focusedPrefabData);
		if (itemsToTransfer.IsEmpty())
			return;
		
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(m_InventoryManager);
		if (!storageManager)
			return;
		
		SCR_InventoryStorageManagerComponent charStorageManager = SCR_InventoryStorageManagerComponent.Cast(m_InventoryManager);
		bool originalLockState = false;
		if (charStorageManager)
		{
			originalLockState = charStorageManager.IsInventoryLocked();
			if (originalLockState)
				charStorageManager.SetInventoryLocked(false);
		}
		
		SCR_InventoryStorageBaseUI targetStorageUI = FindTargetStorage(sourceStorage);
		int itemsTransferred = PerformTransfer(itemsToTransfer, sourceStorageUI, targetStorageUI, storageManager);
		
		if (charStorageManager)
			charStorageManager.SetInventoryLocked(originalLockState);
		
		PlayTransferFeedback(itemsTransferred > 0);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ValidateTransferPrerequisites()
	{
		if (!m_pFocusedSlotUI)
			return false;
		
		SCR_InventoryStorageBaseUI sourceStorageUI = m_pFocusedSlotUI.GetStorageUI();
		if (!sourceStorageUI)
			return false;
		
		InventoryItemComponent focusedItemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		if (!focusedItemComp)
			return false;
		
		IEntity focusedItem = focusedItemComp.GetOwner();
		if (!focusedItem || !focusedItem.GetPrefabData())
			return false;
		
		return SCR_InventoryStorageManagerComponent.Cast(m_InventoryManager) != null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected array<IEntity> GetItemsToTransfer(BaseInventoryStorageComponent sourceStorage, EntityPrefabData targetPrefabData)
	{
		array<IEntity> allItems = {};
		array<IEntity> itemsToTransfer = {};
		
		sourceStorage.GetAll(allItems);
		
		foreach (IEntity item : allItems)
		{
			Print("[RL_InventoryMenuUI] GetItemsToTransfer foreach");
			if (item && item.GetPrefabData() == targetPrefabData)
			{
				itemsToTransfer.Insert(item);
			}
		}
		
		return itemsToTransfer;
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_InventoryStorageBaseUI FindTargetStorage(BaseInventoryStorageComponent sourceStorage)
	{
		if (m_pStorageLootUI && m_pStorageLootUI.GetCurrentNavigationStorage() != sourceStorage)
		{
			BaseInventoryStorageComponent lootStorage = m_pStorageLootUI.GetCurrentNavigationStorage();
			if (lootStorage && !IsStorageArsenal(lootStorage))
			{
				return m_pStorageLootUI;
			}
		}
		
		if (m_aOpenedStoragesUI)
		{
			foreach (SCR_InventoryOpenedStorageUI openedStorage : m_aOpenedStoragesUI)
			{
				Print("[RL_InventoryMenuUI] FindTargetStorage foreach");
				if (!openedStorage)
					continue;
				
				BaseInventoryStorageComponent storage = openedStorage.GetCurrentNavigationStorage();
				if (storage && storage != sourceStorage && !IsStorageArsenal(storage))
				{
					return openedStorage;
				}
			}
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected int PerformTransfer(array<IEntity> itemsToTransfer, SCR_InventoryStorageBaseUI sourceStorageUI, SCR_InventoryStorageBaseUI targetStorageUI, InventoryStorageManagerComponent storageManager)
	{
		if (!targetStorageUI)
		{
			return TransferToCharacter(itemsToTransfer, storageManager);
		}
		
		BaseInventoryStorageComponent targetStorage = targetStorageUI.GetCurrentNavigationStorage();
		if (!targetStorage)
			return 0;
		
		return TransferToStorage(itemsToTransfer, targetStorage, storageManager);
	}
	
	//------------------------------------------------------------------------------------------------
	protected int TransferToCharacter(array<IEntity> itemsToTransfer, InventoryStorageManagerComponent storageManager)
	{
		int itemsTransferred = 0;
		
		foreach (IEntity item : itemsToTransfer)
		{
			Print("[RL_InventoryMenuUI] TransferToCharacter foreach");
			if (!item)
				continue;
			
			BaseInventoryStorageComponent bestStorage = storageManager.FindStorageForItem(item, EStoragePurpose.PURPOSE_ANY);
			if (!bestStorage)
				continue;
			
			if (storageManager.TryMoveItemToStorage(item, bestStorage))
			{
				itemsTransferred++;
			}
		}
		
		return itemsTransferred;
	}
	
	//------------------------------------------------------------------------------------------------
	protected int TransferToStorage(array<IEntity> itemsToTransfer, BaseInventoryStorageComponent targetStorage, InventoryStorageManagerComponent storageManager)
	{
		int itemsTransferred = 0;
		
		foreach (IEntity item : itemsToTransfer)
		{
			Print("[RL_InventoryMenuUI] TransferToStorage foreach");
			if (!item)
				continue;
			
			// Check if item can be moved to target storage before attempting
			if (!storageManager.CanMoveItemToStorage(item, targetStorage))
				continue;
			
			// Move item to target storage
			if (storageManager.TryMoveItemToStorage(item, targetStorage))
			{
				itemsTransferred++;
			}
		}
		
		return itemsTransferred;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PlayTransferFeedback(bool success)
	{
		if (success)
		{
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_CONTAINER_DIFR_DROP);
		}
		else
		{
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_DROP_ERROR);
		}
	}


	
	//------------------------------------------------------------------------------------------------
	// is there a way to access the variables set here when using super so I don't have to copy everything?
	override void NavigationBarUpdate()
	{
		if (!m_pNavigationBar)
			return;

		if (m_bIsUsingGamepad)
		{
			NavigationBarUpdateGamepad();
			if (m_pActiveStorageUI == m_pAttachmentStorageUI)
				return;
		}
		else
		{
			m_pNavigationBar.SetAllButtonEnabled(false);
			m_pNavigationBar.SetButtonEnabled(BUTTON_BACK, true);
			m_pNavigationBar.SetButtonEnabled(BUTTON_SELECT, !m_DraggedSlot);
	
			SCR_InventoryHitZoneUI hzSlot = SCR_InventoryHitZoneUI.Cast(m_pActiveHoveredStorageUI);
			m_pNavigationBar.SetButtonEnabled(BUTTON_REMOVE_TOURNIQUET, (hzSlot && hzSlot.IsTourniquetted()));			
		}

		if (m_DraggedSlot)
			return;

		if (!m_pFocusedSlotUI)
			return;

		InventoryItemComponent itemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		if (!itemComp)
			return;

		IEntity item = itemComp.GetOwner();
		if (!item)
			return;

		bool isItemInCharacter;
		SCR_ArsenalInventorySlotUI arsenalSlot = SCR_ArsenalInventorySlotUI.Cast(m_pFocusedSlotUI);
		if (!arsenalSlot)
		{
			isItemInCharacter = ItemBelongsToThePlayer(item);
		}
		else if (!arsenalSlot.IsAvailable())
		{
			m_pNavigationBar.SetButtonEnabled(BUTTON_SELECT, false);
			return;
		}

		if (isItemInCharacter)
		{
			m_pNavigationBar.SetButtonEnabled(BUTTON_USE, m_StorageManager.CanUseItem_Inventory(item));

			m_pNavigationBar.SetButtonEnabled(BUTTON_EQUIP, !m_pItemToAssign && m_StorageManager.CanEquipItem_Inventory(item));
			m_pNavigationBar.SetButtonEnabled(BUTTON_INSPECT, CanInspectItemInInventory(item, itemComp));
		}

		BaseInventoryStorageComponent desiredStorage = m_pStorageLootUI.GetCurrentNavigationStorage();
		bool isTargetingArsenal = isItemInCharacter && IsStorageArsenal(desiredStorage);

 		if (isItemInCharacter && !isTargetingArsenal && !desiredStorage && m_aOpenedStoragesUI)
			isTargetingArsenal = GetOpenArsenalStorage();

		if (isTargetingArsenal && m_aOpenedStoragesUI.Contains(SCR_InventoryOpenedStorageUI.Cast(m_pActiveHoveredStorageUI)))
			isTargetingArsenal = false;

		if (isTargetingArsenal && DoesSlotContainNonRefundableItems(m_pFocusedSlotUI))
			isTargetingArsenal = false;

		bool allowItemTransfer = !m_bIsUsingGamepad || m_pActiveStorageUI != m_pQuickSlotStorage;

		m_pNavigationBar.SetButtonEnabled(BUTTON_BUY, arsenalSlot != null);
		m_pNavigationBar.SetButtonEnabled(BUTTON_SELL, allowItemTransfer && isItemInCharacter && isTargetingArsenal);
		m_pNavigationBar.SetButtonEnabled(BUTTON_PICK_UP, !isItemInCharacter && !arsenalSlot);
		m_pNavigationBar.SetButtonEnabled(BUTTON_DROP, allowItemTransfer && m_pFocusedSlotUI.IsDraggable() && isItemInCharacter && !isTargetingArsenal);
		m_pNavigationBar.SetButtonEnabled(BUTTON_TRANSFER_ALL, allowItemTransfer && m_pFocusedSlotUI.IsDraggable() && !isTargetingArsenal);
		m_pNavigationBar.SetButtonEnabled(BUTTON_STEP_BACK, true);
	
		HandleSlottedItemFunction();
	}
	
}

