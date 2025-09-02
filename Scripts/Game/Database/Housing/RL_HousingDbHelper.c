class RL_HousingDbHelper : RL_DbHelper
{
	protected ref RL_RestCallback m_callbackSyncHouses;
	protected ref RL_RestCallback m_callbackBuyHouse;
	protected ref RL_RestCallback m_callbackSellHouse;
	protected ref RL_RestCallback m_callbackBuyUpgrade;
	protected ref RL_RestCallback m_callbackCheckOwnership;
	protected ref RL_CallbackAddDynamicUpgrade m_callbackAddDynamicUpgrade;
	protected ref RL_CallbackRemoveDynamicUpgrade m_callbackRemoveDynamicUpgrade;
	
	//------------------------------------------------------------------------------------------------
	void SyncHousesToDatabase(array<ref RL_HouseData> houses, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackSyncHouses) m_callbackSyncHouses = new RL_RestCallback();
		m_callbackSyncHouses.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/sync";
		
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("houses", houses);

        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackSyncHouses, requestURL, jsonCtx.ExportToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void BuyHouse(int cid, string uniqueId, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackBuyHouse) m_callbackBuyHouse = new RL_RestCallback();
		m_callbackBuyHouse.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/buy/" + cid + "/" + uniqueId;

        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackBuyHouse, requestURL, "");
	}
	
	//------------------------------------------------------------------------------------------------
	void SellHouse(int cid, string uniqueId, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackSellHouse) m_callbackSellHouse = new RL_RestCallback();
		m_callbackSellHouse.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/sell/" + cid + "/" + uniqueId;

        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackSellHouse, requestURL, "");
	}
	
	//------------------------------------------------------------------------------------------------
	void BuyHouseUpgrade(int cid, string uniqueId, RL_HouseUpgradeRequestData upgradeData, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackBuyUpgrade) m_callbackBuyUpgrade = new RL_RestCallback();
		m_callbackBuyUpgrade.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/upgrade/" + cid + "/" + uniqueId;
		
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("upgradeType", upgradeData.upgradeType);
		jsonCtx.WriteValue("upgradeName", upgradeData.upgradeName);
		jsonCtx.WriteValue("upgradePrice", upgradeData.upgradePrice);
		jsonCtx.WriteValue("resourceNamePrefab", upgradeData.resourceNamePrefab);

        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackBuyUpgrade, requestURL, jsonCtx.ExportToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void CheckHouseOwnership(int cid, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackCheckOwnership) m_callbackCheckOwnership = new RL_RestCallback();
		m_callbackCheckOwnership.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/check-ownership/" + cid;

        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackCheckOwnership, requestURL);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddDynamicUpgrade(int cid, string upgradeUid, ResourceName prefabPath, vector position, vector angles, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackAddDynamicUpgrade) m_callbackAddDynamicUpgrade = new RL_CallbackAddDynamicUpgrade();
		m_callbackAddDynamicUpgrade.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/dynamic-upgrade/add/" + cid;
		
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("upgradeUid", upgradeUid);
		string fullPrefabPath = prefabPath;
		jsonCtx.WriteValue("prefabPath", fullPrefabPath);
		
		array<float> posArray = {position[0], position[1], position[2]};
		array<float> rotArray = {angles[0], angles[1], angles[2]};
		jsonCtx.WriteValue("position", posArray);
		jsonCtx.WriteValue("rotation", rotArray);

        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddDynamicUpgrade, requestURL, jsonCtx.ExportToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void RemoveDynamicUpgrade(int cid, string upgradeUid, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackRemoveDynamicUpgrade) m_callbackRemoveDynamicUpgrade = new RL_CallbackRemoveDynamicUpgrade();
		m_callbackRemoveDynamicUpgrade.SetCallback(callbackThis, callbackFunction);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "housing/dynamic-upgrade/remove/" + cid + "/" + upgradeUid;
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		string emptyJson = jsonCtx.ExportToString();
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackRemoveDynamicUpgrade, requestURL, emptyJson);
	}
} 