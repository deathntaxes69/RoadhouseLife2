modded class SCR_InventoryStorageManagerComponent
{
	private IEntity m_CurrentOpenedStorage;
	
	//------------------------------------------------------------------------------------------------
	override void SetStorageToOpen(IEntity storage)
	{
		super.SetStorageToOpen(storage);
		m_CurrentOpenedStorage = storage;
	}

	//------------------------------------------------------------------------------------------------
	override void OnInventoryMenuClosed()
	{
		if (m_CurrentOpenedStorage)
		{
			RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(m_CurrentOpenedStorage);
			if (storageBox)
			{
				auto storageComp = SCR_UniversalInventoryStorageComponent.Cast(m_CurrentOpenedStorage.FindComponent(SCR_UniversalInventoryStorageComponent));
				if (storageComp)
				{
					SCR_ItemAttributeCollection itemAttributes = SCR_ItemAttributeCollection.Cast(storageComp.GetAttributes());
					if (itemAttributes)
						itemAttributes.SetVisible(false);
				}
				SaveStorageBox(storageBox);
				ResetStorageLoadedState(storageBox);
			}
			m_CurrentOpenedStorage = null;
		}
		
		super.OnInventoryMenuClosed();
	}
	
	//------------------------------------------------------------------------------------------------
	private void SaveStorageBox(RL_StorageBox_Entity storageBox)
	{
		if (!m_Storage)
			return;
			
		IEntity owner = m_Storage.GetOwner();
		if (!owner)
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(owner);
		if (!character)
			return;
			
		string storageType = storageBox.GetStorageType();
		if (storageType.IsEmpty())
			return;


		if (storageBox.GetOwnerId() != character.GetCharacterId())
			return;
			
		PrintFormat("[StorageDebug] Saving storage box %1 for character %2", storageType, character.GetCharacterId());
		character.SaveStorageBox(EPF_NetworkUtils.GetRplId(storageBox), storageType);
	}
	
	//------------------------------------------------------------------------------------------------
	private void ResetStorageLoadedState(RL_StorageBox_Entity storageBox)
	{
		if (!storageBox)
			return;
			
		IEntity owner = m_Storage.GetOwner();
		if (!owner)
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(owner);
		if (!character)
			return;
		
		character.ResetStorageLoadedState(storageBox);
	}
	
	override void InsertItem( IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null  )
	{
		if(pStorageTo) {
			IEntity owner = pStorageTo.GetOwner();
			
			auto trader = EL_TraderManagerComponent.Cast(owner.FindComponent(EL_TraderManagerComponent));
			if (trader) {
				// check if we get the correct Item traded
				if (pItem.GetPrefabData().GetPrefabName() != trader.m_ItemToReceive)
				{
					this.SetReturnCode(EInventoryRetCode.RETCODE_ITEM_TOO_BIG); // Todo check how we can improve this
					cb.InvokeOnFailed();
					return;
				}
				
				// delete Item that we get traded
				bool deleteSuccess = this.TryDeleteItem(pItem);
				if (!deleteSuccess){
					cb.InvokeOnFailed();
					return;
				}
				// spawn and insert ItemToGive into inventory
				EntitySpawnParams spawnParams();
        		spawnParams.Transform[3] = GetOwner().GetOrigin(); // Source origin	
				auto barterItem = GetGame().SpawnEntityPrefabEx(trader.m_ItemToGive, false, null, spawnParams);
				bool insertSuccess = TryInsertItemInStorage(barterItem, pStorageFrom);
				if (!insertSuccess)
				{
					TryInsertItemInStorage(GetGame().SpawnEntityPrefabEx(trader.m_ItemToReceive, false), pStorageFrom);
					return;
				}
				this.SetReturnCode(EInventoryRetCode.RETCODE_OK);
				cb.InvokeOnComplete();
				return;
			}
		}
		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
	}
	
	// Anti dupe
	override void Action_OpenInventory()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetOwner());
		
		if(!character)
			return;
		
		if(!character.GetCanOpenInventory())
			return;

		super.Action_OpenInventory();
	}
	
	// Reason for this and the above is because some things like opening trunk go straight to this method but want the instant ness of the top one also
	override void OpenInventory()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetOwner());
		
		if(!character)
			return;
		
		if(!character.GetCanOpenInventory())
			return;

		super.OpenInventory();
	}
}

modded class SCR_InvCallBack
{
	void InvokeOnComplete()
	{
		this.OnComplete();
	}

	void InvokeOnFailed()
	{
		this.OnFailed();
	}
};