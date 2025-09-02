class RL_CollectablesDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackAddCollectable;
    protected ref RL_RestCallback m_callbackCheckCollectable;

    void AddCollectable(string uid, int cid, string collectableId, string collectableName, int collectableValue, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackAddCollectable) m_callbackAddCollectable = new RL_RestCallback();
        if(callbackThis)
           m_callbackAddCollectable.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "addcollectable";
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("uid", uid);
        jsonCtx.WriteValue("cid", cid);
        jsonCtx.WriteValue("collectable_id", collectableId);
        jsonCtx.WriteValue("collectable_name", collectableName);
        jsonCtx.WriteValue("collectable_value", collectableValue);
        
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackAddCollectable, requestURL, jsonCtx.ExportToString());
    }
    
    void CheckCollectableOwnership(int cid, string collectableId, Managed callbackThis, string callbackFunction)
    {
        if (!m_callbackCheckCollectable) m_callbackCheckCollectable = new RL_RestCallback();
        m_callbackCheckCollectable.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("checkcollectable/%1/%2", cid, collectableId);

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackCheckCollectable, requestURL);
    }
}
