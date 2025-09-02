class RL_KillhouseDbHelper : RL_DbHelper
{
	protected ref RL_CallbackLogKillhouseRun m_callbackLogKillhouseRun;

	void LogKillhouseRun(string cid, string killhouseName, float completionTimeSeconds, int targetsHit, int totalTargets, int hostagesKilled, int totalHostages, int headshots, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
	{
		if (!m_restContext)
			return;

		if (!m_callbackLogKillhouseRun) m_callbackLogKillhouseRun = new RL_CallbackLogKillhouseRun();
		m_callbackLogKillhouseRun.SetCallback(callbackThis, callbackFunction, context);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "logkillhouse/" + cid;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("killhouseName", killhouseName);
		jsonCtx.WriteValue("completionTimeSeconds", completionTimeSeconds);
		jsonCtx.WriteValue("targetsHit", targetsHit);
		jsonCtx.WriteValue("totalTargets", totalTargets);
		jsonCtx.WriteValue("hostagesKilled", hostagesKilled);
		jsonCtx.WriteValue("totalHostages", totalHostages);
		jsonCtx.WriteValue("headshots", headshots);

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackLogKillhouseRun, requestURL, jsonCtx.ExportToString());
	}
}

class RL_CallbackLogKillhouseRun : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		if (m_callbackThis && m_callbackFunction)
		{
			ScriptModule scriptModule = GetGame().GetScriptModule();
			if (m_callbackContext)
				scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, true, m_callbackContext);
			else
				scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, true, null);
		}
	}

	override void OnError(int errorCode)
	{
		if (m_callbackThis && m_callbackFunction)
		{
			ScriptModule scriptModule = GetGame().GetScriptModule();
			if (m_callbackContext)
				scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, false, m_callbackContext);
			else
				scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, false, null);
		}
	}
}


