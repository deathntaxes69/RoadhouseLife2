[ComponentEditorProps(category: "", description: "")]
class RL_StorageHelperComponentClass : ScriptComponentClass {}

class RL_StorageHelperComponent : ScriptComponent
{
	EPF_PersistenceManager m_persistenceManager;

	//------------------------------------------------------------------------------------------------
	void LoadStorageFromDb(EPF_PersistenceManager persistenceManager, string playerPersistenceID, int playerID, string storageType, int ownerRplId, int upgradeType, string upgradePrefab)
	{
		if (!persistenceManager)
			return;

		Tuple6<int, string, string, int, int, string> context(playerID, playerPersistenceID, storageType, ownerRplId, upgradeType, upgradePrefab);
		EDF_DbFindCallbackMultiple<EDF_DbEntity> loadHandler(this, "CallbackLoadStorageFromDb", context);
		persistenceManager
			.GetDbContext()
			.FindAllAsync(RL_StorageBoxSaveData, EDF_DbFind.And({
				EDF_DbFind.Field("m_sOwnerPersistenceID").Equals(playerPersistenceID),
				EDF_DbFind.Field("m_sStorageType").Equals(storageType)
			}), callback: loadHandler);
	}

	//------------------------------------------------------------------------------------------------
	void CallbackLoadStorageFromDb(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> findResults, Managed context)
	{
		Tuple6<int, string, string, int, int, string> typedContext = Tuple6<int, string, string, int, int, string>.Cast(context);
		if (!typedContext)
			return;
	
		int playerId = typedContext.param1;
		string playerPersistenceID = typedContext.param2;
		string storageType = typedContext.param3;
		int ownerRplId = typedContext.param4;
		int upgradeType = typedContext.param5;
		string upgradePrefab = typedContext.param6;

		m_persistenceManager = EPF_PersistenceManager.GetInstance();
		RL_StorageBox_Entity existingStorage = null;
		if (m_persistenceManager && findResults)
		{
			if (findResults.Count() == 1)
			{
				EPF_EntitySaveData saveData = EPF_EntitySaveData.Cast(findResults.Get(findResults.Count() - 1));
				if (saveData)
				{
					IEntity storage = m_persistenceManager.SpawnWorldEntity(saveData);
					existingStorage = RL_StorageBox_Entity.Cast(storage);
					if (existingStorage && Replication.IsServer())
						existingStorage.SetStorageLoaded(true);
				}
			}
			else if (findResults.Count() > 1)
				return;
		}

		ProcessStorageLoadCallback(playerId, playerPersistenceID, existingStorage, storageType, ownerRplId, upgradeType, upgradePrefab);
	}

	//------------------------------------------------------------------------------------------------
	void SaveStorageBox(string playerPersistenceID, string storageType, string persistenceId)
	{
		PrintFormat("[StorageDebug] SaveStorageBox for %1/%2", playerPersistenceID, storageType);
		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		if (!persistenceManager)
		{
			PrintFormat("[StorageDebug] No persistence manager instance 1");
			return;
		}
			
		Tuple3<string, string, string> context(playerPersistenceID, storageType, persistenceId);
		EDF_DbFindCallbackMultiple<EDF_DbEntity> loadHandler(this, "CallbackSaveFind", context);
		persistenceManager
			.GetDbContext()
			.FindAllAsync(RL_StorageBoxSaveData, EDF_DbFind.And({
				EDF_DbFind.Field("m_sOwnerPersistenceID").Equals(playerPersistenceID),
				EDF_DbFind.Field("m_sStorageType").Equals(storageType)
			}), callback: loadHandler);
	}

