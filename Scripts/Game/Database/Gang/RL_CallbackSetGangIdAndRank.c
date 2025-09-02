class RL_CallbackSetGangIdAndRank : RL_RestCallback
{
    override void OnSuccess(string data, int dataSize)
    {
        
        bool success = true;
        RL_SetGangIdAndRankData castedResults = new RL_SetGangIdAndRankData();
        castedResults.ExpandFromRAW(data);
        
        string characterId = castedResults.cid.ToString();
        SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(characterId);
        if(character)
        {
            character.SetGangId(castedResults.gangId);
            character.SetGangRank(castedResults.gangRank);
        }

        if(m_callbackThis && m_callbackFunction)
            GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, success, data);
        
    }
}
class RL_SetGangIdAndRankData : JsonApiStruct
{
    int cid = 0;
    int gangId = 0;
    int gangRank = 0;

    void RL_SetGangIdAndRankData()
    {
        RegV("cid");
        RegV("gangId");
        RegV("gangRank");
    }
}
