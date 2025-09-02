class RL_LapRecordsDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackSubmitLapTime;

    void SubmitLapTime(string uid, int cid, string trackName, string vehicleName, float lapTime, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackSubmitLapTime) m_callbackSubmitLapTime = new RL_RestCallback();
        if(callbackThis)
           m_callbackSubmitLapTime.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "submitlaptime";
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("uid", uid);
        jsonCtx.WriteValue("cid", cid);
        jsonCtx.WriteValue("track_name", trackName);
        jsonCtx.WriteValue("vehicle_name", vehicleName);
        jsonCtx.WriteValue("lap_time", lapTime);
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackSubmitLapTime, requestURL, jsonCtx.ExportToString());
    }
} 