modded class SCR_ChimeraCharacter
{ 
    protected Managed m_callbackWidget;
    protected string m_callbackFunction;
    RL_ContactsWidget m_contactsWidget;
    
    void GetPlayerList(Managed callbackWidget, string callbackFunction)
    {
        m_callbackWidget = callbackWidget;
        m_callbackFunction = callbackFunction;
        Rpc(RpcAsk_GetPlayerList, RL_Utils.GetPlayerId());
    }
    protected ref array<int> m_playerList;
    protected ref array<string> m_nameList;
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetPlayerList(int requestorId)
    {
        Print("RpcAsk_GetPlayerList");

        m_playerList = {};
        m_nameList = {};
        array<ref RL_PlayerData> tempSortingList = {};

        array<int> playerList = {};
		GetGame().GetPlayerManager().GetPlayers(playerList);
        foreach (int playerId : playerList)
		{
            //Print("[RL_PhoneCharacter] RpcAsk_GetPlayerList foreach loop");
            string playerName = RL_Utils.GetPlayerName(playerId);
            if(!playerName)
                continue;

            // Dont include the current player
            if(playerId == requestorId)
                continue;
            
            tempSortingList.Insert(RL_PlayerData(playerId, playerName));
        }
        tempSortingList.Sort();
        foreach (auto o: tempSortingList) {
            //Print("[RL_PhoneCharacter] RpcAsk_GetPlayerList foreach loop 2");
            m_playerList.Insert(o.m_iPlayerId);
            m_nameList.Insert(o.m_sName);
        }
        Rpc(RpcDo_GetPlayerListCallback, m_playerList, m_nameList);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_GetPlayerListCallback(array<int> playerList, array<string> nameList)
    {
        if(m_contactsWidget)
            m_contactsWidget.FillContactList(playerList, nameList);
        ScriptModule scriptModule = GetGame().GetScriptModule();
        if(m_callbackWidget && m_callbackFunction)    
            scriptModule.Call(m_callbackWidget, m_callbackFunction, true, null, playerList, nameList);

    }
} 
class RL_PlayerData
{
    int m_iPlayerId;

    [SortAttribute()]
    string m_sName;
    
    void RL_PlayerData(int playerId, string playerName)
    {
        m_iPlayerId = playerId;
        m_sName = playerName;
    }
}