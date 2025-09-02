modded class SCR_ChimeraCharacter
{
    protected ref RL_CollectablesDbHelper m_collectablesHelper;
    protected string m_currentCollectableId;
    protected string m_currentCollectableName;
    protected int m_currentCollectableValue;
    protected RL_SkillsCategory m_currentXpCategory;
    protected int m_currentXpReward;
    
    string EncodeString(string input)
    {
        string encoded = "";
        for (int i = 0; i < input.Length(); i++)
        {
            Print("[RL_CollectablesCharacter] EncodeString for");
            int charCode = input.ToAscii(i);
            if (encoded.IsEmpty())
                encoded = charCode.ToString();
            else
                encoded += charCode.ToString();
        }
        return encoded;
    }
    
    void CollectCollectable(string collectableId, string collectableName, int collectableValue, RL_SkillsCategory xpCategory, int xpReward)
    {
        Rpc(RpcAsk_CollectCollectable, collectableId, collectableName, collectableValue, xpCategory, xpReward);
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_CollectCollectable(string collectableId, string collectableName, int collectableValue, RL_SkillsCategory xpCategory, int xpReward)
    {
        Print("RpcAsk_CollectCollectable");
        if (!m_collectablesHelper) m_collectablesHelper = new RL_CollectablesDbHelper();
        
        string encodedId = EncodeString(collectableName);
        
        m_currentCollectableId = encodedId;
        m_currentCollectableName = collectableName;
        m_currentCollectableValue = collectableValue;
        m_currentXpCategory = xpCategory;
        m_currentXpReward = xpReward;
        
        int cid = GetCharacterId().ToInt();
        
        m_collectablesHelper.CheckCollectableOwnership(cid, encodedId, this, "CheckCollectableCallback");
    }
    
    void CheckCollectableCallback(bool success, string results)
    {
        SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
        loadContext.ImportFromString(results);
        
        int collectableOwned = 0;
        loadContext.ReadValue("owned", collectableOwned);
        
        if (success && collectableOwned == 0)
        {
            string uid = GetIdentityId();
            int cid = GetCharacterId().ToInt();
            
            m_collectablesHelper.AddCollectable(uid, cid, m_currentCollectableId, m_currentCollectableName, m_currentCollectableValue, this, "AddCollectableCallback");
        }
        else if (success && collectableOwned == 1)
        {
            Rpc(RpcDo_CollectableAlreadyOwned);
        }
        else
        {
            Rpc(RpcDo_CollectableError, "Failed to check collectable ownership");
        }
    }
    
    void AddCollectableCallback(bool success, string results)
    {
        if (success)
        {
            if (m_currentXpReward > 0)
                AddXp(m_currentXpCategory, m_currentXpReward);
            
            if (m_currentCollectableValue > 0)
                TransactMoney(0, m_currentCollectableValue);
            
            Rpc(RpcDo_CollectableSuccess, m_currentXpReward, m_currentCollectableValue, m_currentXpCategory);
        }
        else
        {
            Rpc(RpcDo_CollectableError, "Failed to add collectable");
        }
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_CollectableSuccess(int xpReward, int moneyReward, RL_SkillsCategory xpCategory)
    {
        string message = "Successfully collected item!";
        
        if (xpReward > 0)
        {
            string categoryName = RL_SkillsUtils.CategoryIdToName(xpCategory);
            message += string.Format("\n+%1 %2 XP", xpReward, categoryName);
        }
        
        if (moneyReward > 0)
            message += string.Format("\n+$%1 added to bank", moneyReward);
            
        RL_Utils.Notify(message, "COLLECTABLES");
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_CollectableAlreadyOwned()
    {
        RL_Utils.Notify("You already own this collectable!", "COLLECTABLES");
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_CollectableError(string errorMessage)
    {
        RL_Utils.Notify("Failed to collect item: " + errorMessage, "COLLECTABLES");
    }
}
