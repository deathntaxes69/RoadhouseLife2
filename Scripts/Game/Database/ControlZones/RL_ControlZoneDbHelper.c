class RL_ControlZoneDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackGetControlZone;
    protected ref RL_RestCallback m_callbackUpsertControlZone;

    void GetControlZone(string zoneId, Managed callbackTarget, string callbackMethod)
    {
        if (!m_restContext)
        {
            Print("RL_ControlZoneDbHelper: Rest context not initialized", LogLevel.ERROR);
            return;
        }

        if (!m_callbackGetControlZone) m_callbackGetControlZone = new RL_RestCallback();
        m_callbackGetControlZone.SetCallback(callbackTarget, callbackMethod);

        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("controlzone/get/%1", zoneId);
        	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackGetControlZone, requestURL);
    }

    void UpsertControlZone(string zoneId, string zoneName, int gangIdControlling, Managed callbackTarget, string callbackMethod)
    {
        if (!m_restContext)
        {
            Print("RL_ControlZoneDbHelper: Rest context not initialized", LogLevel.ERROR);
            return;
        }

        if (!m_callbackUpsertControlZone) m_callbackUpsertControlZone = new RL_RestCallback();
        m_callbackUpsertControlZone.SetCallback(callbackTarget, callbackMethod);

        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "controlzone/upsert";
        
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("zoneId", zoneId);
        jsonCtx.WriteValue("zoneName", zoneName);
        jsonCtx.WriteValue("gangIdControlling", gangIdControlling);
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackUpsertControlZone, requestURL, jsonCtx.ExportToString());
    }
}
