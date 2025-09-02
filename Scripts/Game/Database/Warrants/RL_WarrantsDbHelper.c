class RL_WarrantsDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackGetConversation;
    protected ref RL_RestCallback m_callbackTexts;
    protected ref RL_RestCallback m_callbackWarrant;
    protected ref RL_RestCallback m_callbackDeleteWarrant;

    void GetAllWarrants( Managed callbackThis, string callbackFunction)
    {
		if (!m_callbackTexts) m_callbackTexts = new RL_RestCallback();
        m_callbackTexts.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "getallwarrants";

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackTexts, requestURL);

    }
    void IssueWarrant(string cid, int crimeId, string issuerId, Managed callbackThis = null, string callbackFunction = "")
    {
		if (!m_callbackWarrant) m_callbackWarrant = new RL_RestCallback();
        if(callbackThis)
            m_callbackWarrant.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("issuewarrant/%1/%2/%3", cid, crimeId, issuerId);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackWarrant, requestURL, jsonCtx.ExportToString());

    }
    void DeleteWarrant(string cid, int warrantId, Managed callbackThis = null, string callbackFunction = "")
    {
		if (!m_callbackDeleteWarrant) m_callbackDeleteWarrant = new RL_RestCallback();
        if(callbackThis)
            m_callbackDeleteWarrant.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("deletewarrant/%1/%2", cid, warrantId);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackDeleteWarrant, requestURL, jsonCtx.ExportToString());

    }
   

}