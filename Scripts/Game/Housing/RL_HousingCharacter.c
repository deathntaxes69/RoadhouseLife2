modded class SCR_ChimeraCharacter
{
	protected ref RL_HousingDbHelper m_housingHelper;
	protected string m_pendingHouseId;
	
	//------------------------------------------------------------------------------------------------
	void BuyHouse(string uniqueId)
	{
		m_pendingHouseId = uniqueId;
		CheckHouseOwnership(this, "OnOwnershipCheckForPurchase");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnOwnershipCheckForPurchase(bool success, string results)
	{
		if (!success)
		{
			Notify("Unable to verify house ownership. Purchase cancelled.", "HOUSING");
			m_pendingHouseId = "";
			return;
		}
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		
		bool ownsHouse = false;
		loadContext.ReadValue("ownsHouse", ownsHouse);
		
		if (!RL_Utils.IsDevServer() && ownsHouse)
		{
			Notify("You already own a house. You can only own one house at a time.", "HOUSING");
			m_pendingHouseId = "";
			return;
		}
		
		Rpc(RpcAsk_BuyHouse, m_pendingHouseId);
		m_pendingHouseId = "";
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyHouse(string uniqueId)
	{
		Print("RpcAsk_BuyHouse");
		if (!m_housingHelper) m_housingHelper = new RL_HousingDbHelper();
		int cid = GetCharacterId().ToInt();
		m_housingHelper.BuyHouse(cid, uniqueId, this, "BuyHouseCallback");
	}
	
	void BuyHouseCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			string uniqueId;
			loadContext.ReadValue("uniqueId", uniqueId);
			
			RL_HouseManager houseManager = RL_HouseManager.GetInstance();
			if (houseManager)
			{
				map<string, RL_HouseComponent> houseComponents = houseManager.GetHouseComponents();
				if (houseComponents)
				{
					RL_HouseComponent houseComponent = houseComponents.Get(uniqueId);
					if (houseComponent)
					{
						int characterId = GetCharacterId().ToInt();
						houseComponent.SetOwnerCid(characterId);
						
						IEntity houseEntity = houseComponent.GetOwner();
						if (houseEntity)
						{
							vector boundsMin, boundsMax;
							if (houseComponent.GetHouseBounds(boundsMin, boundsMax))
							{
								ClearHouseBounds();
								ClearOwnedHouseId();
								SetHouseBounds(boundsMin, boundsMax);
								SetOwnedHouseId(uniqueId);
								Rpc(RpcDo_CreateSingleHouseMarker, houseEntity.GetOrigin(), uniqueId, boundsMin, boundsMax);
							}
						}
					}
				}
			}
		}
		
		Rpc(RpcDo_BuyHouseCallback, success, results);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_CreateSingleHouseMarker(vector housePosition, string houseId, vector boundsMin, vector boundsMax)
	{
		CreateHouseMarker(housePosition, houseId, boundsMin, boundsMax);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_BuyHouseCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			int newBank;
			string uniqueId;
			int price;
			
			loadContext.ReadValue("newBank", newBank);
			loadContext.ReadValue("uniqueId", uniqueId);
			loadContext.ReadValue("price", price);
			
			string message = string.Format("House purchased for %1!\nNew bank balance: %2", 
				RL_Utils.FormatMoney(price), RL_Utils.FormatMoney(newBank));
			Notify(message, "HOUSING");
		}
		else
		{
			string errorMsg = "Failed to purchase house";
			if (!results.IsEmpty())
			{
				SCR_JsonLoadContext errorContext = new SCR_JsonLoadContext();
				if (errorContext.ImportFromString(results))
				{
					string error;
					if (errorContext.ReadValue("error", error) && !error.IsEmpty())
						errorMsg = error;
				}
			}
			Notify(errorMsg, "HOUSING");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SellHouse(string uniqueId)
	{
		Rpc(RpcAsk_SellHouse, uniqueId);
	}
	
	//------------------------------------------------------------------------------------------------
	void BuyHouseUpgrade(string uniqueId, int upgradeType, string upgradeName, int upgradePrice, string resourceNamePrefab)
	{
		Rpc(RpcAsk_BuyHouseUpgrade, uniqueId, upgradeType, upgradeName, upgradePrice, resourceNamePrefab);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SellHouse(string uniqueId)
	{
		Print("RpcAsk_SellHouse");
		if (!m_housingHelper) m_housingHelper = new RL_HousingDbHelper();
		int cid = GetCharacterId().ToInt();
		m_housingHelper.SellHouse(cid, uniqueId, this, "SellHouseCallback");
	}
	
	void SellHouseCallback(bool success, string results)
	{
		if (success)
		{
			ClearHouseBounds();
			ClearOwnedHouseId();
		}
		
		Rpc(RpcDo_SellHouseCallback, success, results);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_SellHouseCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			int newBank;
			string uniqueId;
			int price;
			
			loadContext.ReadValue("newBank", newBank);
			loadContext.ReadValue("uniqueId", uniqueId);
			loadContext.ReadValue("price", price);
			
			ClearHouseBounds();
			ClearOwnedHouseId();
			
			string message = string.Format("House sold for %1!\nNew bank balance: %2", 
				RL_Utils.FormatMoney(price), RL_Utils.FormatMoney(newBank));
			Notify(message, "HOUSING");
		}
		else
		{
			string errorMsg = "Failed to sell house";
			if (!results.IsEmpty())
			{
				SCR_JsonLoadContext errorContext = new SCR_JsonLoadContext();
				if (errorContext.ImportFromString(results))
				{
					string error;
					if (errorContext.ReadValue("error", error) && !error.IsEmpty())
						errorMsg = error;
				}
			}
			Notify(errorMsg, "HOUSING");
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyHouseUpgrade(string uniqueId, int upgradeType, string upgradeName, int upgradePrice, string resourceNamePrefab)
	{
		Print("RpcAsk_BuyHouseUpgrade");
		if (!m_housingHelper) m_housingHelper = new RL_HousingDbHelper();
		int cid = GetCharacterId().ToInt();
		
		ref RL_HouseUpgradeRequestData upgradeData = new RL_HouseUpgradeRequestData();
		upgradeData.upgradeType = upgradeType;
		upgradeData.upgradeName = upgradeName;
		upgradeData.upgradePrice = upgradePrice;
		upgradeData.resourceNamePrefab = resourceNamePrefab;
		
		m_housingHelper.BuyHouseUpgrade(cid, uniqueId, upgradeData, this, "BuyHouseUpgradeCallback");
	}
	
	void BuyHouseUpgradeCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			string uniqueId;
			int upgradeType;
			string upgradeName;
			string resourceNamePrefab;
			
			loadContext.ReadValue("uniqueId", uniqueId);
			loadContext.ReadValue("upgradeType", upgradeType);
			loadContext.ReadValue("upgradeName", upgradeName);
			loadContext.ReadValue("resourceNamePrefab", resourceNamePrefab);
			
			RL_HouseManager houseManager = RL_HouseManager.GetInstance();
			if (houseManager)
			{
				map<string, RL_HouseComponent> houseComponents = houseManager.GetHouseComponents();
				if (houseComponents)
				{
					RL_HouseComponent houseComponent = houseComponents.Get(uniqueId);
					if (houseComponent)
					{
						RL_BaseUpgradeData upgrade = new RL_BaseUpgradeData();
						upgrade.SetData(upgradeType, upgradeName, resourceNamePrefab, "");
						houseComponent.AddUpgrade(upgrade);
					}
				}
			}
		}
		
		Rpc(RpcDo_BuyHouseUpgradeCallback, success, results);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_BuyHouseUpgradeCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			int newBank;
			string uniqueId;
			string upgradeName;
			int price;
			int upgradeType;
			string resourceNamePrefab;
			
			loadContext.ReadValue("newBank", newBank);
			loadContext.ReadValue("uniqueId", uniqueId);
			loadContext.ReadValue("upgradeName", upgradeName);
			loadContext.ReadValue("price", price);
			loadContext.ReadValue("upgradeType", upgradeType);
			loadContext.ReadValue("resourceNamePrefab", resourceNamePrefab);
			
			string message = string.Format("Upgrade '%1' purchased for %2!\nNew bank balance: %3", 
				upgradeName, RL_Utils.FormatMoney(price), RL_Utils.FormatMoney(newBank));
			Notify(message, "HOUSING");
		}
		else
		{
			string errorMsg = "Failed to purchase upgrade";
			if (!results.IsEmpty())
			{
				SCR_JsonLoadContext errorContext = new SCR_JsonLoadContext();
				if (errorContext.ImportFromString(results))
				{
					string error;
					if (errorContext.ReadValue("error", error) && !error.IsEmpty())
						errorMsg = error;
				}
			}
			Notify(errorMsg, "HOUSING");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void CheckHouseOwnership(Managed callbackThis, string callbackFunction)
	{
		if (!m_housingHelper) m_housingHelper = new RL_HousingDbHelper();
		int cid = GetCharacterId().ToInt();
		m_housingHelper.CheckHouseOwnership(cid, callbackThis, callbackFunction);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddDynamicUpgrade(string upgradeUid, ResourceName prefabPath, vector position, vector angles)
	{
		Rpc(RpcAsk_AddDynamicUpgrade, upgradeUid, prefabPath, position, angles);
	}
	
	//------------------------------------------------------------------------------------------------
	void RemoveDynamicUpgrade(string upgradeUid)
	{
		Rpc(RpcAsk_RemoveDynamicUpgrade, upgradeUid);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AddDynamicUpgrade(string upgradeUid, ResourceName prefabPath, vector position, vector angles)
	{
		Print("RpcAsk_AddDynamicUpgrade");
		if (!m_housingHelper) m_housingHelper = new RL_HousingDbHelper();
		int cid = GetCharacterId().ToInt();
		m_housingHelper.AddDynamicUpgrade(cid, upgradeUid, prefabPath, position, angles, this, "AddDynamicUpgradeCallback");
	}
	
	void AddDynamicUpgradeCallback(bool success, string results)
	{
		if (!success)
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return;
			
		string ownedHouseId = character.GetOwnedHouseId();
		if (ownedHouseId.IsEmpty())
			return;
		
		RL_HouseManager houseManager = RL_HouseManager.GetInstance();
		if (!houseManager)
			return;
		
		map<string, RL_HouseComponent> houseComponents = houseManager.GetHouseComponents();
		RL_HouseComponent houseComponent = houseComponents.Get(ownedHouseId);
		
		if (houseComponent)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			if (loadContext.ImportFromString(results))
			{
				string upgradeUid, prefabPath, placedAtString;
				vector position, rotation;
				if (loadContext.ReadValue("upgradeUid", upgradeUid) &&
					loadContext.ReadValue("prefabPath", prefabPath) &&
					loadContext.ReadValue("position", position) &&
					loadContext.ReadValue("rotation", rotation) &&
					loadContext.ReadValue("placedAt", placedAtString))
				{
					RL_DynamicUpgradeData dynamicUpgrade = new RL_DynamicUpgradeData();
					dynamicUpgrade.SetData(upgradeUid, prefabPath, position, rotation, placedAtString);
					houseComponent.AddDynamicUpgrade(dynamicUpgrade);
				}
			}
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RemoveDynamicUpgrade(string upgradeUid)
	{
		Print("RpcAsk_RemoveDynamicUpgrade");
		if (!m_housingHelper) m_housingHelper = new RL_HousingDbHelper();
		int cid = GetCharacterId().ToInt();
		m_housingHelper.RemoveDynamicUpgrade(cid, upgradeUid, this, "RemoveDynamicUpgradeCallback");
	}
	
	void RemoveDynamicUpgradeCallback(bool success, string results)
	{
		if (!success)
			return;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return;
			
		string ownedHouseId = character.GetOwnedHouseId();
		if (ownedHouseId.IsEmpty())
			return;
		
		RL_HouseManager houseManager = RL_HouseManager.GetInstance();
		if (!houseManager)
			return;
		
		map<string, RL_HouseComponent> houseComponents = houseManager.GetHouseComponents();
		RL_HouseComponent houseComponent = houseComponents.Get(ownedHouseId);
		
		if (houseComponent)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			if (loadContext.ImportFromString(results))
			{
				string removedUpgradeUid;
				if (loadContext.ReadValue("upgradeUid", removedUpgradeUid))
				{
					houseComponent.RemoveDynamicUpgrade(removedUpgradeUid);
				}
			}
		}
	}
} 