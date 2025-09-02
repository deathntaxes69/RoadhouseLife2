modded class SCR_ChimeraCharacter
{
	protected ref RL_PerksDbHelper m_perksHelper;
	protected ref RL_PerksWidget m_perksWidget;
	
	//------------------------------------------------------------------------------------------------
	bool HasPerk(string perkId)
	{
		if (!m_sOwnedPerks)
			return false;
			
		return m_sOwnedPerks.Contains(perkId);
	}
		
	//------------------------------------------------------------------------------------------------
	void GetAvailablePerkPoints(RL_PerksWidget widget = null)
	{
		if(widget)
			m_perksWidget = widget;
		Rpc(RpcAsk_GetAvailablePerkPoints);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_GetAvailablePerkPoints()
	{
		Print("RpcAsk_GetAvailablePerkPoints");
		if (!m_perksHelper)
			m_perksHelper = new RL_PerksDbHelper();
		m_perksHelper.GetCharacterPerkPoints(GetCharacterId(), this, "GetAvailablePerkPointsCallback");
	}
	
	//------------------------------------------------------------------------------------------------
	void GetAvailablePerkPointsCallback(bool success, string results)
	{
		Rpc(RpcDo_GetAvailablePerkPointsCallback, success, results);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_GetAvailablePerkPointsCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
			if (jsonCtx.ImportFromString(results))
			{
				int available;
				jsonCtx.ReadValue("perkPointsAvailable", available);
				
				if(m_perksWidget)
					m_perksWidget.OnPerkPointsLoaded(available);
			}
		}
	}
		
	//------------------------------------------------------------------------------------------------
	void PurchasePerk(string perkId, int perkPointCost)
	{
		Rpc(RpcAsk_PurchasePerk, perkId, perkPointCost);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_PurchasePerk(string perkId, int perkPointCost)
	{
		Print("RpcAsk_PurchasePerk");
		if (!m_perksHelper)
			m_perksHelper = new RL_PerksDbHelper();
		
		m_purchasingPerkId = perkId;
		m_perksHelper.PurchasePerk(GetCharacterId(), perkId, perkPointCost, this, "PurchasePerkCallback");
	}

	protected string m_purchasingPerkId;
	
	//------------------------------------------------------------------------------------------------
	void PurchasePerkCallback(bool success, string results)
	{
		if (success)
		{
			if (!m_sOwnedPerks)
				m_sOwnedPerks = new array<string>();
			
			m_sOwnedPerks.Insert(m_purchasingPerkId);
			Replication.BumpMe();
		}
		
		Rpc(RpcDo_PurchasePerkCallback, success, results);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_PurchasePerkCallback(bool success, string results)
	{
		if (success)
		{
			if(m_perksWidget)
			{
				int perkCost = GetPerkCostById(m_purchasingPerkId);
				m_perksWidget.OnPerkPurchased(true, m_purchasingPerkId, perkCost);
			}
		}
		else
		{
			if(m_perksWidget)
				m_perksWidget.OnPerkPurchased(false, m_purchasingPerkId, 0);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	void ResetPerks(int cost)
	{
		Rpc(RpcAsk_ResetPerks, cost);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ResetPerks(int cost)
	{
		Print("RpcAsk_ResetPerks");
		if (!m_perksHelper)
			m_perksHelper = new RL_PerksDbHelper();
		m_perksHelper.ResetPerkPoints(GetCharacterId(), cost, this, "ResetPerksCallback");
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPerksCallback(bool success, string results)
	{
		if (success)
		{
			// Clear owned perks array
			if (!m_sOwnedPerks)
				m_sOwnedPerks = new array<string>();
			else
				m_sOwnedPerks.Clear();
			
			Replication.BumpMe();
		}
		
		Rpc(RpcDo_ResetPerksCallback, success, results);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ResetPerksCallback(bool success, string results)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void AddPerkPoints(int points)
	{
		Rpc(RpcAsk_AddPerkPoints, points);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AddPerkPoints(int points)
	{
		Print("RpcAsk_AddPerkPoints");
		if (!m_perksHelper)
			m_perksHelper = new RL_PerksDbHelper();
		m_perksHelper.AddPerkPoints(GetCharacterId(), points);
	}
	
	//------------------------------------------------------------------------------------------------
	private int GetPerkCostById(string perkId)
	{
		Resource perkConfigResource = BaseContainerTools.LoadContainer("{5AFE299F7D419846}Configs/Perks/PerkList.conf");
		if(!perkConfigResource)
			return 0;
		
		RL_PerksConfig perkConfig = RL_PerksConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(perkConfigResource.GetResource().ToBaseContainer()));
		if(!perkConfig || !perkConfig.Perks)
			return 0;
		
		foreach(RL_PerkDefinition perk : perkConfig.Perks)
		{
			Print("[RL_PerkCharacter] GetPerkCostById foreach loop");
			if(perk.perkId == perkId)
				return perk.perkPointCost;
		}
		
		return 0;
	}
}