modded class SCR_PlayerEditableEntityUIComponent
{
	//Gamemaster name tags
	override protected void SetName(TextWidget nameWidget, int playerID)
	{
		
		string playerName = SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(playerID);
		if (playerName.IsEmpty())
			return;

		nameWidget.SetText(playerName);
		GetGame().GetCallqueue().Remove(SetName);
		IEntity entity = SCR_PossessingManagerComponent.GetPlayerMainEntity(playerID);
		if(!entity) return;
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if(!character) return;
		string shortIdentityId = RL_Utils.ShortenIdentityId(character.GetIdentityId());
		nameWidget.SetText(string.Format("%1 - %2 - %3 - %4", character.GetCharacterName(), character.GetCharacterId(), shortIdentityId, playerName ));
	}
}