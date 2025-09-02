[BaseContainerProps(configRoot: true)]
modded class SCR_ChatPanelManager
{
    // Side chat name tags
    override void OnNewMessageGeneral(string msg, int channelId, int senderId)
	{	
        string roleplayName = "";

        IEntity entity = SCR_PossessingManagerComponent.GetPlayerMainEntity(senderId);
        if(entity) {
            SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
            if(character)
            {
                roleplayName = character.GetCharacterName();
            }
        }
        if(roleplayName == "")
            roleplayName = SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(senderId);
		SCR_ChatMessageGeneral m = new SCR_ChatMessageGeneral(
			msg,
			m_ChatEntity.GetChannel(channelId),
			senderId,
			roleplayName
        );
		
		this.OnNewMessage(m);
	}
}