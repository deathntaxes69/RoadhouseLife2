class RL_GangDbHelper : RL_DbHelper
{
    protected ref RL_CallbackSetGangIdAndRank m_callbackGetGangIdAndRank;
    protected ref RL_RestCallback m_callbackGetGangRank;
    protected ref RL_RestCallback m_callbackCreateGang;
    protected ref RL_RestCallback m_callbackSetGangOwner;
    protected ref RL_RestCallback m_callbackSetGangName;
    protected ref RL_RestCallback m_callbackSetGangFreq;
    protected ref RL_RestCallback m_callbackAddGangExp;
    protected ref RL_RestCallback m_callbackGetGang;
    protected ref RL_RestCallback m_callbackGangTransaction;
    protected ref RL_RestCallback m_callbackGetGangBalance;

    void SetGangIdAndRank(string cid, int gangId, int gangRank, Managed callbackThis = null, string callbackFunction = "")
    {
		if (!m_callbackGetGangIdAndRank) m_callbackGetGangIdAndRank = new RL_CallbackSetGangIdAndRank();
        if(callbackThis)
           m_callbackGetGangIdAndRank.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("setgangidandrank/%1/%2/%3", cid, gangId, gangRank);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackGetGangIdAndRank, requestURL, jsonCtx.ExportToString());
    }
    void CreateGang(string ownerUid, string name, Managed callbackThis, string callbackFunction)
    {
        if (!m_callbackCreateGang) m_callbackCreateGang = new RL_RestCallback();
        m_callbackCreateGang.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "creategang";
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("owner_uid", ownerUid);
        jsonCtx.WriteValue("name", name);
        
        string jsonData = jsonCtx.ExportToString();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackCreateGang, requestURL, jsonData);
    }
    
    void SetGangOwner(string gangId, string ownerUid, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackSetGangOwner) m_callbackSetGangOwner = new RL_RestCallback();
        if(callbackThis)
           m_callbackSetGangOwner.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("setgangowner/%1/%2", gangId, ownerUid);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackSetGangOwner, requestURL, jsonCtx.ExportToString());
    }
    
    void SetGangName(string gangId, string name, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackSetGangName) m_callbackSetGangName = new RL_RestCallback();
        if(callbackThis)
           m_callbackSetGangName.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("setgangname/%1/%2", gangId, name);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackSetGangName, requestURL, jsonCtx.ExportToString());
    }
    
    void SetGangFreq(string gangId, int freq, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackSetGangFreq) m_callbackSetGangFreq = new RL_RestCallback();
        if(callbackThis)
           m_callbackSetGangFreq.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("setgangfreq/%1/%2", gangId, freq);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackSetGangFreq, requestURL, jsonCtx.ExportToString());
    }
    
    void AddGangExp(string gangId, int addition, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackAddGangExp) m_callbackAddGangExp = new RL_RestCallback();
        if(callbackThis)
           m_callbackAddGangExp.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("addgangexp/%1/%2", gangId, addition);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackAddGangExp, requestURL, jsonCtx.ExportToString());
    }
    
    void GetGang(int gangId, Managed callbackThis, string callbackFunction)
    {
        if (!m_callbackGetGang) m_callbackGetGang = new RL_RestCallback();
        m_callbackGetGang.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("getgang/%1", gangId);
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackGetGang, requestURL);
    }

    void GetGangBalance(int gangId, Managed callbackThis, string callbackFunction)
    {
        if (!m_callbackGetGangBalance) m_callbackGetGangBalance = new RL_RestCallback();
        m_callbackGetGangBalance.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("getgangbalance/%1", gangId);

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackGetGangBalance, requestURL);
    }

    void GangTransaction(string cid, string gangId, int cash, int bank, int gangBankUpdate, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackGangTransaction) m_callbackGangTransaction = new RL_RestCallback();
        if(callbackThis)
           m_callbackGangTransaction.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = string.Format("gangtransaction/%1/%2", cid, gangId);
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("cash", cash);
        jsonCtx.WriteValue("bank", bank);
        jsonCtx.WriteValue("gang_bank_update", gangBankUpdate);
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackGangTransaction, requestURL, jsonCtx.ExportToString());
    }
}