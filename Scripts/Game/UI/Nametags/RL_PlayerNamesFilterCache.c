
modded class SCR_PlayerNamesFilterCache
{
	
	override string GetPlayerDisplayName(int playerId)
	{
		RL_ServerEventManagerComponent eventManager = RL_ServerEventManagerComponent.GetEventManager();
		if(eventManager)
		{
			string roleplayName = eventManager.GetPlayerNameFromCache(playerId);
			if(roleplayName && roleplayName != string.Empty)
				return roleplayName;
		}
		string username = GetNameFromManagers(playerId);
		if (!username.IsEmpty())
		{
			SearchAndUpdateNames(username);
			username = m_mUsernames[username];
		}
		return username;
	}
	
}