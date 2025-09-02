modded class SCR_ChimeraCharacter
{ 
    protected ref RL_WarrantsWidget m_warrantsWidget;
    protected ref RL_WarrantsDbHelper m_warrantsHelper;
    
    void GetAllWarrants(RL_WarrantsWidget widget)
    {
        m_warrantsWidget = widget;
        Rpc(RpcAsk_GetAllWarrants);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetAllWarrants()
    {
        Print("RpcAsk_GetAllWarrants");
        if (!m_warrantsHelper) m_warrantsHelper = new RL_WarrantsDbHelper();
        m_warrantsHelper.GetAllWarrants(this, "GetAllWarrantsCallback");
    }
    void GetAllWarrantsCallback(bool success, string results)
    {
        Print("GetAllWarrantsCallback");
		Rpc(RpcDo_GetAllWarrantsCallback, success, results);
 
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_GetAllWarrantsCallback(bool success, string results)
	{

        ref array<ref RL_WarrantData> castedResults;
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		loadContext.ReadValue("data", castedResults);

		Print("RpcDo_GetAllWarrantsCallback");
		Print(castedResults);
		if(m_warrantsWidget)
			m_warrantsWidget.FillWarrantsList(success, castedResults);
	}
    void IssueWarrant(RL_WarrantsWidget widget, int suspectPlayerId, int crimeId)
    {
        m_warrantsWidget = widget;
        Rpc(RpcAsk_IssueWarrant, suspectPlayerId, crimeId, RL_Utils.GetPlayerId());
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_IssueWarrant(int suspectPlayerId, int crimeId, int issuerPlayerId)
    {
        Print("RpcAsk_IssueWarrant");
        IEntity suspectEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(suspectPlayerId);
        SCR_ChimeraCharacter suspectCharacter = SCR_ChimeraCharacter.Cast(suspectEntity);

        IEntity issuerEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(issuerPlayerId);
        SCR_ChimeraCharacter issuerCharacter = SCR_ChimeraCharacter.Cast(issuerEntity);
        if(!suspectCharacter || !issuerCharacter) return;

        if (!m_warrantsHelper) m_warrantsHelper = new RL_WarrantsDbHelper();
        m_warrantsHelper.IssueWarrant(suspectCharacter.GetCharacterId(), crimeId, issuerCharacter.GetCharacterId(), this, "IssueWarrantCallback");
    }
    void IssueWarrantCallback(bool success, string results)
    {
        Print("IssueWarrantCallback");
        
        if (success && !results.IsEmpty())
        {
            SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
            loadContext.ImportFromString(results);
            
            int characterId;
            bool newLocalStatus;
            
            if (loadContext.ReadValue("characterId", characterId) &&
                loadContext.ReadValue("newLocalStatus", newLocalStatus))
            {
                string characterIdStr = characterId.ToString();
                SCR_ChimeraCharacter targetCharacter = RL_Utils.FindCharacterById(characterIdStr);
                if (targetCharacter)
                {
                    targetCharacter.SetHasWarrant(newLocalStatus);
                }
            }
        }
        
		Rpc(RpcDo_IssueWarrantCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_IssueWarrantCallback(bool success, string results)
	{
		if(success)
		{
            RL_Utils.Notify("Warrant Issused", "PHONE");
			if(m_warrantsWidget)
				GetAllWarrants(m_warrantsWidget);
		}
        else
            RL_Utils.Notify("Failed to Issue Warrant", "PHONE");
	}
    void DeleteWarrant(RL_WarrantsWidget widget, int warrantId, int characterId)
    {
        m_warrantsWidget = widget;
        Rpc(RpcAsk_DeleteWarrant, warrantId, RL_Utils.GetPlayerId());
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_DeleteWarrant(int warrantId, int deletePlayerId)
    {
        Print("RpcAsk_DeleteWarrant");
        IEntity deleteEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(deletePlayerId);
        SCR_ChimeraCharacter deleteCharacter = SCR_ChimeraCharacter.Cast(deleteEntity);
        if(!deleteCharacter) return;

        if (!m_warrantsHelper) m_warrantsHelper = new RL_WarrantsDbHelper();
        m_warrantsHelper.DeleteWarrant(deleteCharacter.GetCharacterId(), warrantId, this, "DeleteWarrantCallback");
    }
    void DeleteWarrantCallback(bool success, string results)
    {
        Print("DeleteWarrantCallback");
        
        if (success && !results.IsEmpty())
        {
            SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
            loadContext.ImportFromString(results);
            
            int characterId;
            bool newLocalStatus;
            
            if (loadContext.ReadValue("characterId", characterId) &&
                loadContext.ReadValue("newLocalStatus", newLocalStatus))
            {
                string characterIdStr = characterId.ToString();
                SCR_ChimeraCharacter targetCharacter = RL_Utils.FindCharacterById(characterIdStr);
                if (targetCharacter)
                {
                    targetCharacter.SetHasWarrant(newLocalStatus);
                }
            }
        }
        
		Rpc(RpcDo_DeleteWarrantCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_DeleteWarrantCallback(bool success, string results)
	{
		if(success)
		{
            RL_Utils.Notify("Warrant Deleted", "PHONE");
			if(m_warrantsWidget)
				GetAllWarrants(m_warrantsWidget);
		}
        else
            RL_Utils.Notify("Failed to Delete Warrant", "PHONE");
	}
}   