	//------------------------------------------------------------------------------------------------
	void CallbackSaveFind(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> findResults, Managed context)
	{
		Print("[StorageDebug] CallbackSaveFind");
		Print(statusCode);
		Tuple3<string, string, string> typedContext = Tuple3<string, string, string>.Cast(context);
		string playerPersistenceID = typedContext.param1;
		string storageType = typedContext.param2;
		string storagePersistenceID = typedContext.param3;

		PrintFormat("[StorageDebug] CallbackSaveFind for %1/%2 sentId=%3", playerPersistenceID, storageType, storagePersistenceID);
		if(!m_persistenceManager)
			m_persistenceManager = EPF_PersistenceManager.GetInstance();
		if (!m_persistenceManager)
		{
			PrintFormat("[StorageDebug] No persistence manager instance 2");
			return;
		}

		RL_StorageBox_Entity storageEnt;
		if (!findResults || findResults.Count() < 1)
		{
			PrintFormat("[StorageDebug] No storage found for %1/%2, will try to find %3", playerPersistenceID, storageType, storagePersistenceID);
		} else {
			EDF_DbEntity storageDbEntity = findResults.Get(findResults.Count() - 1);
			PrintFormat("[StorageDebug] Found storage for %1/%2, will save %3", playerPersistenceID, storageType, storageDbEntity.GetId());
			storageEnt = RL_StorageBox_Entity.Cast(m_persistenceManager.FindEntityByPersistentId(storageDbEntity.GetId()));
			if(storageEnt)
				storagePersistenceID = storageDbEntity.GetId();
		}
		if(!storageEnt)
		{
			PrintFormat("[StorageDebug] No storage from find results, trying to find sent persistence ID %1", storagePersistenceID);
			storageEnt = RL_StorageBox_Entity.Cast(m_persistenceManager.FindEntityByPersistentId(storagePersistenceID));
		}
		if (!storageEnt)
		{
			PrintFormat("[StorageDebug] Storage entity not found for ID %1", storagePersistenceID);
			return;
		}

		EPF_PersistenceComponent component = EPF_PersistenceComponent.Cast(storageEnt.FindComponent(EPF_PersistenceComponent));
		if (!component)
		{
			PrintFormat("[StorageDebug] No persistence component found on storage entity");
			return;
		}

		EPF_EntitySaveData saveData = component.Save();
		if (saveData)
		{
			m_persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
			PrintFormat("[StorageDebug] Storage saved for %1/%2 entid=%3", playerPersistenceID, storageType, storagePersistenceID);
		}
		else
		{
			PrintFormat("[StorageDebug] Failed to create save data for storage");
		}
	}

