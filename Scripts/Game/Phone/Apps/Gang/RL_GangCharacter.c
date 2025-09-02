modded class SCR_ChimeraCharacter
{
	protected ref RL_GangWidget m_gangWidget;
    protected ref RL_GangDbHelper m_gangHelper;
	protected ref Managed m_gangAtmUI;
	
	protected Managed m_callbackWidget;
    protected string m_callbackFunction;
	
	[RplProp(onRplName: "OnGangInvitesReplicated")]
	protected ref array<string> m_gangInvites = {};

	protected static const int GANG_CREATION_COST = 30000;
	
	//------------------------------------------------------------------------------------------------
	void OnGangInvitesReplicated()
	{
		Print("OnGangInvitesReplicated called - refreshing UI");
		if (m_gangWidget)
		{
			m_gangWidget.RefreshInvitesList();
		}
	}
	
	void AddInvite(string senderName, string gangName, int gangId)
	{
		Print("AddInvite 1");
		if (!m_gangInvites)
			m_gangInvites = new array<string>();
		Print(m_gangInvites);
		// Check if player already has an invite from this gang
		foreach (string existingInvite : m_gangInvites)
		{
			Print("[RL_GangCharacter] AddInvite foreach loop");
			array<string> data = {};
			existingInvite.Split("|", data, false);
			
			if (data.Count() >= 3 && data[2].ToInt() == gangId)
			{
				Print("already has invite");
				return;
			}
		}
		Print("AddInvite 2");
		//sanitize inputs
		senderName.Replace("|", "");
		gangName.Replace("|", "");
		string inviteData = string.Format("%1|%2|%3", senderName, gangName, gangId);
		Print("AddInvite 3");
		Print(inviteData);
		m_gangInvites.Insert(inviteData);
		Replication.BumpMe();
	}
	
	void RemoveInvite(int index, Managed callbackWidget = null, string callbackFunction = "")
	{
		Rpc(RpcAsk_RemoveInvite, index);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RemoveInvite(int index)
	{
		Print("RpcAsk_RemoveInvite");

		if (index < 0 || index >= m_gangInvites.Count())
		{
			Rpc(RpcDo_RemoveInviteCallback, false);
			return;
		}
			
		m_gangInvites.Remove(index);
		Replication.BumpMe();
		
		Rpc(RpcDo_RemoveInviteCallback, true);
	}
	
	void AcceptInvite(int index)
	{
		if (index < 0 || index >= m_gangInvites.Count())
			return;
			
		string inviteData = m_gangInvites[index];
		array<string> data = {};
		inviteData.Split("|", data, false);
		
		if (data.Count() < 3)
			return;
			
		int gangId = data[2].ToInt();
		SetGangIdAndRank(this.GetCharacterId(), gangId, 1);
		RemoveInvite(index);
	}
	
	array<string> GetInvites()
	{	
		Print("GetInvites called");
		PrintFormat("invites %1", m_gangInvites);
		return m_gangInvites;
	}
	
	void GetGang(RL_GangWidget widget)
    {
        m_gangWidget = widget;
        if (m_gangWidget)
        {
            m_gangWidget.RefreshInvitesList();
        }
        Rpc(RpcAsk_GetGang);
    }
    
    //------------------------------------------------------------------------------------------------
    void SetGangWidget(RL_GangWidget widget)
    {
        m_gangWidget = widget;
        if (m_gangWidget)
        {
            m_gangWidget.RefreshInvitesList();
        }
    }
    
    //------------------------------------------------------------------------------------------------
    void RefreshGangUI()
    {
        if (m_gangWidget)
        {
            m_gangWidget.RefreshInvitesList();
        }
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetGang()
    {
		Print("RpcAsk_GetGang");

        if (!m_gangHelper) m_gangHelper = new RL_GangDbHelper();
		int gangId = this.GetGangId();
        m_gangHelper.GetGang(gangId, this, "GetGangCallback");
    }
    void GetGangCallback(bool success, string results)
    {
		Rpc(RpcDo_GetGangCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_GetGangCallback(bool success, string results)
	{
        ref RL_GangData gangData = new RL_GangData();
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		loadContext.ReadValue("data", gangData);

		if (m_gangWidget)
			{
				if (success && gangData && gangData.GetGangId() > 0)
					{
					Print("UPDATING HAS GANG");
	        		m_gangWidget.UpdateGangData(success, true, gangData);
					}
				else
					{
					Print("UPDATING NO GANG");
					m_gangWidget.UpdateGangData(success, false, gangData);
					}
			}
	}
	void SetGangIdAndRank(string characterId, int gangId, int gangRank)
    {
        Rpc(RpcAsk_SetGangIdAndRank, characterId, gangId, gangRank);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetGangIdAndRank(string characterId, int gangId, int gangRank)
    {
		Print("RpcAsk_SetGangIdAndRank");

        if (!m_gangHelper) m_gangHelper = new RL_GangDbHelper();
        m_gangHelper.SetGangIdAndRank(characterId, gangId, gangRank, this, "SetGangCallback");
    }
    void SetGangCallback(bool success, string results)
    {
        if (success && !results.IsEmpty())
        {
            SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
            loadContext.ImportFromString(results);
            
            int gangId = -1;
            int gangRank = -1;
            string targetCharacterId = "";
            
            bool idRead = loadContext.ReadValue("gangId", gangId);
            bool rankRead = loadContext.ReadValue("gangRank", gangRank);
            bool cidRead = loadContext.ReadValue("cid", targetCharacterId);
            
            if (idRead && rankRead && cidRead && targetCharacterId == GetCharacterId())
            {
                SetGangId(gangId);
                SetGangRank(gangRank);
            }
        }
        
        Rpc(RpcDo_SetGangCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_SetGangCallback(bool success, string results)
	{
		if(success)
		{
			Print("SUCCESS");
            RL_Utils.Notify("Sucessfully modified gang status", "GANG");
			
			// Refresh gang data
			if (m_gangWidget)
				{
				Print("GETTING GANG");
				GetGang(m_gangWidget);
				}
		}
        else
            RL_Utils.Notify("Failed to modify gang status", "GANG");
	}
	
	void CreateGang(string gangName)
    {
        Rpc(RpcAsk_CreateGang, gangName);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_CreateGang(string gangName)
    {
		Print("RpcAsk_CreateGang");
        if (GetBank() < GANG_CREATION_COST)
        {
            Rpc(RpcDo_CreateGangCallback, false, string.Format("Insufficient funds. You need $%1 in your bank to create a gang.", GANG_CREATION_COST));
            return;
		}
        if (!m_gangHelper) m_gangHelper = new RL_GangDbHelper();
        m_gangHelper.CreateGang(this.GetCharacterId(), gangName, this, "CreateGangCallback");
    }
    void CreateGangCallback(bool success, string results)
    {
        if (success && (results.Contains("Failed to") || results.Contains("Gang name already exists")))
        {
            success = false;
        }
        
        if (success && !results.IsEmpty())
        {
            SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
            loadContext.ImportFromString(results);
            
            int gangId = -1;
            int gangRank = -1;
            bool idRead = loadContext.ReadValue("id", gangId);
            bool rankRead = loadContext.ReadValue("gangRank", gangRank);
            
            if (idRead && rankRead && gangId > 0)
            {
                SetGangId(gangId);
                SetGangRank(gangRank);
                TransactMoney(0, -GANG_CREATION_COST);
            }
            else
            {
                success = false;
            }
        }
        
		Rpc(RpcDo_CreateGangCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_CreateGangCallback(bool success, string results)
	{
		if(success)
		{
            RL_Utils.Notify("Successfully created gang", "GANG");
			
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			int gangId = -1;
			int gangRank = -1;
			bool idRead = loadContext.ReadValue("id", gangId);
			bool rankRead = loadContext.ReadValue("gangRank", gangRank);
			
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
			if (character)
			{
				character.SetGangId(gangId);
				character.SetGangRank(gangRank);
			}
			
			// make sure db and local synced
			if (m_gangWidget)
			{
				SetGangIdAndRank(this.GetCharacterId(), gangId, gangRank);
			}
		}
        else
		{
			string errorMessage = "Failed to create gang";
			if (!results.IsEmpty())
			{
				if (results.Contains("Gang name already exists"))
					errorMessage = "Gang name is already taken. Please choose a different name.";
				else
					{
					errorMessage = results;
					}
			}
			
			RL_Utils.Notify(errorMessage, "GANG");
		}
	}
	
	// make all of this duplicate code into one class somewhere
	void GetServerPlayerList(Managed callbackWidget, string callbackFunction)
    {
        m_callbackWidget = callbackWidget;
        m_callbackFunction = callbackFunction;
        Rpc(RpcAsk_GetServerPlayerList, RL_Utils.GetPlayerId());
    }
	
    protected ref array<int> m_playerList;
    protected ref array<string> m_nameList;
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetServerPlayerList(int requestorId)
    {
        Print("RpcAsk_GetServerPlayerList");

        m_playerList = {};
        m_nameList = {};

        array<int> playerList = {};
		GetGame().GetPlayerManager().GetPlayers(playerList);
		
		SCR_ChimeraCharacter requestorCharacter = SCR_ChimeraCharacter.Cast(SCR_PossessingManagerComponent.GetPlayerMainEntity(requestorId));
		int requestorGangId = -1;
		if (requestorCharacter)
			requestorGangId = requestorCharacter.GetGangId();
		
        foreach (int playerId : playerList)
		{
			//Print("[RL_GangCharacter] RpcAsk_GetServerPlayerList foreach loop");
            string playerName = RL_Utils.GetPlayerName(playerId);
            if(!playerName)
                continue;

            // Dont include the current player
            if(playerId == requestorId)
                continue;
                
            IEntity playerEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(playerId);
            if (!playerEntity)
                continue;
                
            SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
            if (!playerCharacter)
                continue;
                
            if (playerCharacter.IsPolice())
                continue;
                
            if (requestorGangId != -1 && playerCharacter.GetGangId() == requestorGangId)
                continue;
            
            m_playerList.Insert(playerId);
            m_nameList.Insert(playerName);
        }
        Rpc(RpcDo_GetServerPlayerListCallback, m_playerList, m_nameList);
    }
	
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_GetServerPlayerListCallback(array<int> playerList, array<string> nameList)
    {
        if(m_gangWidget)
            m_gangWidget.FillContactList(playerList, nameList);
        ScriptModule scriptModule = GetGame().GetScriptModule();
        if(m_callbackWidget && m_callbackFunction)    
            scriptModule.Call(m_callbackWidget, m_callbackFunction, true, null, playerList, nameList);
    }
	
	void SendGangInvite(int recipient, string m_gangName, int m_gangId)
    {
        Rpc(RpcAsk_SendGangInvite, RL_Utils.GetPlayerId(), recipient, m_gangName, m_gangId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SendGangInvite(int sender, int recipient, string m_gangName, int m_gangId)
    {
		
		PrintFormat("reciever %1", recipient);
        IEntity recipientEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(recipient);
		if (!recipientEntity)
		{
			Print("no reciever entity");
			return;
		}
		SCR_ChimeraCharacter recipientCharacter = SCR_ChimeraCharacter.Cast(recipientEntity);
		if (!recipientCharacter)
		{
			Print("no reciever char");
			return;
		}
        
		string senderName = RL_Utils.GetPlayerName(sender);
		
		if (recipientCharacter.GetGangId() <= 0)
		{
			recipientCharacter.AddInvite(senderName, m_gangName, m_gangId);
			Print("DEBUG SENT invite");
		}
		else
		{
			Print("DEBUG DID NOT invite");
		}
    }
	
	void GangTransaction(int cash, int bank, int gangBankUpdate, int targetGangId)
    {
        Rpc(RpcAsk_GangTransaction, cash, bank, gangBankUpdate, targetGangId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GangTransaction(int cash, int bank, int gangBankUpdate, int targetGangId)
    {
		Print("RpcAsk_GangTransaction");
        if (!m_gangHelper) m_gangHelper = new RL_GangDbHelper();
        m_gangHelper.GangTransaction(this.GetCharacterId(), targetGangId.ToString(), cash, bank, gangBankUpdate, this, "GangTransactionCallback");
    }
    void GangTransactionCallback(bool success, string results)
    {
        Print("GangTransactionCallback");
        
        if (success && !results.IsEmpty())
        {
            SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
            loadContext.ImportFromString(results);
            
            int characterCash = -1;
            int characterBank = -1;
            
            if (loadContext.ReadValue("character_cash", characterCash) &&
                loadContext.ReadValue("character_bank", characterBank))
            {
                m_iCash = characterCash;
                m_iBank = characterBank;
                Replication.BumpMe();
            }
        }
        
		Rpc(RpcDo_GangTransactionCallback, success, results);
    }
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_GangTransactionCallback(bool success, string results)
	{
		if(success)
		{
            if (!results.IsEmpty())
            {
                SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
                loadContext.ImportFromString(results);
                
                int characterCash = -1;
                int characterBank = -1;
                
                if (loadContext.ReadValue("character_cash", characterCash) &&
                    loadContext.ReadValue("character_bank", characterBank))
                {
                    m_iCash = characterCash;
                    m_iBank = characterBank;
                    if (m_OnAccountUpdated)
                        m_OnAccountUpdated.Invoke();
                }
            }
            
            RL_Utils.Notify("Gang transaction successful", "GANG");
			if (m_gangWidget)
			{
				GetGang(m_gangWidget);
			}
		}
        else
            RL_Utils.Notify("Gang transaction failed", "GANG");
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_RemoveInviteCallback(bool success)
	{
		if (m_gangWidget)
		{
			m_gangWidget.OnInviteRemoved(success);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void NotifyGangMembers(int gangId, string message)
	{
		Rpc(RpcAsk_NotifyGangMembers, gangId, message);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_NotifyGangMembers(int gangId, string message)
	{
		if (!Replication.IsServer())
			return;

		Print("RpcAsk_NotifyGangMembers");
		// Find all players in the specified gang and notify them
		array<int> playerList = {};
		GetGame().GetPlayerManager().GetPlayers(playerList);
		
		foreach (int playerId : playerList)
		{
			//Print("[RL_GangCharacter] RpcAsk_NotifyGangMembers foreach loop");
			IEntity playerEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(playerId);
			if (!playerEntity)
				continue;
				
			SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
			if (!playerCharacter)
				continue;
				
			// Check if player is in the target gang
			if (playerCharacter.GetGangId() == gangId)
			{
				playerCharacter.Notify(message, "GANG");
			}
		}
	}
	
	void GetGangBalance(Managed gangAtmUI)
    {
        m_gangAtmUI = gangAtmUI;
        Rpc(RpcAsk_GetGangBalance);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_GetGangBalance()
    {
		Print("RpcAsk_GetGangBalance");
        if (!m_gangHelper) m_gangHelper = new RL_GangDbHelper();
        int gangId = this.GetGangId();
        m_gangHelper.GetGangBalance(gangId, this, "GetGangBalanceCallback");
    }
    void GetGangBalanceCallback(bool success, string results)
    {
        Rpc(RpcDo_GetGangBalanceCallback, success, results);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_GetGangBalanceCallback(bool success, string results)
    {
        if (!success)
		{
			RL_Utils.Notify("Failed to load gang balance.", "GANG ATM");
			return;
		}
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.ImportFromString(results);
		
		int balance;
		loadContext.ReadValue("balance", balance);
		
		if (m_gangAtmUI)
			GetGame().GetScriptModule().Call(m_gangAtmUI, "OnGangBalanceReceived", true, null, balance);
        
    }
	
	void AddDeniedFee(int zoneId, string characterId)
	{
		Rpc(RpcAsk_AddDeniedFee, zoneId, characterId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AddDeniedFee(int zoneId, string characterId)
	{
		if (!Replication.IsServer())
			return;
		Print("RpcAsk_AddDeniedFee");

		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_ControlZoneFeesManager feesManager = RL_ControlZoneFeesManager.Cast(gameMode.FindComponent(RL_ControlZoneFeesManager));
		if (feesManager)
			feesManager.AddDeniedFee(zoneId, characterId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ClearDeniedFeesForZone(int zoneId)
	{
		if (!Replication.IsServer())
			return;
		Print("RpcAsk_ClearDeniedFeesForZone");

		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_ControlZoneFeesManager feesManager = RL_ControlZoneFeesManager.Cast(gameMode.FindComponent(RL_ControlZoneFeesManager));
		if (feesManager)
			feesManager.ClearDeniedFeesForZone(zoneId);
	}
	
}