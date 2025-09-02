modded class SCR_ChimeraCharacter
{
	protected string m_sJailUniformPrefab = "{1DF2139C8F080019}Prefabs/Prison_Outfit/Prison_Outfit.et";
	protected RL_CharacterDbHelper m_characterHelper;

	protected RL_PoliceChargeUI m_chargeMenu;
	
	void ChargePlayer(RL_PoliceChargeUI chargeMenu, int suspectId, string crime, int ticketAmount, int jailTime)
	{
		m_chargeMenu = chargeMenu;
		RplId copRplId = EPF_NetworkUtils.GetRplId(this);
		if(copRplId == suspectId)
		{
			RL_Utils.Notify("Something went wrong, DM Borat a clown face emoji and try again", "WHOOPS");
			if(m_chargeMenu) m_chargeMenu.CloseMenu();
			return;
		}
		// Jail player will run after ticket if charge includes both
		if (ticketAmount && ticketAmount > 1)
			Rpc(RpcAsk_TicketPlayer, copRplId, suspectId, crime, ticketAmount, jailTime);
		else if (jailTime && jailTime > 1)
			Rpc(RpcAsk_JailPlayer, copRplId, suspectId, crime, jailTime);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_TicketPlayer(int copId, int suspectId, string crime, int ticketAmount, int jailTime)
	{
		Print("RpcAsk_TicketPlayer");
		Tuple5<int, int, string, int, int> context(copId, suspectId, crime, ticketAmount, jailTime);
		
		SCR_ChimeraCharacter copCharacter, suspectCharacter;
		GetChargeCharacters(copId, suspectId, copCharacter, suspectCharacter);
		if(!suspectCharacter) return;
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(suspectCharacter.GetCharacterId(), 0, (-ticketAmount), this, "TicketCallback", context);
	}
	void TicketCallback(bool success, Managed context)
	{
		Tuple5<int, int, string, int, int> typedContext = Tuple5<int, int, string, int, int>.Cast(context);

		SCR_ChimeraCharacter copCharacter, suspectCharacter;
		GetChargeCharacters(typedContext.param1, typedContext.param2, copCharacter, suspectCharacter);
		if(!suspectCharacter) return;

		copCharacter.CopTicketCallback(success, suspectCharacter.GetCharacterName(), typedContext.param3, typedContext.param4);
		suspectCharacter.SuspectTicketCallback(success, typedContext.param3, typedContext.param4);
		
		if (success)
		{
			if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
			m_characterHelper.LogCrimeHistory(suspectCharacter.GetCharacterId(), typedContext.param3, typedContext.param4, 0, copCharacter.GetCharacterId());
			if(typedContext.param5 && typedContext.param5 > 0)
			{
				//Call server rpc directly since this runs server side already
				RpcAsk_JailPlayer(typedContext.param1, typedContext.param2, typedContext.param3, typedContext.param5);
			}
		}
	}

	void SuspectTicketCallback(bool success, string crime, int amount)
	{
		Rpc(RpcDo_SuspectTicketCallback, success, crime, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_SuspectTicketCallback(bool success, string crime, int amount)
	{
		if (!success)
			RL_Utils.Notify(string.Format("Failed to pay %1 ticket for %2", RL_Utils.FormatMoney(amount), crime), "TICKET");
		else
			RL_Utils.Notify(string.Format("%1 ticket paid for %2", RL_Utils.FormatMoney(amount), crime), "TICKET");
	}

	void CopTicketCallback(bool success, string suspectName, string crime, int amount)
	{
		Rpc(RpcDo_CopTicketCallback, success, suspectName, crime, amount);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_CopTicketCallback(bool success, string suspectName, string crime, int amount)
	{
		if (!success)
		{
			//Stop here if ticket payment failed so cop can issue new jail time considering ticket fail
			RL_Utils.Notify(string.Format("%1 failed to pay your ticket for %2", suspectName, RL_Utils.FormatMoney(amount)), "TICKET");
			return;
		}

		RL_Utils.Notify(string.Format("%1 payed your ticket for %2", suspectName, RL_Utils.FormatMoney(amount)), "TICKET");
		//if (m_chargeMenu)
		//		m_chargeMenu.CloseMenu();
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_JailPlayer(int copId, int suspectId, string crime, int jailTime)
	{
		Print("RpcAsk_JailPlayer");
		Tuple4<int, int, string, int> context(copId, suspectId, crime, jailTime);

		SCR_ChimeraCharacter copCharacter, suspectCharacter;
		GetChargeCharacters(copId, suspectId, copCharacter, suspectCharacter);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.AdjustJailTime(suspectCharacter.GetCharacterId(), jailTime, this, "JailCallback", context);

		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("byCharacterID", copId);
		jsonData.WriteValue("jailTime", jailTime);
		jsonData.WriteValue("crime", crime);

		m_characterHelper.PlayerLog(suspectCharacter.GetCharacterId(), "Jailed", jsonData.ExportToString());
	}

	void JailCallback(bool success, int jailTime, Managed context)
	{
		if(!success)
		{
			Print("JailPlayer Database Call failed");
			return;
		}
		Tuple4<int, int, string, int> typedContext = Tuple4<int, int, string, int>.Cast(context);
		SCR_ChimeraCharacter copCharacter, suspectCharacter;
		GetChargeCharacters(typedContext.param1, typedContext.param2, copCharacter, suspectCharacter);

		if(!suspectCharacter)
		{
			Print("JailCallback failed to find suspectCharacter");
			return;
		}
		int teleportDelay = 0;
		if(suspectCharacter.GetEscortState())
		{
			teleportDelay = 2500;
            suspectCharacter.ForceStopEscortAction();
		}

		RL_MetabolismComponent metabolismComponent = RL_MetabolismComponent.Cast(suspectCharacter.FindComponent(RL_MetabolismComponent));
		if (metabolismComponent)
			metabolismComponent.TriggerFoodDrinkIncrease(1, 1);

		copCharacter.CopJailCallback(success, suspectCharacter.GetCharacterName(), typedContext.param3, typedContext.param4);

		suspectCharacter.SuspectJailCallback(success, typedContext.param3, typedContext.param4);
		GetGame().GetCallqueue().CallLater(suspectCharacter.TeleportToJail, teleportDelay, false);

		// Put in jail uniform
		RL_ShopUtils.RemoveAllClothing(suspectCharacter);
		RL_ShopUtils.SwapEntityClothing(suspectCharacter, m_sJailUniformPrefab, false);
		
		if (suspectCharacter.IsHandcuffed())
			suspectCharacter.RestrainPlayer(false);
		
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.LogCrimeHistory(suspectCharacter.GetCharacterId(), typedContext.param3, 0, typedContext.param4, copCharacter.GetCharacterId());
	}


	void SuspectJailCallback(bool success, string crime, int jailTime)
	{
		Rpc(RpcDo_SuspectJailCallback, success, crime, jailTime);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_SuspectJailCallback(bool success, string crime, int jailTime)
	{
		if (!success)
			return;

		RL_Utils.Notify(string.Format("You were sentenced to %1 months for %2", jailTime, crime), "JAIL");
		//The rest is handeled by RL_JailCharacter
	}

	void CopJailCallback(bool success, string suspectName, string crime, int jailTime)
	{
		Rpc(RpcDo_CopJailCallback, success, suspectName, crime, jailTime);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void RpcDo_CopJailCallback(bool success, string suspectName, string crime, int jailTime)
	{
		if (!success)
		{
			RL_Utils.Notify(string.Format("Failed to send %1 to jail for %2 months", suspectName, jailTime), "JAIL");
			return;
		}

		RL_Utils.Notify(string.Format("Sentenced %1 to jail for %2 months", suspectName, jailTime), "JAIL");
		if (m_chargeMenu)
			m_chargeMenu.CloseMenu();
	}
	private void GetChargeCharacters(int copRplId, int suspectRplId, out SCR_ChimeraCharacter copCharacter, out SCR_ChimeraCharacter suspectCharacter)
	{
		IEntity cop = EPF_NetworkUtils.FindEntityByRplId(copRplId);
		IEntity suspect = EPF_NetworkUtils.FindEntityByRplId(suspectRplId);
		if(!cop || !suspect)
		{
			Print("TicketCallback failed to find copCharacter or suspectCharacter");
			return;
		}

		copCharacter = SCR_ChimeraCharacter.Cast(cop);
		suspectCharacter = SCR_ChimeraCharacter.Cast(suspect);
	}

}
