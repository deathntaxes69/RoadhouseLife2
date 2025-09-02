/*
modded class SCR_FactionManager
{

	override static Faction SGetPlayerFaction(int playerId)
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;

		return factionManager.GetPlayerFaction(playerId);
	}

}
*/