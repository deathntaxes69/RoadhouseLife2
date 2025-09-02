class RL_DbHelper
{
	protected RestContext m_restContext;
	protected string m_baseHeaders;
	protected ref RL_RestCallback m_callback;

	void RL_DbHelper() {
		if (!Replication.IsServer())
		{
			Print("RL_DbHelper can only be used server side", LogLevel.ERROR);
			return;
		}

		// Corrected path to include the 'Env' folder
		string configPath = "$profile:Env/RL_DbConfig.json";
		SCR_JsonLoadContext reader = new SCR_JsonLoadContext();

		if (!reader.LoadFromFile(configPath))
		{
			Print("FATAL ERROR: Failed to load or parse database config file at " + configPath, LogLevel.ERROR);
			return;
		}

		string databaseUrl, apiKey;
		reader.ReadValue("databaseUrl", databaseUrl);
		reader.ReadValue("apiKey", apiKey);

		if (databaseUrl.IsEmpty() || apiKey.IsEmpty())
		{
			Print("FATAL ERROR: 'databaseUrl' or 'apiKey' not found in database config file at " + configPath, LogLevel.ERROR);
			return;
		}

		m_restContext = GetGame().GetRestApi().GetContext(databaseUrl);
		
		string apikeyHeader = "apikey," + apiKey;
		m_baseHeaders = apikeyHeader;
	}

	bool RL_DBHelperCheckIfNotServer()
	{
		if (!Replication.IsServer())
		{
			Print("RL_DBHelperCheckIfNotServer can only be used server side", LogLevel.ERROR);
			Debug.DumpStack();
			return true;
		}
		return false;
	}
}