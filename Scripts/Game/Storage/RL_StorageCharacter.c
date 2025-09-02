modded class SCR_ChimeraCharacter
{
	private ref map<RplId, bool> m_mPendingStorageRequests = new map<RplId, bool>();
	
	//------------------------------------------------------------------------------------------------
	void RequestOpenStorageBox(RplId storageRplId)
	{
		PrintFormat("[StorageCharacter] 1 Request open storage: %1", storageRplId);
		
		if (!storageRplId || storageRplId == Replication.INVALID_ID)
			return;
			
		if (m_mPendingStorageRequests.Contains(storageRplId))
		{
			RL_Utils.Notify("[StorageCharacter] Storage is already being loaded", "STORAGE");
			return;
		}
		
		m_mPendingStorageRequests.Set(storageRplId, true);
		Rpc(RpcAsk_OpenStorageBox, storageRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_OpenStorageBox(RplId storageRplId)
	{
		PrintFormat("[StorageCharacter] 2 Server processing request: %1", storageRplId);
		
		IEntity storageEntity = EPF_NetworkUtils.FindEntityByRplId(storageRplId);
		if (!storageEntity)
		{
			Rpc(RpcDo_StorageLoadFailed, storageRplId);
			return;
		}
			
		RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(storageEntity);
		if (!storageBox)
		{
			Rpc(RpcDo_StorageLoadFailed, storageRplId);
			return;
		}

		if (storageBox.GetOwnerId() != GetCharacterId())
		{
			Rpc(RpcDo_StorageAccessDenied, storageRplId);
			return;
		}
		
		if (storageBox.IsStorageLoaded())
		{
			PrintFormat("[StorageCharacter] 3 Already loaded sending ready");
			Rpc(RpcDo_StorageReady, storageRplId);
			return;
		}
		
		PrintFormat("[StorageCharacter] 3 Loading from database");
		LoadStorageBoxWithCallback(storageBox, storageRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	private void LoadStorageBoxWithCallback(RL_StorageBox_Entity storageBox, RplId storageRplId)
	{
		if (!storageBox)
		{
			Rpc(RpcDo_StorageLoadFailed, storageRplId);
			return;
		}
			
		string storageType = storageBox.GetStorageType();
		if (storageType.IsEmpty())
		{
			Rpc(RpcDo_StorageLoadFailed, storageRplId);
			return;
		}
			
		RL_StorageHelperComponent storageHelper = RL_StorageHelperComponent.Cast(FindComponent(RL_StorageHelperComponent));
		if (GetGame().GetGameMode())
			storageHelper = RL_StorageHelperComponent.Cast(GetGame().GetGameMode().FindComponent(RL_StorageHelperComponent));
		
		if (!storageHelper)
		{
			Rpc(RpcDo_StorageLoadFailed, storageRplId);
			return;
		}
		
		storageHelper.LoadStorageDataIntoExisting(storageBox, GetCharacterId(), storageType, this, storageRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnStorageLoadComplete(int storageRplId, bool success)
	{
		PrintFormat("[StorageCharacter] 4 Load complete result success: %1", success);
		
		if (!success)
		{
			Rpc(RpcDo_StorageLoadFailed, storageRplId);
			return;
		}
		
		Rpc(RpcDo_StorageReady, storageRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_StorageReady(RplId storageRplId)
	{
		PrintFormat("[StorageCharacter] 5 Opening inventory for storage: %1", storageRplId);
		
		m_mPendingStorageRequests.Remove(storageRplId);
		
		IEntity storageEntity = EPF_NetworkUtils.FindEntityByRplId(storageRplId);
		if (!storageEntity)
			return;
		
		RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(storageEntity);
		if (storageBox)
		{
			auto storageComp = SCR_UniversalInventoryStorageComponent.Cast(storageEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
			if (storageComp)
			{
				SCR_ItemAttributeCollection itemAttributes = SCR_ItemAttributeCollection.Cast(storageComp.GetAttributes());
				if (itemAttributes)
					itemAttributes.SetVisible(true);
			}
		}
		
		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(FindComponent(SCR_InventoryStorageManagerComponent));
		if (!invManager)
			return;
		
		invManager.SetStorageToOpen(storageEntity);
		invManager.OpenInventory();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_StorageAccessDenied(RplId storageRplId)
	{
		m_mPendingStorageRequests.Remove(storageRplId);
		RL_Utils.Notify("Storage is locked", "STORAGE");
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_StorageLoadFailed(RplId storageRplId)
	{
		m_mPendingStorageRequests.Remove(storageRplId);
		RL_Utils.Notify("Failed to load storage data", "STORAGE");
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetStorageLoadedState(RL_StorageBox_Entity storageBox)
	{
		if (!storageBox)
			return;
			
		RplComponent rplComp = RplComponent.Cast(storageBox.FindComponent(RplComponent));
		if (!rplComp)
			return;
			
		RplId storageRplId = rplComp.Id();
		
		if (!Replication.IsServer())
		{
			Rpc(RpcAsk_ResetStorageLoadedState, storageRplId);
			return;
		}
		
		if (storageBox.GetOwnerId() != GetCharacterId())
			return;
	
		storageBox.SetStorageLoaded(false);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ResetStorageLoadedState(RplId storageRplId)
	{
		IEntity storageEntity = EPF_NetworkUtils.FindEntityByRplId(storageRplId);
		if (!storageEntity)
			return;
		
		RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(storageEntity);
		if (!storageBox)
			return;
		
		if (storageBox.GetOwnerId() != GetCharacterId())
			return;
		
		storageBox.SetStorageLoaded(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void SaveStorageBox(int storageRplId, string storageType)
	{
		if (!Replication.IsServer())
		{
			Print("Not server requesting SaveStorageBox");
			Rpc(RpcAsk_SaveStorageBox, storageRplId, storageType);
			return;
		}
		Print("Server requesting SaveStorageBox");
		RpcAsk_SaveStorageBox(storageRplId, storageType);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SaveStorageBox(RplIdentity storageRplId, string storageType)
	{
		Print("RpcAsk_SaveStorageBox");
		IEntity storageEntity = EPF_NetworkUtils.FindEntityByRplId(storageRplId);
		Print(storageEntity);
		string persistenceId = EPF_PersistenceComponent.GetPersistentId(storageEntity);
		if (persistenceId.IsEmpty())
		{
			PrintFormat("[StorageDebug] Invalid persistence ID for storage entity %1, character id %2", storageRplId, GetCharacterId());
			return;
		}
		PrintFormat("[StorageDebug] RpcAsk_SaveStorageBox, Persistence ID %1 for character %2", persistenceId, GetCharacterId());
		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		Print(persistenceManager.FindEntityByPersistentId(persistenceId));
		Print("Storage entity ^^");
		
		RL_StorageHelperComponent storageHelper = RL_StorageHelperComponent.Cast(FindComponent(RL_StorageHelperComponent));
		if (GetGame().GetGameMode())
			storageHelper = RL_StorageHelperComponent.Cast(GetGame().GetGameMode().FindComponent(RL_StorageHelperComponent));
		
		if (!storageHelper)
			return;
		
		storageHelper.SaveStorageBox(GetCharacterId(), storageType, persistenceId);
	}
}
