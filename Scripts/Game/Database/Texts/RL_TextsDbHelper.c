class RL_TextsDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackGetConversation;
    protected ref RL_RestCallback m_callbackTexts;

    void GetConversation(string cid, string partyId, Managed callbackThis, string callbackFunction)
    {
		if (!m_callbackTexts) m_callbackTexts = new RL_RestCallback();
        m_callbackTexts.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "getconversation/" + cid + "/" + partyId;

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackTexts, requestURL);

    }
    void GetLastTexts(string cid, Managed callbackThis, string callbackFunction)
    {
		if (!m_callbackTexts) m_callbackTexts = new RL_RestCallback();
        m_callbackTexts.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "getlasttexts/" + cid;

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackTexts, requestURL);

    }
    void SendText(string cid, string recipient, string content, Managed callbackThis, string callbackFunction)
    {
		if (!m_callbackTexts) m_callbackTexts = new RL_RestCallback();
        m_callbackTexts.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "sendtext/" + cid + "/" + recipient;
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("content", content);
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackTexts, requestURL, jsonCtx.ExportToString());

    }

}
