modded class SCR_ChimeraCharacter
{ 
    protected ref RL_TextWidget m_textWidget;
    protected ref RL_TextsDbHelper m_textHelper;
    
    // START GET LAST TEXTS 
    void GetLastTexts(RL_TextWidget textWidget)
    {
        m_textWidget = textWidget;
        Rpc(RpcAsk_GetLastTexts, GetCharacterId());
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetLastTexts(string characterId)
    {
        Print("RpcAsk_GetLastTexts");
        if (!m_textHelper) m_textHelper = new RL_TextsDbHelper();
        m_textHelper.GetLastTexts(characterId, this, "GetLastTextsCallback");
    }
    void GetLastTextsCallback(bool success, string results)
    {
        Print("GetLastTextsCallback");
		Rpc(RpcDo_GetLastTextsCallback, success, results);
 
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_GetLastTextsCallback(bool success, string results)
	{
        ref array<ref RL_TextData> castedResults;
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		loadContext.ReadValue("data", castedResults);

		Print("RpcDo_GetLastTextsCallback");
		Print(castedResults);
		if(m_textWidget)
			m_textWidget.FillLastTexts(success, castedResults);
	}
    // START SINGLE CONVERSATION
    void GetConversation(RL_TextWidget textWidget, string partyId)
    {
        m_textWidget = textWidget;
        Rpc(RpcAsk_GetConversation, GetCharacterId(), partyId);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetConversation(string characterId, string partyId)
    {
        Print("RpcAsk_GetConversation");
        if (!m_textHelper) m_textHelper = new RL_TextsDbHelper();
        m_textHelper.GetConversation(characterId, partyId, this, "GetConversationCallback");
    }
    void GetConversationCallback(bool success, string results)
    {
        Print("GetConversationCallback");
		Rpc(RpcDo_GetConversationCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_GetConversationCallback(bool success, string results)
	{
        ref array<ref RL_TextData> castedResults;
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		loadContext.ReadValue("data", castedResults);

		if(m_textWidget)
			m_textWidget.FillConversation(success, castedResults);
	}
    // START SEND TEXT
    void SendText(RL_TextWidget textWidget, string receiver, string content)
    {
        m_textWidget = textWidget;
        Rpc(RpcAsk_SendText, GetCharacterId(), receiver, content);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SendText(string sender, string receiver, string content)
    {
        Print("RpcAsk_SendText");
        if (!m_textHelper) m_textHelper = new RL_TextsDbHelper();
        m_textHelper.SendText(sender, receiver, content, this, "SendTextCallback");

        SCR_ChimeraCharacter senderCharacter = RL_Utils.FindCharacterById(sender);
        SCR_ChimeraCharacter receiverCharacter = RL_Utils.FindCharacterById(receiver);
        if(senderCharacter && receiverCharacter)
            receiverCharacter.NotifyText(senderCharacter.GetCharacterName(), content);
    }
    void SendTextCallback(bool success, string results)
    {
        Print("SendTextCallback");
		Rpc(RpcDo_SendTextCallback, success, results);
 
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_SendTextCallback(bool success, string results)
	{
		if(!success)
        {
            RL_Utils.Notify("Something went wrong, failed to send text.", "PHONE");
            return;
        }

        if(m_textWidget)
			m_textWidget.OnSendCallback();

        
	}
    void NotifyText(string senderName, string content)
    {
        Rpc(RpcDo_NotifyText, senderName, content);
 
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_NotifyText(string senderName, string content)
	{
		RL_Utils.Notify(string.Format("%1: %2", senderName, content), "TEXT");
	}
}   