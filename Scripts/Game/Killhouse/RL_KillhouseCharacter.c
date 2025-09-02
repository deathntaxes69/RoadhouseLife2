modded class SCR_ChimeraCharacter
{
	protected ref RL_KillhouseDbHelper m_killhouseHelper;

	void LogKillhouseRun(string killhouseName, float completionTimeSeconds, int targetsHit, int totalTargets, int hostagesKilled, int totalHostages, int headshots)
	{
		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("killhouseName", killhouseName);
		jsonCtx.WriteValue("completionTimeSeconds", completionTimeSeconds);
		jsonCtx.WriteValue("targetsHit", targetsHit);
		jsonCtx.WriteValue("totalTargets", totalTargets);
		jsonCtx.WriteValue("hostagesKilled", hostagesKilled);
		jsonCtx.WriteValue("totalHostages", totalHostages);
		jsonCtx.WriteValue("headshots", headshots);

		Rpc(RpcAsk_LogKillhouseRun, GetCharacterId(), jsonCtx.ExportToString());
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_LogKillhouseRun(string characterId, string runStatsJson)
	{
		Print("RpcAsk_LogKillhouseRun");
		if (!m_killhouseHelper)
			m_killhouseHelper = new RL_KillhouseDbHelper();

		SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
		if (jsonCtx.ImportFromString(runStatsJson))
		{
			string killhouseName; float completionTimeSeconds; int targetsHit; int totalTargets; int hostagesKilled; int totalHostages; int headshots;
			jsonCtx.ReadValue("killhouseName", killhouseName);
			jsonCtx.ReadValue("completionTimeSeconds", completionTimeSeconds);
			jsonCtx.ReadValue("targetsHit", targetsHit);
			jsonCtx.ReadValue("totalTargets", totalTargets);
			jsonCtx.ReadValue("hostagesKilled", hostagesKilled);
			jsonCtx.ReadValue("totalHostages", totalHostages);
			jsonCtx.ReadValue("headshots", headshots);

			m_killhouseHelper.LogKillhouseRun(
				characterId,
				killhouseName,
				completionTimeSeconds,
				targetsHit,
				totalTargets,
				hostagesKilled,
				totalHostages,
				headshots,
				this,
				"OnKillhouseRunLogged"
			);
		}
	}

	void OnKillhouseRunLogged(bool success, Managed context = null)
	{
	}
}


