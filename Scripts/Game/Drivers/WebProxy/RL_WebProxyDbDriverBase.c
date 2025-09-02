modded class EDF_WebProxyDbDriver
{
	int m_currentActiveRequests = 0;
	int m_maxActiveRequests = 50;
	

	override void AddOrUpdateAsync(notnull EDF_DbEntity entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		if (s_bForceBlocking)
		{
			EDF_EDbOperationStatusCode statusCode = AddOrUpdate(entity);
			if (callback)
				callback.Invoke(statusCode);

			return;
		}
		if(m_currentActiveRequests > m_maxActiveRequests)
		{
			Print("AddOrUpdateAsync Max calling later "+m_currentActiveRequests);
			GetGame().GetCallqueue().CallLater(AddOrUpdateAsync, 250, false, entity, callback);
			return;
		}
		m_currentActiveRequests++;

		typename entityType = entity.Type();
		string request = string.Format("%1/%2%3", EDF_DbName.Get(entityType), entity.GetId(), m_sAddtionalParams);
		string data = Serialize(entity);
		m_pContext.PUT(new RL_WebProxyDbDriverCallback(this, callback, verb: "PUT", url: request), request, data);
	}
	void DecreaseQueue()
	{
		m_currentActiveRequests--;
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		if (s_bForceBlocking)
		{
			EDF_EDbOperationStatusCode statusCode = Remove(entityType, entityId);
			if (callback)
				callback.Invoke(statusCode);

			return;
		}
		if(m_currentActiveRequests > m_maxActiveRequests)
		{
			Print("RemoveAsync Max calling later "+m_currentActiveRequests);
			GetGame().GetCallqueue().CallLater(RemoveAsync, 250, false, entityType, entityId, callback);
			return;
		}
		m_currentActiveRequests++;

		string request = string.Format("%1/%2%3", EDF_DbName.Get(entityType), entityId, m_sAddtionalParams);
		m_pContext.DELETE(new RL_WebProxyDbDriverCallback(this, callback, verb: "DELETE", url: request), request, string.Empty);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EDF_DbFindCallbackBase callback = null)
	{

		if (s_bForceBlocking)
		{
			EDF_DbFindResultMultiple<EDF_DbEntity> results = FindAll(entityType, condition, orderBy, limit, offset);
			if (callback)
				callback.Invoke(results.GetStatusCode(), results.GetEntities());

			return;
		}
		if(m_currentActiveRequests > m_maxActiveRequests)
		{
			Print("FindAllAsync Max calling later "+m_currentActiveRequests);
			GetGame().GetCallqueue().CallLater(FindAllAsync, 250, false, entityType, condition, orderBy, limit, offset, callback);
			return;
		}
		m_currentActiveRequests++;

		string request = string.Format("%1%2", EDF_DbName.Get(entityType), m_sAddtionalParams);
		string data = Serialize(new EDF_WebProxyDbDriverFindRequest(condition, orderBy, limit, offset));
		//Print(request);
		//Print(data);
		//System.ExportToClipboard(data);
		m_pContext.POST(new RL_WebProxyDbDriverCallback(this, callback, entityType, verb: "POST", url: request), request, data);
	}

}