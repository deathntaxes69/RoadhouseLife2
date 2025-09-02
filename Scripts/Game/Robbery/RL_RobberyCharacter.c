modded class SCR_ChimeraCharacter
{
    void SetRobberyState(RplId targetRplId, bool state)
    {
        Rpc(RpcAsk_SetRobberyState, targetRplId, state);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SetRobberyState(RplId targetRplId, bool state)
	{
		Print("RpcAsk_SetRobberyState");
        IEntity targetEntity = EPF_NetworkUtils.FindEntityByRplId(targetRplId);
        if(!targetEntity) return;
        RL_RobberyManagerComponent robberyManager = RL_RobberyManagerComponent.Cast(targetEntity.FindComponent(RL_RobberyManagerComponent));
        if(!robberyManager) return;
        robberyManager.SetRobberyState(state);

    }
	
	void RobCashSvr(string robberCid, string victimCid, int amount)
	{
		Rpc(RpcDo_RobCashSvr, robberCid, victimCid, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void RpcDo_RobCashSvr(string robberCid, string victimCid, int amount)
	{
		Print("RpcDo_RobCashSvr");
		Tuple3<string, string, int> context(robberCid, victimCid, amount);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(victimCid, (-amount), 0, this, "RobCashCallback", context);
	}

	void RobCashCallback(bool success, Managed context)
	{
		Tuple3<string, string, int> typedContext = Tuple3<string, string, int>.Cast(context);
		SCR_ChimeraCharacter m_robber = RL_Utils.FindCharacterById(typedContext.param1);
		SCR_ChimeraCharacter m_victim = RL_Utils.FindCharacterById(typedContext.param2);

		if (success)
		{
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();

			m_robber.WasRobbed(typedContext.param1, typedContext.param2, typedContext.param3);
			m_victim.WasRobbedVictim(typedContext.param1, typedContext.param2, typedContext.param3);

			SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
			jsonData.WriteValue("location", m_robber.GetOrigin().ToString());
			jsonData.WriteValue("victimCharacterID", typedContext.param2);
			jsonData.WriteValue("amount", typedContext.param3);

			m_characterHelper.PlayerLog(typedContext.param1, "Rob Cash", jsonData.ExportToString());
		} else {
			m_robber.Notify("Somethings broken, Contact developers: RCCB", "ROBBERY");
		}
	}

	void WasRobbed(string robberCid, string victimCid, int amount)
	{
		Rpc(RpcDo_WasRobbed, robberCid, victimCid, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_WasRobbed(string robberCid, string victimCid, int amount)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter m_character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (m_character.GetCharacterId() != robberCid) {
			RL_Utils.Notify("Somethings broken, Contact developers: RDWR", "ROBBERY");
			return;
		}

		RL_Utils.Notify(string.Format("You robbed %1", RL_Utils.FormatMoney(amount)), "ROBBERY");
		m_character.TransactMoney(amount, 0);
	}

	void WasRobbedVictim(string robberCid, string victimCid, int amount)
	{
		Rpc(RpcDo_WasRobbedVictim, robberCid, victimCid, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_WasRobbedVictim(string robberCid, string victimCid, int amount)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter m_character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (m_character.GetCharacterId() != victimCid) {
			RL_Utils.Notify("Somethings broken, Contact developers: RDWRV", "ROBBERY");
			return;
		}

		RL_Utils.Notify(string.Format("You were robbed of %1", RL_Utils.FormatMoney(amount)), "ROBBED");
	}
}