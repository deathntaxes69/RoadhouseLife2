modded class SCR_ChimeraCharacter
{
    protected ref RL_GamblingDbHelper m_gamblingHelper;

    void UpdateSlotsStats(int sessionSpent, int sessionNetProfit, int sessionBiggestWin, int sessionTotalSpins, int sessionTotalWins, int sessionTotalLosses, int sessionTimeSpent, int sessionAverageBet)
    {
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("session_spent", sessionSpent);
        jsonCtx.WriteValue("session_net_profit", sessionNetProfit);
        jsonCtx.WriteValue("session_biggest_win", sessionBiggestWin);
        jsonCtx.WriteValue("session_total_spins", sessionTotalSpins);
        jsonCtx.WriteValue("session_total_wins", sessionTotalWins);
        jsonCtx.WriteValue("session_total_losses", sessionTotalLosses);
        jsonCtx.WriteValue("session_time_spent", sessionTimeSpent);
        jsonCtx.WriteValue("session_average_bet", sessionAverageBet);

        Rpc(RpcAsk_UpdateSlotsStats, GetCharacterId(), jsonCtx.ExportToString());
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_UpdateSlotsStats(string characterId, string sessionStatsJson)
    {
        Print("RpcAsk_UpdateSlotsStats");
        if (!m_gamblingHelper) 
            m_gamblingHelper = new RL_GamblingDbHelper();

        // Parse the JSON to extract individual values
        SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
        if (jsonCtx.ImportFromString(sessionStatsJson))
        {
            int sessionSpent, sessionNetProfit, sessionBiggestWin, sessionTotalSpins;
            int sessionTotalWins, sessionTotalLosses, sessionTimeSpent, sessionAverageBet;
            
            jsonCtx.ReadValue("session_spent", sessionSpent);
            jsonCtx.ReadValue("session_net_profit", sessionNetProfit);
            jsonCtx.ReadValue("session_biggest_win", sessionBiggestWin);
            jsonCtx.ReadValue("session_total_spins", sessionTotalSpins);
            jsonCtx.ReadValue("session_total_wins", sessionTotalWins);
            jsonCtx.ReadValue("session_total_losses", sessionTotalLosses);
            jsonCtx.ReadValue("session_time_spent", sessionTimeSpent);
            jsonCtx.ReadValue("session_average_bet", sessionAverageBet);

            m_gamblingHelper.UpdateSlotsStats(
                characterId, 
                sessionSpent, 
                sessionNetProfit, 
                sessionBiggestWin, 
                sessionTotalSpins, 
                sessionTotalWins, 
                sessionTotalLosses, 
                sessionTimeSpent, 
                sessionAverageBet,
                this,
                "OnSlotsStatsUpdated"
            );
        }
        else
        {
            Print("RL_SlotsCharacter: Failed to parse session stats JSON", LogLevel.ERROR);
        }
    }

    void OnSlotsStatsUpdated(bool success, Managed context = null)
    {
        if (success)
        {
            Print(string.Format("Slots stats updated successfully for character %1", GetCharacterName()));
        }
        else
        {
            Print(string.Format("Failed to update slots stats for character %1", GetCharacterName()), LogLevel.ERROR);
        }
    }
} 