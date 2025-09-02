[ComponentEditorProps(category: "ReforgerTest/Spawning", description: "")]
class RL_RespawnComponentClass : SCR_RespawnComponentClass
{
}

class RL_RespawnComponent : SCR_RespawnComponent
{
	protected RL_RespawnSystemComponent m_respawnSystem;
	protected ref RL_RespawnUI m_respawnMenu;
	protected ref RL_CharacterDbHelper m_characterHelper;

	protected ref array<ref RL_CharacterAccount> m_characterAccounts = {};
	protected ref RL_CharacterAccount m_selectedAccount;
	protected SCR_ChimeraCharacter m_newCharacter;

	protected ref array<string> m_clientSpawnTitles = {};
	protected ref array<vector> m_clientSpawnPositions = {};
	protected ref array<vector> m_clientSpawnRotations = {};

	// Camera shizz
	protected CameraManager m_cameraManager;
	protected CameraBase m_spawnCamera;
	protected CameraBase m_orginalCamera;
	protected bool m_bEnableCameraMoves;

    protected int m_iTimeBetweenActions = 2500; // in ms
	protected int m_iLastActionTime = -1;
	bool IsSpamming(bool silent = false)
	{
		int currentTime = System.GetTickCount();
		int diff;

		if (m_iLastActionTime == -1)
			diff = m_iTimeBetweenActions + 1; // Ensure first action isn't blocked
		else
			diff = currentTime - m_iLastActionTime;


		if (diff < m_iTimeBetweenActions)
		{
			if (!silent)
				RL_Utils.Notify("Canceled, slow down", "ACTION");
			return true;
		}

		m_iLastActionTime = currentTime;

		return false;
    }