	//------------------------------------------------------------------------------------------------
	bool CreateStorage(IEntity storageEnt, string playerPersistenceID, string storageType)
	{
		PrintFormat("[StorageDebug] CreateStorage for %1/%2", playerPersistenceID, storageType);
		EPF_PersistenceComponent component = EPF_PersistenceComponent.Cast(storageEnt.FindComponent(EPF_PersistenceComponent));
		RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(storageEnt);

		if (!component || !storageBox)
			return false;

		storageBox.SetOwnerId(playerPersistenceID);
		storageBox.SetStorageType(storageType);
		
		EPF_EntitySaveData saveData = component.Save();
		if (saveData)
		{
			m_persistenceManager = EPF_PersistenceManager.GetInstance();
			m_persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	IEntity SpawnStorage(string storagePrefabPath, vector spawnPosition, RL_StorageBox_Entity existingStorage, string playerPersistenceID, string storageType, IEntity parentEntity = null, float rotationAngle = 0)
	{
		PrintFormat("[StorageDebug] SpawnStorage for %1/%2", playerPersistenceID, storageType);
		EntitySpawnParams spawnParams();
		spawnParams.Transform[3] = spawnPosition;
		
		IEntity storageEnt;
		
		if (!existingStorage)
		{
			Resource storagePrefab = Resource.Load(storagePrefabPath);
			if (!storagePrefab)
				return null;

			storageEnt = GetGame().SpawnEntityPrefab(storagePrefab, GetGame().GetWorld(), spawnParams);
			if (!storageEnt)
				return null;

			CreateStorage(storageEnt, playerPersistenceID, storageType);
		} else {
			existingStorage.SetWorldTransform(spawnParams.Transform);
			storageEnt = existingStorage;
		}

		// Rotate storage box entity
		storageEnt.SetAngles({0, rotationAngle, 0});
		storageEnt.Update();

		if (parentEntity)
		{
			parentEntity.AddChild(storageEnt, -1, EAddChildFlags.NONE);
		}
		
		return storageEnt;
	}

	//------------------------------------------------------------------------------------------------
	private void ProcessStorageLoadCallback(int playerId, string playerPersistenceID, RL_StorageBox_Entity existingStorage, string storageType, int ownerRplId, int upgradeType, string upgradePrefab)
	{
		PrintFormat("[StorageDebug] ProcessStorageLoadCallback %1/%2", playerPersistenceID, storageType);
		if (ownerRplId == Replication.INVALID_ID)
			return;
		
		IEntity owner = EPF_NetworkUtils.FindEntityByRplId(ownerRplId);
		if (!owner)
			return;
	
		RL_BaseUpgradeHelper.FinishStorageUpgradeLoadSimple(owner, upgradeType, upgradePrefab, storageType, existingStorage);
	}

	//------------------------------------------------------------------------------------------------
	void LoadStorageDataIntoExisting(RL_StorageBox_Entity storageEntity, string playerPersistenceID, string storageType, Managed callbackThis = null, int storageRplId = 0)
	{
		PrintFormat("[StorageDebug] LoadStorageDataIntoExisting for %1/%2", playerPersistenceID, storageType);
		if (!storageEntity)
			return;
			
		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		if (!persistenceManager)
			return;

		Tuple4<int, string, string, Managed> context(storageRplId, playerPersistenceID, storageType, callbackThis);
		EDF_DbFindCallbackMultiple<EDF_DbEntity> loadHandler(this, "CallbackLoadDataIntoExisting", context);
		persistenceManager
			.GetDbContext()
			.FindAllAsync(RL_StorageBoxSaveData, EDF_DbFind.And({
				EDF_DbFind.Field("m_sOwnerPersistenceID").Equals(playerPersistenceID),
				EDF_DbFind.Field("m_sStorageType").Equals(storageType)
			}), callback: loadHandler);
	}

	//------------------------------------------------------------------------------------------------
	void CallbackLoadDataIntoExisting(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> findResults, Managed context)
	{
		Print("[StorageDebug] CallbackLoadDataIntoExisting");
		Tuple4<int, string, string, Managed> typedContext = Tuple4<int, string, string, Managed>.Cast(context);
		if (!typedContext)
			return;

		int storageRplId = typedContext.param1;
		string playerPersistenceID = typedContext.param2;
		string storageType = typedContext.param3;
		Managed callbackThis = typedContext.param4;
		
		IEntity storageEntity = EPF_NetworkUtils.FindEntityByRplId(storageRplId);
		if(!storageEntity)
		{
			PrintFormat("[StorageDebug] Storage entity not found for RplId %1", storageRplId);
			NotifyLoadComplete(callbackThis, storageRplId, false);
			return;
		}
		RL_StorageBox_Entity storageBoxEntity = RL_StorageBox_Entity.Cast(storageEntity);

		if (!storageBoxEntity)
			return;

		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		if (!persistenceManager || !findResults || findResults.Count() < 1)
		{
			PrintFormat("[StorageDebug] No saved storage data found for %1/%2, using empty storage", playerPersistenceID, storageType);
			if (Replication.IsServer())
			{
				storageBoxEntity.SetStorageLoaded(true);
			}
			NotifyLoadComplete(callbackThis, storageRplId, true);
			return;
		}

		EPF_EntitySaveData saveData = EPF_EntitySaveData.Cast(findResults.Get(findResults.Count() - 1));
		if (!saveData)
		{
			PrintFormat("[StorageDebug] Invalid save data for %1/%2", playerPersistenceID, storageType);
			NotifyLoadComplete(callbackThis, storageRplId, false);
			return;
		}

		EPF_PersistenceComponent persistenceComponent = EPF_PersistenceComponent.Cast(storageBoxEntity.FindComponent(EPF_PersistenceComponent));
		if (!persistenceComponent)
		{
			PrintFormat("[StorageDebug] No persistence component found on storage entity for %1/%2", playerPersistenceID, storageType);
			NotifyLoadComplete(callbackThis, storageRplId, false);
			return;
		}
		Print("[StorageDebug] CallbackLoadDataIntoExisting calling persistenceComponent.Load");
		persistenceComponent.Load(saveData);
		if (Replication.IsServer())
		{
			storageBoxEntity.SetStorageLoaded(true);
		}
		NotifyLoadComplete(callbackThis, storageRplId, true);

		PrintFormat("[StorageDebug] Loaded storage data into existing entity for %1/%2", playerPersistenceID, storageType);
	}

	//------------------------------------------------------------------------------------------------
	private void NotifyLoadComplete(Managed callbackThis, int storageRplId, bool success)
	{
		if (!callbackThis || storageRplId == 0)
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(callbackThis);
		if (character)
		{
			character.OnStorageLoadComplete(storageRplId, success);
		}
	}

	//------------------------------------------------------------------------------------------------
	private RL_StorageBox_Entity FindStorageEntityByPersistenceId(string storagePersistenceID)
	{
		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		if (!persistenceManager)
			return null;
			
		IEntity entity = persistenceManager.FindEntityByPersistentId(storagePersistenceID);
		return RL_StorageBox_Entity.Cast(entity);
	}

	//------------------------------------------------------------------------------------------------
} 