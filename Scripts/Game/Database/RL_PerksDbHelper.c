class RL_PerksDbHelper : RL_DbHelper
{
	protected ref RL_RestCallback m_callbackGetCharacterPerks;
	protected ref RL_RestCallback m_callbackGetCharacterPerkPoints;
	protected ref RL_RestCallback m_callbackAddCharacterPerk;
	protected ref RL_RestCallback m_callbackResetPerkPoints;
	protected ref RL_RestCallback m_callbackAddPerkPoints;
	protected ref RL_RestCallback m_callbackSpendPerkPoints;
	protected ref RL_RestCallback m_callbackPurchasePerk;

	void GetCharacterPerks(string cid, Managed callbackThis, string callbackFunction, Managed context = null)
	{
		if (!m_callbackGetCharacterPerks) m_callbackGetCharacterPerks = new RL_RestCallback();
		m_callbackGetCharacterPerks.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "getcharacterperks/" + cid;
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackGetCharacterPerks, requestURL);
	}

	void GetCharacterPerkPoints(string cid, Managed callbackThis, string callbackFunction, Managed context = null)
	{
		if (!m_callbackGetCharacterPerkPoints) m_callbackGetCharacterPerkPoints = new RL_RestCallback();
		m_callbackGetCharacterPerkPoints.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "getcharacterperkpoints/" + cid;
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackGetCharacterPerkPoints, requestURL);
	}

	void PurchasePerk(string cid, string perkId, int perkPointCost, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackPurchasePerk) m_callbackPurchasePerk = new RL_RestCallback();
		if (callbackThis)
			m_callbackPurchasePerk.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "purchaseperk/" + cid;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("perkId", perkId);
		jsonCtx.WriteValue("perkPointCost", perkPointCost);
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackPurchasePerk, requestURL, jsonCtx.ExportToString());
	}

	void AddCharacterPerk(string cid, string perkId, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackAddCharacterPerk) m_callbackAddCharacterPerk = new RL_RestCallback();
		if (callbackThis)
			m_callbackAddCharacterPerk.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "addcharacterperk/" + cid;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("perkId", perkId);
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddCharacterPerk, requestURL, jsonCtx.ExportToString());
	}

	void ResetPerkPoints(string cid, int cost, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackResetPerkPoints) m_callbackResetPerkPoints = new RL_RestCallback();
		if (callbackThis)
			m_callbackResetPerkPoints.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "resetperkpoints/" + cid;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("cost", cost);
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackResetPerkPoints, requestURL, jsonCtx.ExportToString());
	}

	void AddPerkPoints(string cid, int points, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackAddPerkPoints) m_callbackAddPerkPoints = new RL_RestCallback();
		if (callbackThis)
			m_callbackAddPerkPoints.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "addperkpoints/" + cid + "/" + points;
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddPerkPoints, requestURL, "");
	}

	void SpendPerkPoints(string cid, int points, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackSpendPerkPoints) m_callbackSpendPerkPoints = new RL_RestCallback();
		if (callbackThis)
			m_callbackSpendPerkPoints.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "spendperkpoints/" + cid + "/" + points;
	
        if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackSpendPerkPoints, requestURL, "");
	}
}