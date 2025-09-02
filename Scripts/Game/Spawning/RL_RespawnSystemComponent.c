[ComponentEditorProps(category: "ReforgerTest/Spawning", description: "")]
class RL_RespawnSystemComponentClass : EPF_BaseRespawnSystemComponentClass
{
}

class RL_RespawnSystemComponent : EPF_BaseRespawnSystemComponent
{
	[Attribute(category: "New character defaults")]
	protected ref array<ResourceName> m_aDefaultCharacterPrefabs;

	[Attribute(category: "New character defaults")]
	protected ref array<ref EL_DefaultLoadoutItem> m_aDefaultCharacterItems;

	protected ref map<string, ref RL_CharacterAccount> m_mAccounts;

	protected ref map<int, ref RL_CharacterAccount> m_selectedCharacters = new map<int, ref RL_CharacterAccount>();
	protected ref map<int, bool> m_selectedCharactersUsedTracker = new map<int, bool>();
	protected ResourceName m_sCharacterPrefab;

	protected int m_serverStartTime;
	protected ref RL_CharacterDbHelper m_characterHelper;

	RL_ApartmentManagerComponent m_apartmentManager;

	override void OnPlayerDisconnected_S(int playerId, KickCauseCode cause, int timeout)
	{
		RL_ServerEventManagerComponent eventManager = RL_ServerEventManagerComponent.GetEventManager();
		if(eventManager)
			eventManager.ClearPlayerNameFromCache(playerId);

		// Delete character save if disconnected while waiting for CPR
		IEntity entity = m_pPlayerManager.GetPlayerController(playerId).GetControlledEntity();
		if (entity)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
			int connectedTime = character.GetConnectedForSeconds();
			int totalPlayTime = character.GetTotalPlayTime();
			string characterId = character.GetCharacterId();


			Print(string.Format("Player %1 disconnected after being connected for %2 seconds, total play time %3 seconds", character.GetCharacterName(), connectedTime, totalPlayTime));
			Print(cause);
			
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
			m_characterHelper.UpdatePlayTime(character.GetCharacterId(), totalPlayTime);

			RL_LapTimerManager lapManager = RL_LapTimerManager.GetInstance();
			lapManager.RemoveCharacterTimer(characterId);

			BaseGameMode gameMode = GetGame().GetGameMode();

			m_apartmentManager =  RL_ApartmentManagerComponent.Cast(gameMode.FindComponent(RL_ApartmentManagerComponent));
			m_apartmentManager.RemoveApartment(playerId, characterId);

			if (EntityUtils.IsPlayerVehicle(entity.GetParent())) {
				CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
				if (compartmentAccess)
				{
					BaseCompartmentSlot compartment = compartmentAccess.GetCompartment();
					if (compartment && compartment.IsPiloting())
					{
						//Print("++++++++++++ LOGGED OUT WHILE DRIVING !!!");
						CarControllerComponent m_Vehicle_c = CarControllerComponent.Cast(entity.GetParent().FindComponent(CarControllerComponent));
						if (m_Vehicle_c)
							m_Vehicle_c.SetPersistentHandBrake(true);
					}
				}
			}

			SCR_CharacterDamageManagerComponent dmc = EL_Component<SCR_CharacterDamageManagerComponent>.Find(entity);
			if (dmc && dmc.IsWaitingForCpr())
			{
				Print("------ Combat log, deleting player");
				SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
				jsonData.WriteValue("location", character.GetOrigin().ToString());
				jsonData.WriteValue("characterName", character.GetCharacterName());
				jsonData.WriteValue("death_id", dmc.GetDeathId());

				m_characterHelper.PlayerLog(characterId, "Combat Log", jsonData.ExportToString());

				SCR_EntityHelper.DeleteEntityAndChildren(entity);
				return;
			}
		}
		super.OnPlayerDisconnected_S(playerId, cause, timeout);
	}
	// Temp hack until this bug is fixed https://github.com/Arkensor/EnfusionPersistenceFramework/issues/49
	override void OnPlayerRegistered_S(int playerId)
	{
		PrintFormat("--------- EPF_BaseRespawnSystemComponent.OnPlayerRegistered_S(%1)", playerId);
		if(!m_missionHeader)
			m_missionHeader = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
		
		if (RplSession.Mode() != RplMode.Dedicated || !IsDedicatedServer())
		{
			Print("--------- Workbench bypass");
			WaitForUid(playerId);
		}
		else
		{
			Print("--------- NO Workbench bypass NO");
			EDF_ScriptInvokerCallback1<int> callback(this, "WaitForUid");
			m_pGameMode.GetOnPlayerAuditSuccess().Insert(callback.Invoke);
		}
	}

	override void OnInit(IEntity owner)
	{
		m_serverStartTime = System.GetUnixTime();
		Print("RL_RespawnSystemComponent OnInit");
		super.OnInit(owner);
	}

	void OnAccountSelected(int playerId, RL_CharacterAccount character)
	{
		Print("RL_RespawnSystemComponent OnAccountSelected");
		m_selectedCharacters.Set(playerId, character);
		m_selectedCharactersUsedTracker.Set(playerId, false);
		//m_sCharacterPrefab = characterPrefab;
		Tuple2<int, string> context(playerId, character.GetCharacterId());
		super.HandlePlayerLoad(context);
	}

	RL_CharacterAccount GetSelectedAccount(int playerId)
	{
		return m_selectedCharacters.Get(playerId);
	}
	override protected void HandoverToPlayer(int playerId, IEntity character)
	{
		HandoverToPlayerWithAttempts(playerId, character, 1);
	}
	protected void HandoverToPlayerWithAttempts(int playerId, IEntity character, int attempt)
	{
		PrintFormat("HandoverToPlayer(%1, %2) - Attempt %3", playerId, character, attempt);
		SCR_PlayerController playerController = SCR_PlayerController.Cast(m_pPlayerManager.GetPlayerController(playerId));
		if(!playerController || !playerController.m_OnControlledEntityChanged)
		{
			Print("No player controller or m_OnControlledEntityChanged", LogLevel.ERROR);
			Print(playerController);
			
			if(attempt < 30)
			{
				GetGame().GetCallqueue().CallLater(CallAgainHandoverToPlayer, 1000, false, playerId, character, attempt + 1);
			}
			else
			{
				GetGame().GetPlayerManager().KickPlayer(playerId, PlayerManagerKickReason.ERROR);
				PrintFormat("HandoverToPlayer failed after %1 attempts for player %2", attempt, playerId, LogLevel.ERROR);
			}
			return;
		}
		EDF_ScriptInvokerCallback2<IEntity, IEntity> callback(this, "OnHandoverComplete", new Tuple1<int>(playerId));
		playerController.m_OnControlledEntityChanged.Insert(callback.Invoke);

		playerController.SetInitialMainEntity(character);

		m_pGameMode.OnPlayerEntityChanged_S(playerId, null, character);

		SCR_RespawnComponent respawn = SCR_RespawnComponent.Cast(playerController.GetRespawnComponent());
		respawn.NotifySpawn(character);
	}

	void CallAgainHandoverToPlayer(int playerId, IEntity character, int attempt)
	{
		HandoverToPlayerWithAttempts(playerId, character, attempt);
	}
	override void OnHandoverComplete(IEntity from, IEntity to, Managed context)
	{
		Tuple1<int> typedContext = Tuple1<int>.Cast(context);
		PrintFormat("OnHandoverComplete Player ID %1", typedContext.param1);

		super.OnHandoverComplete(from, to, context);

		RL_RespawnComponent respawnComponent = RL_RespawnComponent.Cast(SCR_RespawnComponent.SGetPlayerRespawnComponent(typedContext.param1));

		respawnComponent.NotifyReadyForSpawn(to, m_selectedCharacters.Get(typedContext.param1), m_selectedCharactersUsedTracker.Get(typedContext.param1));
		m_selectedCharactersUsedTracker.Set(typedContext.param1, true);
	}

	override void HandlePlayerLoad(Managed context)
	{
		Print("HandlePlayerLoad");
		Tuple2<int, string> characterContext = Tuple2<int, string>.Cast(context);

		RL_RespawnComponent respawnComponent = RL_RespawnComponent.Cast(SCR_RespawnComponent.SGetPlayerRespawnComponent(characterContext.param1));
		if (!respawnComponent)
		{
			Print("No respawnComponent");
			return;
		}
		m_selectedCharacters.Set(characterContext.param1, null);
		respawnComponent.NotifySelectCharacter();

	}

	override protected void LoadCharacter(int playerId, string characterPersistenceId, EPF_CharacterSaveData saveData)
	{
		// If 30 minutes since last save, set debug spawn
		//if (RL_Utils.LastSavedGreaterThan(saveData, 30))
		// If last save was before restart
		
		if (m_serverStartTime > saveData.m_iLastSaved && !NoRespawnOnRestart())
		{
			Print("LoadCharacter Respawning after restart");
			vector position, yawPitchRoll;
			GetCreationPosition(playerId, characterPersistenceId, position, yawPitchRoll);
			saveData.m_pTransformation.m_vOrigin = position;
			saveData.m_pTransformation.m_vAngles = yawPitchRoll;
		} else {
			Print("LoadCharacter No respawn after restart");
		}

		super.LoadCharacter(playerId, characterPersistenceId, saveData);

	}

	override protected void GetCreationPosition(int playerId, string characterPersistenceId, out vector position, out vector yawPitchRoll)
	{
		// Set spawn to debug, HandoverToPlayer will trigger spawn menu
		EPF_SpawnPoint spawnPoint = RL_SpawnPoint.GetDebugSpawnPoint();
		if (!spawnPoint)
		{
			Print("Could not spawn character, no debug spawn point on the map.", LogLevel.ERROR);
			return;
		}

		spawnPoint.GetPosYPR(position, yawPitchRoll);
	}

	override protected ResourceName GetCreationPrefab(int playerId, string characterPersistenceId)
	{
		if (!m_selectedCharacters.Contains(playerId) || !m_selectedCharacters.Get(playerId))
			return "{CE23D4366B47E9B9}Prefabs/Characters/Presets/White_Male_01.et";

		return m_selectedCharacters.Get(playerId).GetPrefab();
	}

	override protected void OnCharacterCreated(int playerId, string characterPersistenceId, IEntity character)
	{
		Print("Respawn OnCharacterCreated");
		InventoryStorageManagerComponent storageManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		foreach (EL_DefaultLoadoutItem loadoutItem : m_aDefaultCharacterItems)
		{
			Print("[RL_RespawnSystemComponent] RpcAsk_RequestSpawnPoints foreach");
			if (loadoutItem.m_ePurpose != EStoragePurpose.PURPOSE_LOADOUT_PROXY)
			{
				Debug.Error(string.Format("Failed to add '%1' as default character item. Only clothing/backpack/vest etc. with purpose '%2' are allowed as top level entries.", loadoutItem.m_rPrefab, typename.EnumToString(EStoragePurpose, EStoragePurpose.PURPOSE_LOADOUT_PROXY)));
				continue;
			}

			IEntity slotEntity = SpawnDefaultCharacterItem(storageManager, loadoutItem);
			if (!slotEntity)
				continue;

			if (!storageManager.TryInsertItem(slotEntity, loadoutItem.m_ePurpose))
				SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);
		}
	}

	// Apply default loadout to character
	protected IEntity SpawnDefaultCharacterItem(InventoryStorageManagerComponent storageManager, EL_DefaultLoadoutItem loadoutItem)
	{
		EntitySpawnParams spawnParams();
		spawnParams.Transform[3] = storageManager.GetOwner().GetOrigin();
		IEntity slotEntity = GetGame().SpawnEntityPrefabEx(loadoutItem.m_rPrefab, false, null, spawnParams);
		if (!slotEntity) return null;

		if (loadoutItem.m_aComponentDefaults)
		{
			foreach (EL_DefaultLoadoutItemComponent componentDefault : loadoutItem.m_aComponentDefaults)
			{
				Print("[RL_RespawnSystemComponent] SpawnDefaultCharacterItem foreach");
				componentDefault.ApplyTo(slotEntity);
			}
		}

		if (loadoutItem.m_aStoredItems)
		{
			array<Managed> outComponents();
			slotEntity.FindComponents(BaseInventoryStorageComponent, outComponents);

			foreach (EL_DefaultLoadoutItem storedItem : loadoutItem.m_aStoredItems)
			{
				Print("[RL_RespawnSystemComponent] SpawnDefaultCharacterItem foreach 2");
				for (int i = 0; i < storedItem.m_iAmount; i++)
				{
					Print("[RL_RespawnSystemComponent] SpawnDefaultCharacterItem for");
					IEntity spawnedItem = SpawnDefaultCharacterItem(storageManager, storedItem);

					foreach (Managed componentRef : outComponents)
					{
						Print("[RL_RespawnSystemComponent] SpawnDefaultCharacterItem foreach 3");
						BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(componentRef);
						if (storageComponent.GetPurpose() & storedItem.m_ePurpose)
						{
							if (!storageManager.TryInsertItemInStorage(spawnedItem, storageComponent)) continue;

							InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(spawnedItem.FindComponent(InventoryItemComponent));
							if (inventoryItemComponent && !inventoryItemComponent.GetParentSlot()) continue;

							break;
						}
					}
				}
			}
		}
		return slotEntity;
	}
	SCR_MissionHeader m_missionHeader;
	bool IsDedicatedServer()
	{
		if(!m_missionHeader)
			m_missionHeader = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());

		if(!m_missionHeader) return false;
		Print("IsDedicatedServer");
		Print(m_missionHeader);

		return m_missionHeader.IsDedicatedServer();
	}
	bool NoRespawnOnRestart()
	{
		if(!m_missionHeader)
			m_missionHeader = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());

		if(!m_missionHeader) return false;

		return m_missionHeader.NoRespawnOnRestart();
	}

}