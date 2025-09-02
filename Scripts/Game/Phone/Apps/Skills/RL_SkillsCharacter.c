modded class SCR_ChimeraCharacter
{ 
    protected ref RL_SkillsDbHelper m_skillsHelper;
    protected ref Managed m_callbackWidget;
    protected string m_callbackWidgetFunction;
    
    void GetAllSkills(Managed widget, string callbackFunction)
    {
        m_callbackWidget = widget;
        m_callbackWidgetFunction = callbackFunction;
        Rpc(RpcAsk_GetAllSkills, GetCharacterId());
    }
    

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetAllSkills(string characterId)
    {
        Print("RpcAsk_GetAllSkills");
        if (!m_skillsHelper) m_skillsHelper = new RL_SkillsDbHelper();
        m_skillsHelper.GetAllSkills(characterId, this, "GetAllSkillsCallback");
    }
    void GetAllSkillsCallback(bool success, string results)
    {
        //Print("GetAllSkillsCallback");
		Rpc(RpcDo_GetAllSkillsCallback, success, results);
 
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_GetAllSkillsCallback(bool success, string results)
	{
        ref array<ref RL_SkillData> castedResults;
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		loadContext.ReadValue("data", castedResults);

		if(m_callbackWidget && m_callbackWidgetFunction)
            GetGame().GetScriptModule().Call(m_callbackWidget, m_callbackWidgetFunction, true, null, success, castedResults);

	}
    
}   