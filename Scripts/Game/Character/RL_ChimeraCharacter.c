modded class SCR_ChimeraCharacter
{
	protected ref RL_CharacterDbHelper m_characterHelper;
	protected ref ScriptInvokerVoid m_OnAccountUpdated;

	[RplProp()]
	protected string m_sIdentityId = "";
	[RplProp()]
	protected string m_sCharacterId = "";
	[RplProp()]
	protected string m_sName = "";
	[RplProp()]
	protected int m_iAge = 0;
	[RplProp()]
	protected int m_iCash = 0;
	[RplProp()]
	protected int m_iBank = 0;
	[RplProp()]
	protected int m_iJailTime = 0;
	[RplProp()]
	protected int m_iPoliceRank = 0;
	[RplProp()]
	protected int m_iPoliceDept = 0;
	[RplProp()]
	protected int m_iMedicRank = 0;
	[RplProp()]
	protected string m_sLicenses = "";
	[RplProp()]
	protected string m_sRecipes = "";
	[RplProp()]
	protected int m_gangId = -1;
	[RplProp()]
	protected int m_gangRank = -1;
	[RplProp()]
	protected int m_aparmentId = 0;
	[RplProp()]
	protected string m_sApartment = "";
	[RplProp()]
	protected bool m_bSeatBelt = false;
	[RplProp()]
	protected int m_iConnectedAt = 0;
	[RplProp()]
	protected int m_iTotalPlayTime = 0;
	[RplProp()]
	protected bool m_bHasWarrant = false;
	[RplProp()]
	protected ref array<string> m_sOwnedPerks;
	[RplProp()]
	protected vector m_vHouseBoundsMin;
	[RplProp()]
	protected vector m_vHouseBoundsMax;
	[RplProp()]
	protected bool m_bHasHouseBounds = false;
	[RplProp()]
	protected string m_sOwnedHouseId = "";
	[RplProp()]
	protected bool m_bCanOpenInventory = true;
	
	protected static const int NewPlayerPlayTime = 172800;

	void InitCharacterServer(RL_CharacterAccount account)
	{

		m_sIdentityId = account.GetIdentityId();
		m_sCharacterId = account.GetCharacterId();
		m_sName = account.GetName();
		m_iAge = account.GetAge();
		m_iJailTime = account.GetJailTime();
		m_iPoliceRank = account.GetPoliceRank();
		m_iPoliceDept = account.GetPoliceDept();
		m_iMedicRank = account.GetMedicRank();
		m_sLicenses = account.GetLicenses();
		m_sRecipes = account.GetRecipes();
		m_gangId = account.GetGangId();
		m_gangRank = account.GetGangRank();
		m_aparmentId = account.GetApartmentLocationId();
		m_sApartment = account.GetApartment();
		m_iConnectedAt = account.GetConnectedAt();
		m_iTotalPlayTime = account.GetPlayTime();
		m_bHasWarrant = account.GetHasWarrant();
		m_sOwnedPerks = account.GetOwnedPerks();
		if (!m_sOwnedPerks)
			m_sOwnedPerks = new array<string>();
		
		if (!HasHouseBounds())
			InitializeHouseData(account.GetOwnedHouseId(), account.GetOwnedHousePrefab());
		
		auto keysComp = RL_VehicleKeysComponent.Cast(this.FindComponent(RL_VehicleKeysComponent));
		if(keysComp)
			keysComp.SetVehicleKeys(account.GetOwnedVehiclesAsStrings(), false);

		Replication.BumpMe();
	}

	void InitCharacterLocal()
	{
		// Make an empty transaction to refresh hud
		TransactMoney();
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		character.InitAnimationKeyBinds();
		m_bSeatBelt = false;
		GetGame().GetCallqueue().CallLater(CreateOwnedHouseMarker, 10000, false);
		RL_MapUIComponent.UnstreamInvsibleMarkers();
	}

	ScriptInvokerVoid GetOnAccountUpdated()
	{
		if (!m_OnAccountUpdated)
			m_OnAccountUpdated = new ScriptInvokerVoid();

		return m_OnAccountUpdated;
	}

	string GetIdentityId()
	{
		return m_sIdentityId;
	}

	string GetCharacterId()
	{
		if (!m_sCharacterId)
			return "-1";

		return m_sCharacterId;
	}

	int GetCash()
	{
		return m_iCash;
	}

	int GetBank()
	{
		return m_iBank;
	}

	string GetCharacterName()
	{
		return m_sName;
	}

	int GetAge()
	{
		return m_iAge;
	}

	int GetJailTime()
	{
		return m_iJailTime;
	}

	int GetPoliceRank()
	{
		return m_iPoliceRank;
	}

	int GetPoliceDept()
	{
		return m_iPoliceDept;
	}
	
	int GetMedicRank()
	{
		return m_iMedicRank;
	}

	bool IsPolice()
	{
		return (m_iPoliceRank > 0);
	}

	bool IsMedic()
	{
		return (m_iMedicRank > 0);
	}

	int GetConnectedForSeconds()
	{
		return (System.GetTickCount() - m_iConnectedAt) / 1000;
	}

	int GetTotalPlayTime()
	{
		return m_iTotalPlayTime + GetConnectedForSeconds();
	}

	bool IsNewPlayer()
	{
		return GetTotalPlayTime() < NewPlayerPlayTime;
	}

	string GetLicenses()
	{
		return m_sLicenses;
	}

	string GetRecipes()
	{
		return m_sRecipes;
	}

	bool HasLicense(string license)
	{
		if (!m_sLicenses || m_sLicenses.IsEmpty())
			return false;

		array<string> licenseArray = {};
		m_sLicenses.Split(",", licenseArray, true);

		foreach (string lic : licenseArray)
		{
			Print("[RL_ChimeraCharacter] HasLicense foreach loop");
			if (lic == license)
				return true;
		}

		return false;
	}

	bool HasRecipe(string recipe)
	{
		if (!m_sRecipes || m_sRecipes.IsEmpty())
			return false;

		array<string> recipeArray = {};
		m_sRecipes.Split(",", recipeArray, true);

		foreach (string rec : recipeArray)
		{
			if (rec == recipe)
				return true;
		}

		return false;
	}

	int GetGangId()
	{
		return m_gangId;
	}

	void SetGangId(int gangId)
	{
		m_gangId = gangId;
		Replication.BumpMe();
	}

	int GetGangRank()
	{
		return m_gangRank;
	}

	void SetGangRank(int gangRank)
	{
		m_gangRank = gangRank;
		Replication.BumpMe();
	}

	int GetApartmentLocationId()
	{
		return m_aparmentId;
	}

	void SetApartmentLocationId(int apartmentId)
	{
		m_aparmentId = apartmentId;
		Replication.BumpMe();
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.SetApartmentId(GetCharacterId(), apartmentId);
	}

	bool HasApartment()
	{
		return !m_sApartment.IsEmpty();
	}

	string GetApartment()
	{
		return m_sApartment;
	}

	void SetApartment(string apartment)
	{
		m_sApartment = apartment;
		Replication.BumpMe();
	}

	bool GetHasWarrant()
	{
		return m_bHasWarrant;
	}

	void SetHasWarrant(bool hasWarrant)
	{
		m_bHasWarrant = hasWarrant;
		Replication.BumpMe();
	}

	bool GetHouseBounds(out vector boundsMin, out vector boundsMax)
	{
		if (!m_bHasHouseBounds)
			return false;
			
		boundsMin = m_vHouseBoundsMin;
		boundsMax = m_vHouseBoundsMax;
		return true;
	}

	void SetHouseBounds(vector boundsMin, vector boundsMax)
	{
		m_vHouseBoundsMin = boundsMin;
		m_vHouseBoundsMax = boundsMax;
		m_bHasHouseBounds = true;
		PrintFormat("[RL_ChimeraCharacter] Set house bounds [%1, %2]", boundsMin, boundsMax);
		Replication.BumpMe();
	}

	void ClearHouseBounds()
	{
		m_vHouseBoundsMin = vector.Zero;
		m_vHouseBoundsMax = vector.Zero;
		m_bHasHouseBounds = false;
		Replication.BumpMe();
	}

	bool HasHouseBounds()
	{
		return m_bHasHouseBounds;
	}

	string GetOwnedHouseId()
	{
		return m_sOwnedHouseId;
	}

	void SetOwnedHouseId(string houseId)
	{
		m_sOwnedHouseId = houseId;
		Replication.BumpMe();
	}

	void ClearOwnedHouseId()
	{
		m_sOwnedHouseId = "";
		Replication.BumpMe();
	}

	bool HasOwnedHouse()
	{
		return !m_sOwnedHouseId.IsEmpty();
	}
	
	void SetCanOpenInventory(bool value)
	{
		m_bCanOpenInventory = value;
		Replication.BumpMe();
	}
	
	bool GetCanOpenInventory()
	{
		return m_bCanOpenInventory;
	}

	//------------------------------------------------------------------------------------------------
	void InitializeHouseData(string houseId, string housePrefab)
	{
		if (!Replication.IsServer())
			return;
		
		SetOwnedHouseId(houseId);
		if (!houseId.IsEmpty())
		{
			vector boundsMin, boundsMax;
			if (RL_HouseManager.GetHouseBoundsByUniqueId(houseId, boundsMin, boundsMax))
				SetHouseBounds(boundsMin, boundsMax);
		}
	}

	void CreateHouseMarker(vector housePosition, string houseId, vector boundsMin, vector boundsMax)
	{
		PrintFormat("[RL_ChimeraCharacter] Creating house marker for house %1", houseId);
		string markerText = "My House";
		string iconName = "HOUSE";
		Color markerColor = Color.FromInt(0xFF00FF00);
		float expireTimeMs = 0;
		RL_MapUtils.CreateMarkerClient(housePosition, markerText, iconName, markerColor, expireTimeMs);
	}

	void TransactMoney(int cashUpdate = 0, int bankUpdate = 0)
	{
		string characterId = GetCharacterId();
		if (characterId == "-1" || characterId == "")
			return;

		Rpc(RpcAsk_TransactMoney, characterId, cashUpdate, bankUpdate);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_TransactMoney(string characterId, int cashUpdate, int bankUpdate)
	{
		PrintFormat("RpcAsk_TransactMoney characterId=%1 cashUpdate=%2 bankUpdate=%3", characterId, cashUpdate, bankUpdate);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, cashUpdate, bankUpdate);
	}

	void TransactMoneyCallback(int cash, int bank)
	{
		m_iCash = cash;
		m_iBank = bank;
		Replication.BumpMe();

		Rpc(RpcDo_TransactMoneyCallback, cash, bank);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_TransactMoneyCallback(int cash, int bank)
	{
		m_iCash = cash;
		m_iBank = bank;
		if (m_OnAccountUpdated)
			m_OnAccountUpdated.Invoke();
	}

	void AdjustJailTimeCallback(int newJailTime)
	{
		m_iJailTime = newJailTime;
		Replication.BumpMe();
		Rpc(RpcDo_AdjustJailTimeCallback, newJailTime);

	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_AdjustJailTimeCallback(int newJailTime)
	{
		m_iJailTime = newJailTime;
		if (m_OnAccountUpdated)
		m_OnAccountUpdated.Invoke();
	}

	void Notify(string content, string title = "", int duration = 5)
	{
		Rpc(RpcDo_Notify, content, title, duration);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Notify(string content, string title, int duration)
	{
		RL_Utils.Notify(content, title, duration);
	}

	void ShowAdminWarning(int duration = 30)
	{
		Rpc(RpcDo_ShowAdminWarning, duration);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ShowAdminWarning(int duration)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;
		hud.ShowAdminWarning();
	}

	void AtmTransferMoney(int targetPlayerId, string targetPlayerName, int amount)
	{
		Rpc(RpcAsk_AtmTransferMoney, GetCharacterId(), targetPlayerId, targetPlayerName, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AtmTransferMoney(string senderCid, int targetPlayerId, string targetPlayerName, int amount)
	{
		if (!Replication.IsServer())
			return;
		Print("RpcAsk_AtmTransferMoney");

		SCR_ChimeraCharacter senderCharacter = RL_Utils.FindCharacterById(senderCid);
		if (!senderCharacter)
			return;

		if (senderCharacter.GetBank() < amount)
		{
			senderCharacter.Notify("Insufficient funds for transfer.", "ATM");
			return;
		}

		IEntity targetEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(targetPlayerId);
		if (!targetEntity)
		{
			senderCharacter.Notify("Target player is not available.", "ATM");
			return;
		}

		SCR_ChimeraCharacter targetCharacter = SCR_ChimeraCharacter.Cast(targetEntity);
		if (!targetCharacter)
		{
			senderCharacter.Notify("Target player is not valid.", "ATM");
			return;
		}

		string targetCid = targetCharacter.GetCharacterId();
		Tuple4<string, string, string, int> context(senderCid, targetCid, targetPlayerName, amount);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(senderCid, 0, -amount, this, "AtmTransferMoneyCallback", context);
	}

	void AtmTransferMoneyCallback(bool success, Managed context)
	{
		Tuple4<string, string, string, int> typedContext = Tuple4<string, string, string, int>.Cast(context);
		SCR_ChimeraCharacter senderCharacter = RL_Utils.FindCharacterById(typedContext.param1);
		SCR_ChimeraCharacter targetCharacter = RL_Utils.FindCharacterById(typedContext.param2);

		if (success)
		{
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
			Tuple4<string, string, string, int> targetContext(typedContext.param1, typedContext.param2, typedContext.param3, typedContext.param4);
			m_characterHelper.TransactMoney(typedContext.param2, 0, typedContext.param4, this, "AtmTransferMoneyTargetCallback", targetContext);
		} else {
			if (senderCharacter)
				senderCharacter.Notify("Transfer failed, insufficient funds.", "ATM");
		}
	}

	void AtmTransferMoneyTargetCallback(bool success, Managed context)
	{
		Tuple4<string, string, string, int> typedContext = Tuple4<string, string, string, int>.Cast(context);
		SCR_ChimeraCharacter senderCharacter = RL_Utils.FindCharacterById(typedContext.param1);
		SCR_ChimeraCharacter targetCharacter = RL_Utils.FindCharacterById(typedContext.param2);

		if (success)
		{
			if (senderCharacter)
				senderCharacter.Notify(string.Format("Successfully transferred %1 to %2.", RL_Utils.FormatMoney(typedContext.param4), typedContext.param3), "ATM");
			if (targetCharacter)
			{
				string senderName = "Unknown";
				if (senderCharacter)
					senderName = senderCharacter.GetCharacterName();
				targetCharacter.Notify(string.Format("You received %1 from %2.", RL_Utils.FormatMoney(typedContext.param4), senderName), "ATM");
			}

			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
			
			SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
			if (senderCharacter)
				jsonData.WriteValue("location", senderCharacter.GetOrigin().ToString());
			jsonData.WriteValue("targetCharacterID", typedContext.param2);
			jsonData.WriteValue("targetPlayerName", typedContext.param3);
			jsonData.WriteValue("amount", typedContext.param4);

			m_characterHelper.PlayerLog(typedContext.param1, "ATM Transfer", jsonData.ExportToString());
		} else {
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
			m_characterHelper.TransactMoney(typedContext.param1, 0, typedContext.param4, null, "", null);
			
			if (senderCharacter)
				senderCharacter.Notify("Transfer failed, target account error. Your money has been refunded.", "ATM");
		}
	}

	void OpenPhone()
	{
		//Why cant we just use "this", idk, it's fucked
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (!RL_Utils.CanPerformAction(character)) return;
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_PhoneMenu);
	}

	// General function for adding experience locally
	protected ref RL_SkillsDbHelper m_experienceHelper;
	void AddXp(int category, int addition)
	{
		Rpc(RpcAsk_AddXp, category, addition);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AddXp(int category, int addition)
	{
		//Print("RpcAsk_AddXp");
		if (!m_experienceHelper)
			m_experienceHelper = new RL_SkillsDbHelper();
		m_experienceHelper.AddXp(this.GetCharacterId(), category, addition);
	}

	// Ask server to add items to player inventory
	Managed m_addAmountCallbackThis;
	string m_addAmountCallbackFunction;

	void AddAmount(ResourceName prefab, int amount, Managed callbackThis = null, string callbackFunction = "", bool dropOverflow = false)
	{
		m_addAmountCallbackThis = callbackThis;
		m_addAmountCallbackFunction = callbackFunction;
		Rpc(RpcAsk_AddAmount, prefab, amount, dropOverflow);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_AddAmount(ResourceName prefab, int amount, bool dropOverflow)
	{
		Print("RpcAsk_AddAmount");
		int amountAdded = RL_InventoryUtils.AddAmount(this, prefab, amount, dropOverflow);
		Rpc(RpcDo_AddAmountCallback, amountAdded);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_AddAmountCallback(int amountAdded)
	{
		Print("RpcDo_AddAmountCallback");
		if (!m_addAmountCallbackThis) return;
		GetGame().GetScriptModule().Call(m_addAmountCallbackThis, m_addAmountCallbackFunction, true, null, amountAdded);
	}

	// Ask server to remove items from player inventory
	// This is stupid to pass two arrays but it works
	Managed m_removeItemsCallbackThis;
	string m_removeItemsCallbackFunction;
	void RemoveItems(array<string> removePrefabs, array<int> removeQuantities, Managed callbackThis = null, string callbackFunction = "", bool includeEquipped = false)
	{
		m_removeItemsCallbackThis = callbackThis;
		m_removeItemsCallbackFunction = callbackFunction;

		Rpc(RpcAsk_RemoveItems, removePrefabs, removeQuantities, includeEquipped);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_RemoveItems(array<string> removePrefabs, array<int> removeQuantities, bool includeEquipped)
	{
		Print("RpcAsk_RemoveItems");
		//Sanity check
		if (removePrefabs.Count() != removeQuantities.Count())
		{
			Rpc(RpcDo_RemoveItemsCallback, false);
			return;
		}

		// Check if they still have all items before removing
		for (int i = 0; i < removePrefabs.Count(); i++)
		{
			Print("[RL_ChimeraCharacter] RpcAsk_RemoveItems for");
			int amountInInventory = RL_InventoryUtils.GetAmount(this, removePrefabs[i], includeEquipped);
			// Check if they have less than called for
			if (amountInInventory < removeQuantities[i])
			{
				Rpc(RpcDo_RemoveItemsCallback, false);
				return;
			}
		}
		for (int i = 0; i < removePrefabs.Count(); i++)
		{
			Print("[RL_ChimeraCharacter] RpcAsk_RemoveItems for 2");
			int amountRemoved = RL_InventoryUtils.RemoveAmount(this, removePrefabs[i], removeQuantities[i], includeEquipped);
			// Check if removed less than called for
			if (amountRemoved < removeQuantities[i])
			{
				Rpc(RpcDo_RemoveItemsCallback, false);
				return;
			}
		}

		Rpc(RpcDo_RemoveItemsCallback, true);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_RemoveItemsCallback(bool success)
	{
		Print("RpcDo_RemoveItemsCallback");
		if (!m_removeItemsCallbackThis) return;
		GetGame().GetScriptModule().Call(m_removeItemsCallbackThis, m_removeItemsCallbackFunction, true, null, success);
	}

	SCR_AudioSource PlayCommonSoundLocally(string soundName, vector mat[4] = { "0 0 0", "0 0 0", "0 0 0" })
	{
		SCR_AudioSourceConfiguration soundConfig = new SCR_AudioSourceConfiguration();
		soundConfig.m_sSoundProject = "{C1DBAA13B8D1068E}Assets/Sounds/commonSounds.acp";
		soundConfig.m_sSoundEventName = soundName;
		soundConfig.m_eFlags = SCR_Enum.RemoveFlag(soundConfig.m_eFlags, EAudioSourceConfigurationFlag.Static);

		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(this, soundConfig);
		if (!audioSource)
			return null;

		if (mat == { "0 0 0", "0 0 0", "0 0 0" })
			RL_Utils.GetLocalCharacter().GetTransform(mat);

		soundManagerEntity.PlayAudioSource(audioSource, mat);
		return audioSource;
	}

	void PlayerLog(string logType, string logData)
	{
		Rpc(RpcAsk_PlayerLog, GetCharacterId(), logType, logData);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void RpcAsk_PlayerLog(string characterId, string logType, string logData)
	{
		Print("RpcAsk_PlayerLog");
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.PlayerLog(characterId, logType, logData);
	}
	
	void BuyLicense(ELicenseType licenseType, int price)
	{
		Rpc(RpcAsk_BuyLicense, licenseType, price);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyLicense(ELicenseType licenseType, int price)
	{
		if (!Replication.IsServer())
			return;

		Print("RpcAsk_BuyLicense");

		if (GetBank() < price)
		{
			Notify("Insufficient bank funds to purchase this license.", "License Purchase");
			return;
		}
		
		string licenseString = licenseType.ToString();
		if (HasLicense(licenseString))
		{
			Notify("You already have this license.", "License Purchase");
			return;
		}
		
		if (!m_characterHelper)
			m_characterHelper = new RL_CharacterDbHelper();
		
		m_characterHelper.AddLicense(GetCharacterId(), licenseString, price, this, "BuyLicenseCallback", null);
	}
	
	void BuyLicenseCallback(bool success, string results)
	{
		
		if (success && !results.IsEmpty())
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			string licenses;
			int bank;
			
			if (loadContext.ReadValue("licenses", licenses) &&
				loadContext.ReadValue("bank", bank))
			{
				m_sLicenses = licenses;
				m_iBank = bank;
				Replication.BumpMe();
			}
		}
		
		Rpc(RpcDo_BuyLicenseCallback, success, results);
	}

	void LearnRecipe(string recipeId)
	{
		Rpc(RpcAsk_LearnRecipe, recipeId);
	}

	void LearnRecipeAndDestroy(string recipeId, int gadgetRplId)
	{
		Rpc(RpcAsk_LearnRecipeAndDestroy, recipeId, gadgetRplId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_LearnRecipe(string recipeId)
	{
		if (!Replication.IsServer())
			return;

		if (recipeId.IsEmpty())
			return;

		if (HasRecipe(recipeId))
		{
			Notify("You already know this recipe.", "CRAFTING");
			return;
		}

		if (!m_characterHelper)
			m_characterHelper = new RL_CharacterDbHelper();

		m_characterHelper.AddRecipe(GetCharacterId(), recipeId, this, "LearnRecipeCallback", null);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_LearnRecipeAndDestroy(string recipeId, int gadgetRplId)
	{
		if (!Replication.IsServer())
			return;

		if (recipeId.IsEmpty())
			return;

		if (HasRecipe(recipeId))
		{
			Notify("You already know this recipe.", "CRAFTING");
			return;
		}

		IEntity gadgetEntity = EPF_NetworkUtils.FindEntityByRplId(gadgetRplId);
		if (gadgetEntity)
		{
			InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(gadgetEntity);
			if (invManager)
			{
				invManager.TryDeleteItem(gadgetEntity);
			}
			else
			{
				SCR_EntityHelper.DeleteEntityAndChildren(gadgetEntity);
			}
		}

		if (!m_characterHelper)
			m_characterHelper = new RL_CharacterDbHelper();

		m_characterHelper.AddRecipe(GetCharacterId(), recipeId, this, "LearnRecipeCallback", null);
	}

	void LearnRecipeCallback(bool success, string results)
	{
		if (success && !results.IsEmpty())
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);

			string recipes;

			if (loadContext.ReadValue("recipes", recipes))
			{
				m_sRecipes = recipes;
				Replication.BumpMe();
			}
		}

		Rpc(RpcDo_LearnRecipeCallback, success, results);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_LearnRecipeCallback(bool success, string results)
	{
		if (success && !results.IsEmpty())
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);

			string recipes;
			if (loadContext.ReadValue("recipes", recipes))
			{
				m_sRecipes = recipes;
				RL_Utils.Notify("Recipe learned!", "CRAFTING");
				if (m_OnAccountUpdated)
					m_OnAccountUpdated.Invoke();
			}
		}
		else
		{
			RL_Utils.Notify("Failed to learn recipe.", "CRAFTING");
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_BuyLicenseCallback(bool success, string results)
	{
		if (success && !results.IsEmpty())
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			string licenses;
			int bank;
			
			if (loadContext.ReadValue("licenses", licenses) &&
				loadContext.ReadValue("bank", bank))
			{
				m_sLicenses = licenses;
				m_iBank = bank;
				RL_Utils.Notify("License purchased successfully!", "License Purchase");
				if (m_OnAccountUpdated)
					m_OnAccountUpdated.Invoke();
			}
			else
			{
				RL_Utils.Notify("Failed to parse license response.", "License Purchase");
			}
		}
		else
		{
			RL_Utils.Notify("Failed to purchase license.", "License Purchase");
		}
	}
	
	void EjectFromVehicle()
	{
		Rpc(RpcDo_EjectFromVehicle);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_EjectFromVehicle()
	{
		IEntity vehicle = this.GetParent();
		if (!vehicle)
			return;

		//SCR_ChimeraCharacter m_character = RL_Utils.GetLocalCharacter();
		SCR_CompartmentAccessComponent access = SCR_CompartmentAccessComponent.Cast(GetCompartmentAccessComponent());
		if (!access)
			return;

		GetCharacterController().StopCharacterGesture();
		
		access.GetOutVehicle(EGetOutType.TELEPORT, -1, true, true);
	}

	void CloseAllMenus()
	{
		Rpc(RpcDo_CloseAllMenus);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_CloseAllMenus()
	{
		auto menuManager = GetGame().GetMenuManager();
		if (menuManager.IsAnyMenuOpen() || menuManager.IsAnyDialogOpen()) {
			menuManager.CloseAllMenus();
		}
	}

	//------------------------------------------------------------------------------------------------
	void CreateOwnedHouseMarker()
	{
		if (Replication.IsServer())
			return;

		if (HasOwnedHouse())
		{
			vector housePosition = ParseHouseIdToPosition(m_sOwnedHouseId);
			if (housePosition != vector.Zero)
			{
				CreateHouseMarker(housePosition, m_sOwnedHouseId, vector.Zero, vector.Zero);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	vector ParseHouseIdToPosition(string houseId)
	{
		if (houseId.IsEmpty())
			return vector.Zero;
			
		array<string> coordinates = {};
		houseId.Split("_", coordinates, true);
		
		if (coordinates.Count() != 3)
			return vector.Zero;
		
		float x = coordinates[0].ToFloat();
		float y = coordinates[1].ToFloat();
		float z = coordinates[2].ToFloat();
		
		return Vector(x, y, z);
	}
	

}
