sealed class RL_WebProxyDbDriverCallback : RestCallback
{
    protected ref EDF_WebProxyDbDriver m_driver;
	protected static ref set<ref RL_WebProxyDbDriverCallback> s_aSelfReferences = new set<ref RL_WebProxyDbDriverCallback>();

	protected ref EDF_DbOperationCallback m_pCallback;
	protected typename m_tResultType;

	protected string m_sVerb;
	protected string m_sUrl;

	//------------------------------------------------------------------------------------------------
	override void OnSuccess(string data, int dataSize)
	{
        // --- DEBUGGING START ---
        Print(string.Format("SUCCESS: Request to URL '%1' with verb '%2' completed. Data size: '%3'", m_sUrl, m_sVerb, dataSize));
        Print(string.Format("SUCCESS DATA: %1", data));
        // --- DEBUGGING END ---

        if(m_driver)
            m_driver.DecreaseQueue();
            
		#ifdef PERSISTENCE_DEBUG
		Print(string.Format("%1::OnSuccess(%2, %3) from %4:%5", this, dataSize, data, m_sVerb, m_sUrl), LogLevel.VERBOSE);
		#endif

		s_aSelfReferences.RemoveItem(this);

		auto statusCallback = EDF_DbOperationStatusOnlyCallback.Cast(m_pCallback);
		if (statusCallback)
		{
			statusCallback.Invoke(EDF_EDbOperationStatusCode.SUCCESS);
			return;
		}

		auto findCallback = EDF_DbFindCallbackBase.Cast(m_pCallback);
		if (!findCallback)
			return; // Could have been a status only operation but no callback was set

		if (dataSize == 0)
		{
			OnFailure(EDF_EDbOperationStatusCode.FAILURE_RESPONSE_MALFORMED);
			return;
		}

		SCR_JsonLoadContext reader();
		array<ref EDF_DbEntity> resultEntities();

		// Read per line individually until json load context has polymorph support: https://feedback.bistudio.com/T173074
		array<string> lines();
		data.Split("\n", lines, true);
		for (int nLine = 1, count = lines.Count() - 1; nLine < count; nLine++)
		{
			//Print("[RL_WebProxyDbDriverCallback] OnSuccess for");
			EDF_DbEntity entity = EDF_DbEntity.Cast(m_tResultType.Spawn());

			if (!reader.ImportFromString(lines[nLine]) || !reader.ReadValue("", entity))
			{
				OnFailure(EDF_EDbOperationStatusCode.FAILURE_RESPONSE_MALFORMED);
				return;
			}

			resultEntities.Insert(entity);
		}

		findCallback.Invoke(EDF_EDbOperationStatusCode.SUCCESS, resultEntities);
	};

	//------------------------------------------------------------------------------------------------
	override void OnError(int errorCode)
	{
        // --- DEBUGGING START ---
        Print(string.Format("ERROR: Request to URL '%1' with verb '%2' failed with code '%3'", m_sUrl, m_sVerb, errorCode), LogLevel.ERROR);
        // --- DEBUGGING END ---

        if(m_driver)
            m_driver.DecreaseQueue();

		s_aSelfReferences.RemoveItem(this);

		#ifdef PERSISTENCE_DEBUG
		Print(string.Format("%1::OnError(%2) from %3:%4", this, typename.EnumToString(ERestResult, errorCode), m_sVerb, m_sUrl), LogLevel.ERROR);
		#endif

		EDF_EDbOperationStatusCode statusCode;
		switch (errorCode)
		{
			default:
			{
				statusCode = EDF_EDbOperationStatusCode.FAILURE_UNKNOWN;
				break;
			}
		}

		OnFailure(statusCode);
	}

	//------------------------------------------------------------------------------------------------
	override void OnTimeout()
	{
        // --- DEBUGGING START ---
        Print(string.Format("TIMEOUT: Request to URL '%1' with verb '%2' timed out.", m_sUrl, m_sVerb), LogLevel.ERROR);
        // --- DEBUGGING END ---

        if(m_driver)
            m_driver.DecreaseQueue();

		s_aSelfReferences.RemoveItem(this);

		#ifdef PERSISTENCE_DEBUG
		Print(string.Format("%1::OnTimeout() from %2:%3", this, m_sVerb, m_sUrl), LogLevel.VERBOSE);
		#endif

		OnFailure(EDF_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnFailure(EDF_EDbOperationStatusCode statusCode)
	{
        if(m_driver)
            m_driver.DecreaseQueue();

		auto statusCallback = EDF_DbOperationStatusOnlyCallback.Cast(m_pCallback);
		if (statusCallback)
		{
			statusCallback.Invoke(statusCode);
			return;
		}

		auto findCallback = EDF_DbFindCallbackBase.Cast(m_pCallback);
		if (findCallback)
			findCallback.Invoke(EDF_EDbOperationStatusCode.FAILURE_UNKNOWN, new array<ref EDF_DbEntity>);
	};

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_aSelfReferences = new set<ref RL_WebProxyDbDriverCallback>();
	}

	//------------------------------------------------------------------------------------------------
	void RL_WebProxyDbDriverCallback(EDF_WebProxyDbDriver driver, EDF_DbOperationCallback callback, typename resultType = typename.Empty, string verb = string.Empty, string url = string.Empty)
	{
        m_driver = driver;
		m_pCallback = callback;
		m_tResultType = resultType;
		m_sVerb = verb;
		m_sUrl = url;
		s_aSelfReferences.Insert(this);
	};
}