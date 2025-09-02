modded class SCR_ChimeraCharacter
{
    protected ref RL_BaseShopDbHelper m_itemShopHelper;
    protected RL_ItemShopUI m_itemShopMenu;

	protected ref array<string> m_aItemShopCacheData = {};
    protected ref array<int> m_iItemShopCacheTimeStamp = {};
    protected int m_iItemShopId = 0;
	protected static const int m_iItemShopCacheExpire = 300; // seconds
	protected int m_iAdminShopIdForNoCache = 99;

    void LoadItemShop(RL_ItemShopUI shopUi, int shopId)
    {
		if (RplSession.Mode() == RplMode.Dedicated && Replication.IsServer())
			return;

        m_itemShopMenu = shopUi;
       	Rpc(Rpc_LoadItemShop, shopId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_LoadItemShop(int shopId)
	{
		if ((m_aItemShopCacheData.Count()) <= shopId)
			m_aItemShopCacheData.Resize(shopId + 2);

		if ((m_iItemShopCacheTimeStamp.Count()) <= shopId)
			m_iItemShopCacheTimeStamp.Resize(shopId + 2);

		Print("Rpc_LoadItemShop shopid=" + shopId + " cachets=" + m_iItemShopCacheTimeStamp[shopId]);

		if (System.GetUnixTime() > m_iItemShopCacheTimeStamp[shopId] || m_aItemShopCacheData[shopId] == string.Empty || shopId == m_iAdminShopIdForNoCache)
		{
            m_iItemShopId = shopId;
			Print("Rpc_LoadItemShop shopid=" + shopId + " NoCache");
		    if (!m_itemShopHelper) m_itemShopHelper = new RL_BaseShopDbHelper();
    	    m_itemShopHelper.LoadItemShop(this, "LoadItemShopCallback", shopId);
		} else {
			//Print("Rpc_LoadItemShop Cache shopid=" + shopId + " data=" + m_aItemShopCacheData[shopId]);
			Rpc(RpcDo_LoadItemShopCallback, true, m_aItemShopCacheData[shopId]);
		}
	}
    void LoadItemShopCallback(bool success, string results, int shopId)
	{
		//m_iItemShopId = shopId; // always returns 0?
		Print("LoadItemShopCallback shopid=" + m_iItemShopId);
        //Print("LoadItemShopCallback shopid=" + m_iItemShopId + " results added cache=" + results + " cachets=" + m_iItemShopCacheTimeStamp[m_iItemShopId]);

		int m_iNewTs = System.GetUnixTime();
		m_iNewTs += m_iItemShopCacheExpire;
		m_aItemShopCacheData[m_iItemShopId] = results;	
		m_iItemShopCacheTimeStamp[m_iItemShopId] = m_iNewTs;
		Print("LoadItemShopCallback shopid=" + m_iItemShopId + " new cachets=" + m_iItemShopCacheTimeStamp[m_iItemShopId]);

		Rpc(RpcDo_LoadItemShopCallback, success, results);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_LoadItemShopCallback(bool success, string results)
	{
        GetGame().GetScriptModule().Call(m_itemShopMenu, "OnShopLoaded", true, null, success, results);
	}
}