	void NotifySelectCharacter()
	{
		if (!m_PlayerController)
		{
			//Print("RL_RespawnComponent has no player controller");
			return;
		}
		string identityId = EPF_Utils.GetPlayerUID(m_PlayerController.GetPlayerId());
		Rpc(RpcDo_NotifySelectCharacter, identityId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_NotifySelectCharacter(string identityId)
	{
		//Print("RpcDo_NotifySelectCharacter");
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_RespawnMenu);
		m_respawnMenu = RL_RespawnUI.Cast(menuBase);
		m_respawnMenu.OpenSelectCharacterFrame(this, identityId);
		Rpc(RpcAsk_LoadAccounts);
	}

	void NotifyReadyForSpawn(IEntity playerEntity, RL_CharacterAccount selectedAccount, bool wasAccountUsedBefore)
	{
		if (!playerEntity) return;
		m_newCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
		if (!m_newCharacter) return;

		if(wasAccountUsedBefore)
		{
			Print("NotifyReadyForSpawn 2 WasAlreadyUsedToSpawn");
			m_characterHelper.LoadCharacter(this, "CallbackReloadAccount", selectedAccount.GetIdentityId(), selectedAccount.GetCharacterId() );
			return;
		}

		if (!selectedAccount) return;

		RL_RadioUtils.SetRandomFrequency(m_newCharacter);
		
		// Cache Account
		m_newCharacter.InitCharacterServer(selectedAccount);
		// Set Nametag
		CharacterIdentityComponent identityComponent = EL_Component<CharacterIdentityComponent>.Find(m_newCharacter);
		Identity characterIdentity = identityComponent.GetIdentity();
		characterIdentity.SetName(selectedAccount.GetName());
		identityComponent.CommitChanges();


		bool nearDebug = RL_Utils.WithinDistance(m_newCharacter, RL_SpawnPoint.GetDebugSpawnPoint(), 10);

		bool teleportedToJail =  false;
		if (m_newCharacter.GetJailTime() > 0)
		{
			m_newCharacter.TeleportToJail();
			teleportedToJail = true;
		}

		Rpc(RpcDo_NotifySpawn, nearDebug, teleportedToJail);
	}
	void CallbackReloadAccount(bool success, string results)
	{
		if (success)
		{
			m_characterAccounts = CastCharacterAccounts(results);
			if(m_characterAccounts && m_characterAccounts.Count() > 0)
			{
				NotifyReadyForSpawn(m_newCharacter, m_characterAccounts[0], false);
				return;
			}

			return;
		}
		Print("CallbackReloadAccount Kick");
		int playerId = this.GetPlayerController().GetPlayerId();
		ErrorKick(playerId);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_NotifySpawn(bool nearDebug, bool teleportedToJail)
	{
		Print("RL_RespawnComponent RpcDo_NotifySpawn");

		//if(!m_selectedAccount)
		//	return;

		IEntity player = GetGame().GetPlayerController().GetControlledEntity();
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(player);

		// Wait for new entity to be handed over to player and teleported to debug
		if (!character || !EPF_NetworkUtils.IsOwner(character) || character.GetCharacterId().IsEmpty())
		{
			Print("RpcDo_NotifySpawn - No character yet, will try again");
			//TODO REMOVE FROM QUEUE?
			GetGame().GetCallqueue().CallLater(RpcDo_NotifySpawn, 100, false, nearDebug, teleportedToJail);
			return;
		}
		Print("RL_RespawnComponent RpcDo_NotifySpawn 2");
		
		RL_RadioUtils.SetChannelActive(0);
		
		character.InitCharacterLocal();

		if(teleportedToJail)
			return;
		
		InApartmentCheck(character);
		
		// If near debug zone, open spawn menu
		if (nearDebug)
		{
			//Print("WithinDistance Debug");
			// Open spawn menu
			OpenSpawnMenu();
		}
	}

	void OpenSpawnMenu()
	{
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_RespawnMenu);
		m_respawnMenu = RL_RespawnUI.Cast(menuBase);
		Rpc(RpcAsk_RequestSpawnPoints);
	}

	protected void InApartmentCheck(SCR_ChimeraCharacter character)
	{
		IEntity ApartmentsSpawnLocation = GetGame().GetWorld().FindEntityByName("ApartmentsSpawnPoint");
		if (!ApartmentsSpawnLocation)
			return;

		if (vector.Distance(character.GetOrigin(), ApartmentsSpawnLocation.GetOrigin()) < 1000)
		{
			int characterApartmentId = character.GetApartmentLocationId();

			IEntity ApartmentExitLocation = GetGame().GetWorld().FindEntityByName("ApartmentExit" + characterApartmentId);
			if (!ApartmentExitLocation)
				return;

			RL_Utils.TeleportPlayer(character, ApartmentExitLocation.GetOrigin());
		}
	}

	void OnAccountSelected(RL_CharacterAccount account)
	{
		//Print("SCR_RespawnComponent OnAccountSelected");
		m_selectedAccount = account;
		Rpc(RpcAsk_OnAccountSelected, RL_Utils.GetPlayerId(), account.GetCharacterId(), account.GetPrefab());
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_OnAccountSelected(int playerId, string characterId, string characterPrefab)
	{
		Print("[RL_RespawnComponent] RpcAsk_OnAccountSelected");
		//Print(m_respawnSystem);
		RL_CharacterAccount m_selectedAccount;
		foreach (RL_CharacterAccount character : m_characterAccounts)
		{
			Print("[RL_RespawnComponent] RpcAsk_OnAccountSelected foreach");
			if (character.GetCharacterId() != characterId)
				continue;

			m_selectedAccount = character;
			break;
		}
		//Print("Setting selected account");
		//Print(m_selectedAccount);
		if (!m_selectedAccount)
		{
			ErrorKick(playerId);
			return;
		}
		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_RespawnSystemComponent respawnSystem = RL_RespawnSystemComponent.Cast(gameMode.FindComponent(RL_RespawnSystemComponent));
		respawnSystem.OnAccountSelected(playerId, m_selectedAccount);

		m_selectedAccount.SetConnectedAt();
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.UpdateLastLogin(characterId);

		Print(string.Format("NLBACKEND AccountSelected uid=%1, playerId=%2, RplIdentity=%3, characterId=%4, characterName=%5, name=%6",
			m_selectedAccount.GetIdentityId(),
			playerId,
			m_PlayerController.GetRplIdentity().ToString(),
			characterId,
			m_selectedAccount.GetName(),
			GetGame().GetPlayerManager().GetPlayerName(playerId)
		));
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_LoadAccounts()
	{
		Print("RpcAsk_LoadAccounts");
		if (m_PlayerController)
		{
			string identityId = EPF_Utils.GetPlayerUID(m_PlayerController.GetPlayerId());
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
			m_characterHelper.LoadCharacters(this, "OnAccountsLoaded", identityId);

			int playerId = m_PlayerController.GetPlayerId();
			Print(string.Format("NLBACKEND LoadAccounts uid=%1, playerId=%2, RplIdentity=%3, name=%4",
				identityId,
				playerId,
				m_PlayerController.GetRplIdentity().ToString(),
				GetGame().GetPlayerManager().GetPlayerName(playerId)
			));

		}
	}

	void OnAccountsLoaded(bool success, string results)
	{
		Print("--- OnAccountsLoaded ---");
		Print(string.Format("Received data: %1", results));
		if (!success)
		{
			Print("OnAccountsLoaded received success = false", LogLevel.ERROR);
			ErrorKick(this.GetPlayerController().GetPlayerId());
			return;
		}

		m_characterAccounts = CastCharacterAccounts(results);

		if (m_characterAccounts)
		{
			Print(string.Format("Found %1 characters after parsing", m_characterAccounts.Count()));
		}
		else
		{
			Print("m_characterAccounts is null after parsing", LogLevel.ERROR);
		}

		Rpc(RpcDo_OnAccountsLoaded, success, results);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_OnAccountsLoaded(bool success, string results)
	{
		m_characterAccounts = CastCharacterAccounts(results);
		if (m_respawnMenu)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();

			loadContext.ImportFromString(results);
			int acountid = "";
			loadContext.ReadValue("accountid", acountid);

			int whitelist = 0;
			loadContext.ReadValue("whitelist", whitelist);
			
			int linked = 0;
			loadContext.ReadValue("linked", linked);

			if (whitelist == -1) {
				//Print("not whitelisted!!");
				m_respawnMenu.ShowNotWhitelisted(acountid);
				return;
			} else if (linked == -1) {
				//Print("not linked!!");
				m_respawnMenu.ShowNotLinked(acountid);
				return;
			}

			int ban_expire_seconds = -1;
			string ban_note = "";
			loadContext.ReadValue("ban_expire_seconds", ban_expire_seconds);
			loadContext.ReadValue("ban_note", ban_note);

			if (ban_expire_seconds > 0) {
				//Print("Banned, time remaining: " + ban_expire_seconds + "s reason: " + ban_note);
				m_respawnMenu.ShowBanned(ban_expire_seconds, ban_note, acountid);
				return;
			}

			m_respawnMenu.ShowMOTD();

			m_respawnMenu.OnAccountsLoaded(success, m_characterAccounts);
		}
	}

	array<ref RL_CharacterAccount> CastCharacterAccounts(string data)
	{
		ref array<ref RL_CharacterAccount> castedResults;
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		bool success = loadContext.ImportFromString(data);
		Print(string.Format("SCR_JsonLoadContext.ImportFromString success: %1", success));
		if (!success)
		{
			Print("Failed to parse JSON string in CastCharacterAccounts", LogLevel.ERROR);
		}
		loadContext.ReadValue("data.characters", castedResults);

		return castedResults;
	}

	void AddCharacter(string uid, string name, int age, string prefab)
	{
		//Print("AddCharacter");
		Rpc(RpcAsk_AddCharacter, uid, name, age, prefab);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AddCharacter(string uid, string name, int age, string prefab)
	{
		Print("[RL_RespawnComponent] RpcAsk_AddCharacter");
		
		array<string> nameParts = {};
		name.Split(" ", nameParts, true);
		
		if (nameParts.Count() < 2)
		{
			Rpc(RpcDo_CharacterCreationError, "Character name must contain both first and last name");
			return;
		}
		
		if (nameParts[0].Length() < 2 || nameParts[1].Length() < 2)
		{
			Rpc(RpcDo_CharacterCreationError, "First and last names must be at least 2 characters long");
			return;
		}
		if (m_characterAccounts)
		{
			foreach (RL_CharacterAccount existingCharacter : m_characterAccounts)
			{

				Print("[RL_RespawnComponent] RpcAsk_AddCharacter foreach");
				if (existingCharacter && existingCharacter.GetName() == name)
				{
					Rpc(RpcDo_CharacterCreationError, "A character with this name already exists");
					return;
				}
			}
		}
		
		if (age < 18 || age > 80)
		{
			Rpc(RpcDo_CharacterCreationError, "Age must be between 18 and 80");
			return;
		}
		
		if (prefab.IsEmpty())
		{
			Rpc(RpcDo_CharacterCreationError, "Invalid character model selected");
			return;
		}
		
		RL_CharacterAccount characterAccount = RL_CharacterAccount.Create(uid, name, age, prefab);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.AddCharacter(this, "AddCharacterCallback", characterAccount);
	}

	void AddCharacterCallback(RL_CharacterAccount characterAccount, int nameInUse)
	{
		//Print("AddCharacterCallback inuse=" + nameInUse);
		if (nameInUse != 0)
		{
			Rpc(RpcDo_CharacterCreationNameInUse);
			return;
		}

		Rpc(RpcAsk_LoadAccounts);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_CharacterCreationNameInUse()
	{
		if (m_respawnMenu)
		{
			m_respawnMenu.ShowNameInUse();
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_CharacterCreationError(string errorMessage)
	{
		if (m_respawnMenu)
		{
			m_respawnMenu.OnCharacterCreationError(errorMessage);
		}
	}

	void OnSpawnSelected(IEntity player, vector spawnVector, vector spawnVectorRotation)
	{
		Print("SCR_RespawnComponent OnSpawnSelected");
		Print(spawnVectorRotation);
		IEntity localPlayer = GetGame().GetPlayerController().GetControlledEntity();
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(localPlayer);
		if(character)
		{
			Print("SCR_RespawnComponent OnSpawnSelected - Full Heal before spawn teleport");
			character.Heal();
		}
		RL_Utils.TeleportPlayer(localPlayer, spawnVector, spawnVectorRotation);
		// todo change this to actually spawn the player instead of teleport?

	}

	void SetRespawnCamera()
	{
		//Print("SetRespawnCamera");
		if (!m_cameraManager)
			m_cameraManager = GetGame().GetCameraManager();
		if (!m_orginalCamera)
			m_orginalCamera = m_cameraManager.CurrentCamera();

		if (!m_spawnCamera)
		{
			Resource res = Resource.Load("{D6DE32D1C0FCC1C7}Prefabs/Editor/Camera/ManualCameraBase.et");
			if (res)
				m_spawnCamera = CameraBase.Cast(GetGame().SpawnEntityPrefab(res, GetGame().GetWorld()));
		}
		if (m_cameraManager && m_spawnCamera)
		{
			m_cameraManager.SetCamera(m_spawnCamera);
		}
	}

	void RemoveRespawnCamera()
	{
		//Print("RemoveRespawnCamera");
		if (m_cameraManager && m_orginalCamera)
		{
			//Print("SetCamera");
			m_cameraManager.SetCamera(m_orginalCamera);
		}
		if (m_spawnCamera)
		{
			//Print("DeleteEntityAndChildren");
			SCR_EntityHelper.DeleteEntityAndChildren(m_spawnCamera);
		}
	}

	CameraBase GetSpawnCamera()
	{
		if (m_spawnCamera)
			return m_spawnCamera;
		return null;
	}

	void ErrorKick(int playerId)
	{
		GetGame().GetPlayerManager().KickPlayer(playerId, PlayerManagerKickReason.ERROR);
	}

	void ~RL_RespawnComponent()
	{
		RemoveRespawnCamera();
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RequestSpawnPoints()
	{
		Print("[RL_RespawnComponent] RpcAsk_RequestSpawnPoints");
		if (!m_PlayerController) return;
		
		IEntity player = m_PlayerController.GetControlledEntity();
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(player);
		if (!character) return;
		
		array<EPF_SpawnPoint> availableSpawnPoints = RL_SpawnPoint.GetSpawnPointsForPlayer(
			character.GetCharacterId().ToInt(), 
			character.IsPolice(), 
			character.IsMedic()
		);
		
		array<string> titles = {};
		array<vector> positions = {};
		array<vector> rotations = {}; // todo put these into a class/array
		
		foreach (EPF_SpawnPoint spawnPoint : availableSpawnPoints)
		{
			Print("[RL_RespawnComponent] RpcAsk_RequestSpawnPoints foreach");
			RL_SpawnPoint castedSpawn = RL_SpawnPoint.Cast(spawnPoint);
			if (castedSpawn)
			{
				vector position, yawPitchRoll;
				castedSpawn.GetPosYPR(position, yawPitchRoll);

				titles.Insert(castedSpawn.GetTitle());
				positions.Insert(position);
				rotations.Insert(yawPitchRoll);
			}
		}
		
		Rpc(RpcDo_ReceiveSpawnPoints, titles, positions, rotations);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ReceiveSpawnPoints(array<string> titles, array<vector> positions, array<vector> rotations)
	{
		m_clientSpawnTitles = titles;
		m_clientSpawnPositions = positions;
		m_clientSpawnRotations = rotations;
		
		if (m_respawnMenu)
		{
			m_respawnMenu.SetSpawnPoints(m_clientSpawnTitles, m_clientSpawnPositions, m_clientSpawnRotations);
			m_respawnMenu.OpenSelectSpawnFrame(this);
		}
	}



}

modded enum PlayerManagerKickReason {
	ERROR = 33
}