class RL_ApartmentDbHelper : RL_DbHelper
{
	protected ref RL_RestCallback m_callbackGetApartment;
	protected ref RL_RestCallback m_callbackBuyApartment;
	protected ref RL_RestCallback m_callbackSellApartment;
	protected ref RL_RestCallback m_callbackBuyUpgrade;
	protected ref RL_RestCallback m_callbackCheckOwnership;
	
	//------------------------------------------------------------------------------------------------
	void GetApartmentByOwner(int cid, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackGetApartment) m_callbackGetApartment = new RL_RestCallback();
		m_callbackGetApartment.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "apartments/get/" + cid;
		
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackGetApartment, requestURL);
	}
	
	//------------------------------------------------------------------------------------------------
	void BuyApartment(int cid, string apartment, int price, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackBuyApartment) m_callbackBuyApartment = new RL_RestCallback();
		m_callbackBuyApartment.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "apartments/buy/" + cid;
		
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("apartment", apartment);
		jsonCtx.WriteValue("price", price);
		
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackBuyApartment, requestURL, jsonCtx.ExportToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void SellApartment(int cid, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackSellApartment) m_callbackSellApartment = new RL_RestCallback();
		m_callbackSellApartment.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "apartments/sell/" + cid;
		
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackSellApartment, requestURL, "");
	}
	
	//------------------------------------------------------------------------------------------------
	void BuyApartmentUpgrade(int cid, RL_ApartmentUpgradeRequestData upgradeData, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackBuyUpgrade) m_callbackBuyUpgrade = new RL_RestCallback();
		m_callbackBuyUpgrade.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "apartments/upgrade/" + cid;
		
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("upgradeType", upgradeData.upgradeType);
		jsonCtx.WriteValue("upgradeName", upgradeData.upgradeName);
		jsonCtx.WriteValue("upgradePrice", upgradeData.upgradePrice);
		jsonCtx.WriteValue("resourceNamePrefab", upgradeData.resourceNamePrefab);
		
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackBuyUpgrade, requestURL, jsonCtx.ExportToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void CheckApartmentOwnership(int cid, Managed callbackThis, string callbackFunction)
	{
		if (!m_callbackCheckOwnership) m_callbackCheckOwnership = new RL_RestCallback();
		m_callbackCheckOwnership.SetCallback(callbackThis, callbackFunction);
		
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "apartments/check-ownership/" + cid;
		
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackCheckOwnership, requestURL);
	}
} 