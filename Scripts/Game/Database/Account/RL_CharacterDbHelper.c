class RL_CharacterDbHelper : RL_DbHelper
{
	protected int m_iSuspiciousLimit = 100000;
	protected ref RL_RestCallback m_callbackLoadCharacters;
	protected ref RL_CallbackAddCharacter m_callbackAddCharacter;
	protected ref RL_CallbackTransactMoney m_callbackTransactMoney;
	protected ref RL_CallbackAdjustJailTime m_callbackAddJailTime;
	protected ref RL_CallbackFetchLicenses m_callbackFetchLicenses;
	protected ref RL_RestCallback m_callbackAddLicense;
	protected ref RL_CallbackModifyLicense m_callbackRemoveLicense;
	protected ref RL_RestCallback m_callbackFetchRecipes;
	protected ref RL_RestCallback m_callbackAddRecipe;
	protected ref RL_RestCallback m_callbackRemoveRecipe;
	protected ref RL_RestCallback m_callbackSetAparmentId;

	void LoadCharacters(Managed callbackThis, string callbackFunction, string uid)
	{
		if (!m_callbackLoadCharacters) m_callbackLoadCharacters = new RL_RestCallback();
		m_callbackLoadCharacters.SetCallback(callbackThis, callbackFunction);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "fetchcharacters/" + uid;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackLoadCharacters, requestURL);
	}

	void LoadCharacter(Managed callbackThis, string callbackFunction, string uid, string cid)
	{
		if (!m_callbackLoadCharacters) m_callbackLoadCharacters = new RL_RestCallback();
		m_callbackLoadCharacters.SetCallback(callbackThis, callbackFunction);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "fetchcharacter/" + uid + "/" + cid;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackLoadCharacters, requestURL);
	}


	void AddCharacter(Managed callbackThis, string callbackFunction, RL_CharacterAccount character)
	{
		if (!m_callbackAddCharacter) m_callbackAddCharacter = new RL_CallbackAddCharacter();
		m_callbackAddCharacter.SetCallback(callbackThis, callbackFunction);
		m_callbackAddCharacter.SetCharacter(character);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "insertcharacter";

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("name", character.GetName());
		jsonCtx.WriteValue("age", character.GetAge());
		jsonCtx.WriteValue("uid", character.GetIdentityId());
		jsonCtx.WriteValue("prefab", character.GetPrefab());

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddCharacter, requestURL, jsonCtx.ExportToString());
	}

	void TransactMoney(string cid, int cashUpdate = 0, int bankUpdate = 0, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (cashUpdate > m_iSuspiciousLimit || bankUpdate > m_iSuspiciousLimit)
		{
			Print("TransactMoney Suspicious Transaction");
			Print(string.Format("Character ID: %1 , Cash Update: %2 , Bank Update: %3", cid, cashUpdate, bankUpdate));
			if (context)
				Print(context);
			//todo player_log
		}

		if (cid == "-1" || cid == "")
			return;

		if (!m_callbackTransactMoney) m_callbackTransactMoney = new RL_CallbackTransactMoney();
		m_callbackTransactMoney.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "transactmoney/" + cid;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("cash_update", cashUpdate);
		jsonCtx.WriteValue("bank_update", bankUpdate);
		jsonCtx.WriteValue("location", RL_Utils.FindCharacterById(cid).GetOrigin().ToString());
		jsonCtx.WriteValue("callback", callbackFunction);

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackTransactMoney, requestURL, jsonCtx.ExportToString());
	}

	void AdjustJailTime(string cid, int jailTime = 0, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackAddJailTime) m_callbackAddJailTime = new RL_CallbackAdjustJailTime();
		m_callbackAddJailTime.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "adjustjailtime/" + cid;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("jail_time", jailTime);

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddJailTime, requestURL, jsonCtx.ExportToString());
	}

	void FetchLicenses(Managed callbackThis, string callbackFunction, string characterId, Managed context = null)
	{
		if (!m_callbackFetchLicenses) m_callbackFetchLicenses = new RL_CallbackFetchLicenses();
		m_callbackFetchLicenses.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "fetchlicenses/" + characterId;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackFetchLicenses, requestURL);
	}

	void AddLicense(string characterId, string license, int price, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackAddLicense) m_callbackAddLicense = new RL_RestCallback();
		m_callbackAddLicense.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "addlicense/" + characterId + "/" + license + "/" + price;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddLicense, requestURL, "");
	}

	void RemoveLicense(string characterId, string license, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackRemoveLicense) m_callbackRemoveLicense = new RL_CallbackModifyLicense();
		m_callbackRemoveLicense.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "removelicense/" + characterId + "/" + license;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackRemoveLicense, requestURL, "");
	}

	void FetchRecipes(Managed callbackThis, string callbackFunction, string characterId, Managed context = null)
	{
		if (!m_callbackFetchRecipes) m_callbackFetchRecipes = new RL_RestCallback();
		m_callbackFetchRecipes.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "fetchrecipes/" + characterId;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(m_callbackFetchRecipes, requestURL);
	}

	void AddRecipe(string characterId, string recipe, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackAddRecipe) m_callbackAddRecipe = new RL_RestCallback();
		m_callbackAddRecipe.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "addrecipe/" + characterId + "/" + recipe;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackAddRecipe, requestURL, "");
	}

	void RemoveRecipe(string characterId, string recipe, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_callbackRemoveRecipe) m_callbackRemoveRecipe = new RL_RestCallback();
		m_callbackRemoveRecipe.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "removerecipe/" + characterId + "/" + recipe;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackRemoveRecipe, requestURL, "");
	}

	void PlayerLog(string characterId, string logType, string logData)
	{
		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "playerlog/" + characterId;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("log_type", logType);
		jsonCtx.WriteValue("log_data", logData);

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(RL_RestCallback(), requestURL, jsonCtx.ExportToString());
	}

	void SetApartmentId(string characterId, int apartmentId)
	{
		if (!m_callbackSetAparmentId) m_callbackSetAparmentId = new RL_RestCallback();

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "setapartmentid/" + characterId + "/" + apartmentId;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackSetAparmentId, requestURL, "");
	}

	void UpdatePlayTime(string characterId, int totalPlayTime)
	{
		if (totalPlayTime < 1 || characterId == "")
			return;

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "updateplaytime/" + characterId + "/" + totalPlayTime;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(RL_RestCallback(), requestURL);
	}

	void UpdateLastLogin(string characterId)
	{
		if (characterId == "")
			return;

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "updatelastlogin/" + characterId;

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.GET(RL_RestCallback(), requestURL);
	}

	void LogCrimeHistory(string characterId, string charge, int ticketAmount, int jailTimeAmount, string officerChargingId)
	{
		if (characterId == "" || officerChargingId == "")
			return;

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "logcrimehistory";

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("cid", characterId.ToInt());
		jsonCtx.WriteValue("charge", charge);
		jsonCtx.WriteValue("ticket_amount", ticketAmount);
		jsonCtx.WriteValue("jail_time_amount", jailTimeAmount);
		jsonCtx.WriteValue("officer_charging_cid", officerChargingId.ToInt());

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(RL_RestCallback(), requestURL, jsonCtx.ExportToString());
	}

}
