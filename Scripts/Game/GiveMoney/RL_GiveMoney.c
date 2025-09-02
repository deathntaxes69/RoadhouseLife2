modded class SCR_ChimeraCharacter
{
	protected RL_GiveMoneyUI m_giveMoneyMenu;

	void GiveMoney(string receiverCid)
	{
		Rpc(RpcDo_GiveMoney, receiverCid);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_GiveMoney(string receiverCid)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_GiveMoneyMenu);
		m_giveMoneyMenu = RL_GiveMoneyUI.Cast(menuBase);
		m_giveMoneyMenu.SetReceivingCharacterId(receiverCid);
	}

	void GaveMoneySvr(string senderCid, string receiverCid, int amount)
	{
		Rpc(RpcDo_GaveMoneySvr, senderCid, receiverCid, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void RpcDo_GaveMoneySvr(string senderCid, string receiverCid, int amount)
	{
		Print("RpcDo_GaveMoneySvr");
		Tuple3<string, string, int> context(senderCid, receiverCid, amount);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(senderCid, (-amount), 0, this, "GiveMoneyCallback", context);
	}

	void GiveMoneyCallback(bool success, Managed context)
	{
		Tuple3<string, string, int> typedContext = Tuple3<string, string, int>.Cast(context);
		SCR_ChimeraCharacter m_character = RL_Utils.FindCharacterById(typedContext.param1);
		SCR_ChimeraCharacter m_otherCharacter = RL_Utils.FindCharacterById(typedContext.param2);

		if (success)
		{
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();

			m_otherCharacter.GaveMoney(typedContext.param1, typedContext.param2, typedContext.param3);

			SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
			jsonData.WriteValue("location", m_character.GetOrigin().ToString());
			jsonData.WriteValue("toCharacterID", typedContext.param2);
			jsonData.WriteValue("amount", typedContext.param3);

			m_characterHelper.PlayerLog(typedContext.param1, "Give Money", jsonData.ExportToString());
		} else {
			m_character.Notify("Somethings broken, Contact developers: GMCB", "GIVE MONEY");
		}
	}

	void GaveMoney(string senderCid, string receiverCid, int amount)
	{
		Rpc(RpcDo_GaveMoney, senderCid, receiverCid, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_GaveMoney(string senderCid, string receiverCid, int amount)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter m_character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (m_character.GetCharacterId() != receiverCid) {
			RL_Utils.Notify("Somethings broken, Contact developers: RDGM", "CASH RECEIVED");
			return;
		}

		SCR_ChimeraCharacter m_otherCharacter = RL_Utils.GetCharacterByCharacterIdLocal(senderCid);

		RL_Utils.Notify("Someone has given you $" + amount, "CASH RECEIVED");
		m_character.TransactMoney(amount, 0);
	}

}
