class RL_SkillsDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackGetAllSkills;
    protected ref RL_RestCallback m_callbackAddXp;

    void GetAllSkills(string cid, Managed callbackThis, string callbackFunction)
    {
		if (!m_callbackGetAllSkills) m_callbackGetAllSkills = new RL_RestCallback();
        m_callbackGetAllSkills.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("getallskills/%1", cid);

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackGetAllSkills, requestURL);

    }
    void AddXp(string cid, int category, int addition, Managed callbackThis = null, string callbackFunction = "")
    {
		if (!m_callbackAddXp) m_callbackAddXp = new RL_RestCallback();
        if(callbackThis)
            m_callbackAddXp.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("addxp/%1/%2/%3", cid, category, addition);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackAddXp, requestURL, jsonCtx.ExportToString());

    }

}
