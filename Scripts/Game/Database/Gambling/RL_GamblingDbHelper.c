class RL_GamblingDbHelper : RL_DbHelper
{
    protected ref RL_CallbackUpdateSlotsStats m_callbackUpdateSlotsStats;

    void UpdateSlotsStats(string cid, int sessionSpent, int sessionNetProfit, int sessionBiggestWin, int sessionTotalSpins, int sessionTotalWins, int sessionTotalLosses, int sessionTimeSpent, int sessionAverageBet, Managed callbackThis = null, string callbackFunction = "", Managed context = null)
    {
        if (!m_restContext) {
            Print("RL_GamblingDbHelper: Rest context not initialized", LogLevel.ERROR);
            return;
        }

        if (!m_callbackUpdateSlotsStats) m_callbackUpdateSlotsStats = new RL_CallbackUpdateSlotsStats();
        m_callbackUpdateSlotsStats.SetCallback(callbackThis, callbackFunction, context);

        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "updateslotsstats/" + cid;

        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("session_spent", sessionSpent);
        jsonCtx.WriteValue("session_net_profit", sessionNetProfit);
        jsonCtx.WriteValue("session_biggest_win", sessionBiggestWin);
        jsonCtx.WriteValue("session_total_spins", sessionTotalSpins);
        jsonCtx.WriteValue("session_total_wins", sessionTotalWins);
        jsonCtx.WriteValue("session_total_losses", sessionTotalLosses);
        jsonCtx.WriteValue("session_time_spent", sessionTimeSpent);
        jsonCtx.WriteValue("session_average_bet", sessionAverageBet);
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackUpdateSlotsStats, requestURL, jsonCtx.ExportToString());
    }
}

class RL_CallbackUpdateSlotsStats : RL_RestCallback
{
    override void OnSuccess(string data, int dataSize)
    {
        Print("RL_GamblingDbHelper: Slots stats updated successfully");
        
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
        Print(string.Format("RL_GamblingDbHelper: Failed to update slots stats. Error code: %1", errorCode), LogLevel.ERROR);
        
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