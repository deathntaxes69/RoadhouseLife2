modded class SCR_ChimeraCharacter
{
	protected ref RL_CharacterDbHelper m_characterHelper;
	void SpinSlotsSvr(string playerCid, int betAmount)
	{
		Rpc(RpcDo_SpinSlotsSvr, playerCid, betAmount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void RpcDo_SpinSlotsSvr(string playerCid, int betAmount)
	{
		Print("RpcDo_SpinSlotsSvr");
		Tuple2<string, int> context(playerCid, betAmount);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(playerCid, (-betAmount), 0, this, "SpinSlotsCallback", context);
	}

	void SpinSlotsCallback(bool success, Managed context)
	{
		Tuple2<string, int> typedContext = Tuple2<string, int>.Cast(context);
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(typedContext.param1);

		if (success)
		{
			character.SpinValidated(typedContext.param1, typedContext.param2);
		} else {
			character.SpinValidationFailed(typedContext.param1, "Not enough cash. Need $" + typedContext.param2.ToString());
		}
	}

	void SpinValidated(string playerCid, int betAmount)
	{
		Rpc(RpcDo_SpinValidated, playerCid, betAmount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_SpinValidated(string playerCid, int betAmount)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (character.GetCharacterId() != playerCid)
			return;

		MenuBase currentMenu = GetGame().GetMenuManager().GetTopMenu();
		RL_SlotsUI slotsUI = RL_SlotsUI.Cast(currentMenu);
		if (slotsUI)
		{
			slotsUI.OnSpinValidated(betAmount);
		}
	}

	void SpinValidationFailed(string playerCid, string reason)
	{
		Rpc(RpcDo_SpinValidationFailed, playerCid, reason);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_SpinValidationFailed(string playerCid, string reason)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (character.GetCharacterId() != playerCid)
			return;

		MenuBase currentMenu = GetGame().GetMenuManager().GetTopMenu();
		RL_SlotsUI slotsUI = RL_SlotsUI.Cast(currentMenu);
		if (slotsUI)
		{
			slotsUI.OnSpinValidationFailed(reason);
		}
	}

	void SlotsWinSvr(string playerCid, int winnings, array<int> slots)
	{
		Rpc(RpcDo_SlotsWinSvr, playerCid, winnings, slots);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void RpcDo_SlotsWinSvr(string playerCid, int winnings, array<int> slots)
	{
		Print("RpcDo_SlotsWinSvr");
		Tuple3<string, int, ref array<int>> context(playerCid, winnings, slots);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(playerCid, winnings, 0, this, "SlotsWinCallback", context);
	}

	void SlotsWinCallback(bool success, Managed context)
	{
		Tuple3<string, int, ref array<int>> typedContext = Tuple3<string, int, ref array<int>>.Cast(context);
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(typedContext.param1);

		if (success)
		{
			character.SlotsWinValidated(typedContext.param1, typedContext.param2);
		} else {
			character.SlotsWinValidationFailed(typedContext.param1, "Failed to process winnings");
		}
	}

	void SlotsWinValidated(string playerCid, int winnings)
	{
		Rpc(RpcDo_SlotsWinValidated, playerCid, winnings);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_SlotsWinValidated(string playerCid, int winnings)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (character.GetCharacterId() != playerCid)
			return;

		MenuBase currentMenu = GetGame().GetMenuManager().GetTopMenu();
		RL_SlotsUI slotsUI = RL_SlotsUI.Cast(currentMenu);
		if (slotsUI)
		{
			slotsUI.OnSpinWin(winnings);
		}
	}

	void SlotsWinValidationFailed(string playerCid, string reason)
	{
		Rpc(RpcDo_SlotsWinValidationFailed, playerCid, reason);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_SlotsWinValidationFailed(string playerCid, string reason)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (character.GetCharacterId() != playerCid)
			return;

		MenuBase currentMenu = GetGame().GetMenuManager().GetTopMenu();
		RL_SlotsUI slotsUI = RL_SlotsUI.Cast(currentMenu);
		if (slotsUI)
		{
			slotsUI.OnSpinValidationFailed(reason);
		}
	}
}