
class RL_VehicleRetrievalContext : Managed
{
	RplId entityId;
	string prefabName;
	int garageId;
	string characterId;
	string characterName;
	int retrievalCost;
	int vehiclePrice;
	string vehicleColors;
	
	void RL_VehicleRetrievalContext(RplId id, string prefab, int garage, string charId, string charName, int cost, int price, string colors = "")
	{
		entityId = id;
		prefabName = prefab;
		garageId = garage;
		characterId = charId;
		characterName = charName;
		retrievalCost = cost;
		vehiclePrice = price;
		vehicleColors = colors;
	}
}

modded class SCR_ChimeraCharacter
{
	protected array<IEntity> m_nearbyVehicles;

	protected RL_GarageUI m_garageMenu;

	protected ref RL_GarageDbHelper m_garageHelper;
	protected ref RL_CharacterDbHelper m_characterHelper;
	protected ref array<Managed> m_purchaseContexts = {};
	protected static const int m_iGarageRetrieveCost = 300;
	protected static const int m_iGarageRetrieveImpoundedCost = 1000; // status 2 = impounded
	protected static const float m_fGarageRetrieveChoppedMultiplier = 0.1; // status 3 = chopped

	bool StoreVehicle(IEntity player)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(player);
		if (!character)
			return false;

		Rpc(RpcAsk_StoreVehicle, character.GetCharacterId());
		return true;
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_StoreVehicle(string characterId)
	{
		Print("RpcAsk_StoreVehicle");
		float searchRadius = 10.0;
		bool vehicleStored = false;

		m_nearbyVehicles = RL_Utils.GetNearbyVehicles(this.GetOrigin(), searchRadius);
		if (!m_nearbyVehicles)
		{
			Print("Cancel StoreVehicle. No nearby Vehicles", LogLevel.ERROR);
		}
		foreach (IEntity vehicle : m_nearbyVehicles)
		{
			Print("[RL_VehicleSystem] RpcAsk_StoreVehicle foreach");
			if (!vehicle)
				continue;

			RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
			if (!vehicleManagerComponent)
				continue;

			string ownerId = vehicleManagerComponent.GetOwnerId();
			if (!ownerId || characterId != vehicleManagerComponent.GetOwnerId())
			{
				//Print("Cancel StoreVehicle. Player is not owner", LogLevel.ERROR);
				continue;
			}

			int garageId = vehicleManagerComponent.GetGarageId();
			if (!garageId)
			{
				Print("Cancel StoreVehicle. No garage ID", LogLevel.ERROR);
				continue;
			}

			vehicleManagerComponent.SetLockedState(true);

			DeleteVehicleAndUpdate(vehicle, garageId);
			vehicleStored = true;
			break;
		}
		if (!vehicleStored)
			Rpc(RpcDo_StoreVehicleCallback, vehicleStored);
	}

	protected void DeleteVehicleAndUpdate(IEntity vehicle, int garageId, int retryI = 0)
	{
		Print("[RL_VehicleSystem] DeleteVehicleAndUpdate");
		if (retryI > 10 || !vehicle)
		{
			Rpc(RpcDo_StoreVehicleCallback, false);
			return;
		}

		if (RL_Utils.EjectAllPlayers(vehicle))
		{
			retryI++;
			GetGame().GetCallqueue().CallLater(DeleteVehicleAndUpdate, 300, false, vehicle, garageId, retryI);
			return;
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(vehicle);
		UpdateGarageState(garageId, 0);
		Rpc(RpcDo_StoreVehicleCallback, true);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_StoreVehicleCallback(bool vehicleStored)
	{
		if (vehicleStored)
			RL_Utils.Notify("Succesfully stored vehicle in garage.", "GARAGE");
		else
			RL_Utils.Notify("No owned vehicles nearby to store.", "GARAGE");
	}

	bool RetrieveVehicle(RplId entityRplId, RL_GarageItem garageItem, string characterId, string characterName)
	{
		int vehicleStatus = garageItem.GetStatus();
		int baseCost = GetRetrievalCostForStatus(vehicleStatus, garageItem.GetVehiclePrice());
		if (baseCost > RL_Utils.GetLocalCharacter().GetBank())
		{
			string statusText = "";
			if (vehicleStatus == 2)
				statusText = " (impounded)";
			else if (vehicleStatus == 3)
				statusText = " (chopped)";
				
			RL_Utils.Notify(string.Format("You need at least $%1 in the bank to retrieve this vehicle%2!", baseCost, statusText), "GARAGE");
			return false;
		}

		string vehicleColors = garageItem.GetVehicleColors();
		Rpc(RpcAsk_RetrieveVehicle, entityRplId, garageItem.GetPrefab(), garageItem.GetVehicleId(), characterId, characterName, vehicleStatus, garageItem.GetVehiclePrice(), vehicleColors);
		return true;
	}
	
	protected int GetRetrievalCostForStatus(int status, int vehiclePrice = 0)
	{
		if (status == 2)
			return m_iGarageRetrieveImpoundedCost;
		else if (status == 3)
		{
			if (vehiclePrice > 0)
			{
				return (int)Math.Max(300, Math.Min(30000, Math.Round(vehiclePrice * m_fGarageRetrieveChoppedMultiplier)));
			}
			else
			{
				return 300;
			}
		}
		else
			return m_iGarageRetrieveCost;
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_RetrieveVehicle(RplId entityId, string prefabName, int garageId, string characterId, string characterName, int vehicleStatus, int vehiclePrice, string vehicleColors)
	{
		PrintFormat("RpcAsk_RetrieveVehicle prefabName=%1 characterId=%2", prefabName, characterId);
		int retrievalCost = GetRetrievalCostForStatus(vehicleStatus, vehiclePrice);
		RL_VehicleSystemComponent vehicleSystem = EL_Component<RL_VehicleSystemComponent>.Find(EPF_NetworkUtils.FindEntityByRplId(entityId));
		if(!vehicleSystem || !vehicleSystem.CanSpawn(prefabName))
		{
			Rpc(RpcDo_RetrieveVehicleCallback, false, retrievalCost);
			return;
		}
		RL_VehicleRetrievalContext context = new RL_VehicleRetrievalContext(entityId, prefabName, garageId, characterId, characterName, retrievalCost, vehiclePrice, vehicleColors);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, 0, -retrievalCost, this, "RetrieveVehicleTransactionCallback", context);
	}

	void RetrieveVehicleTransactionCallback(bool success, Managed context)
	{
		Print("RetrieveVehicleTransactionCallback");
		if (!success)
		{
			Rpc(RpcDo_RetrieveVehicleCallback, false, 0);
			return;
		}
		RL_VehicleRetrievalContext typedContext = RL_VehicleRetrievalContext.Cast(context);
		ProcessVehicleRetrieval(typedContext.entityId, typedContext.prefabName, typedContext.garageId, typedContext.characterId, typedContext.characterName, typedContext.retrievalCost, typedContext.vehiclePrice, typedContext.vehicleColors);
	}
	
	protected void ProcessVehicleRetrieval(RplId entityId, string prefabName, int garageId, string characterId, string characterName, int retrievalCost, int vehiclePrice, string vehicleColors = "")
	{
		Print("ProcessVehicleRetrieval");
		RL_VehicleSystemComponent vehicleSystem = EL_Component<RL_VehicleSystemComponent>.Find(EPF_NetworkUtils.FindEntityByRplId(entityId));
		IEntity spawnResult = vehicleSystem.Spawn(prefabName, garageId, characterId, characterName, vehiclePrice);
		if (spawnResult) {
			UpdateGarageState(garageId, 1);

			if (!vehicleColors.IsEmpty())
			{
				ApplyColorsFromJson(spawnResult, vehicleColors);
			}

			SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
			jsonData.WriteValue("location", spawnResult.GetOrigin().ToString());
			jsonData.WriteValue("prefab", prefabName);
			jsonData.WriteValue("retrievalCost", retrievalCost);
			jsonData.WriteValue("vehiclePrice", vehiclePrice);

			m_characterHelper.PlayerLog(characterId, "Retrieved Vehicle", jsonData.ExportToString());
		}

		Rpc(RpcDo_RetrieveVehicleCallback, !!spawnResult, retrievalCost);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_RetrieveVehicleCallback(bool spawnResult, int retrievalCost)
	{
		if (spawnResult)
		{
			RL_Utils.Notify(string.Format("Vehicle retrieved from garage. $%1 was removed from your bank", retrievalCost), "GARAGE");
		} else {
			if (retrievalCost == 0)
			{
				RL_Utils.Notify("You don't have enough money in the bank to retrieve this vehicle.", "GARAGE");
			} else {
				RL_Utils.Notify("Vehicle returned to your garage. The spawn point might be blocked.", "GARAGE");
			}
		}
	}

	bool PurchaseVehicle(RplId entityRplId, string prefabName, int price, string characterId, string characterName)
	{

		Rpc(RpcAsk_PurchaseVehicle, entityRplId, prefabName, price, characterId, characterName);
		return true;
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_PurchaseVehicle(RplId entityRplId, string prefabName, int price, string characterId, string characterName)
	{
		Print("RpcAsk_PurchaseVehicle");
		Tuple5<int, string, string, string, int> context(entityRplId, prefabName, characterId, characterName, price);
		//m_purchaseContexts.Insert(context);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, (-price), 0, this, "PurchaseVehicleCallback", context);
	}

	void PurchaseVehicleCallback(bool success, Managed context)
	{
		Print("PurchaseVehicleCallback");
		Rpc(RpcDo_PurchaseVehicleCallback, success);
		if (!success)
			return;
		Tuple5<int, string, string, string, int> typedContext = Tuple5<int, string, string, string, int>.Cast(context);

		RL_GarageItem vehicle = RL_GarageItem.Create(typedContext.param3, typedContext.param2);
		if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
		m_garageHelper.AddToGarage(this, "AddToGarageCallback", context, vehicle);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_PurchaseVehicleCallback(bool success)
	{
		if (success)
			RL_Utils.Notify("Vehicle successfully purchased.", "SHOP");
		else
			RL_Utils.Notify("You don't have enough cash on hand.", "SHOP");
	}

	void AddToGarageCallback(bool success, int garageId, Managed context)
	{
		if (!success)
			return;
		
		Tuple5<int, string, string, string, int> typedContext = Tuple5<int, string, string, string, int>.Cast(context);
		RL_VehicleSystemComponent vehicleSystem = EL_Component<RL_VehicleSystemComponent>.Find(EPF_NetworkUtils.FindEntityByRplId(typedContext.param1));
		
		SCR_ChimeraCharacter purchaser = RL_Utils.FindCharacterById(typedContext.param3);
		if (purchaser)
		{
			auto keysComp = RL_VehicleKeysComponent.Cast(purchaser.FindComponent(RL_VehicleKeysComponent));
			if(keysComp)
				keysComp.ReceiveVehicleKey(garageId, typedContext.param2);
		}
		
		IEntity spawnResult = vehicleSystem.Spawn(typedContext.param2, garageId, typedContext.param3, typedContext.param4, typedContext.param5);
		if (spawnResult)
		{
			UpdateGarageState(garageId, 1);
		}
		else
		{
			Rpc(RpcDo_AddToGarageCallback, false);
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_AddToGarageCallback(bool success)
	{
		if (!success)
			RL_Utils.Notify("Failed to spawn your new ride. It should be in your garage.", "SHOP");
	}

	protected void UpdateGarageState(int garageId, int active)
	{
		if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
		m_garageHelper.UpdateGarageState(garageId, active);
	}

	void LoadGarage(RL_GarageUI garageMenu, string characterId)
	{
		m_garageMenu = garageMenu;
		Rpc(Rpc_LoadGarage, characterId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_LoadGarage(string characterId)
	{
		Print("Rpc_LoadGarage");
		if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
		m_garageHelper.LoadGarage(this, "LoadGarageCallback", characterId);
	}

	void LoadGarageCallback(bool success, string results)
	{
		Print("LoadShopCallback");
		Rpc(RpcDo_LoadGarageCallback, success, results);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_LoadGarageCallback(bool success, string results)
	{
		Print("RpcDo_LoadShopCallback");
		GetGame().GetScriptModule().Call(m_garageMenu, "OnGarageLoaded", true, null, success, results);
	}
	
	protected void ApplyColorsFromJson(IEntity vehicle, string colorsJson)
	{
		SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
		if (!jsonCtx.ImportFromString(colorsJson))
			return;
			
		array<string> slotNames = {};
		array<int> slotColors = {};
		int bodyColor = -1;
		
		jsonCtx.ReadValue("slotNames", slotNames);
		jsonCtx.ReadValue("slotColors", slotColors);
		jsonCtx.ReadValue("bodyColor", bodyColor);
		
		RL_VehicleModsComponent modsComponent = RL_VehicleModsComponent.Cast(vehicle.FindComponent(RL_VehicleModsComponent));
		if (modsComponent)
			GetGame().GetCallqueue().CallLater(modsComponent.ApplyColorsWithAuthority, 1000, false, slotNames, slotColors, bodyColor);
	}

}
