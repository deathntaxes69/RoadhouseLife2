modded class SCR_ChimeraCharacter
{
    protected ref RL_BaseShopDbHelper m_furnitureShopHelper;
    protected RL_FurnitureShopUI m_furnitureShopMenu;

    protected ref array<string> m_aFurnitureShopCacheData = {};
    protected ref array<int> m_iFurnitureShopCacheTimeStamp = {};
    protected int m_iFurnitureShopId = 0;
	protected static const int m_iFurnitureShopCacheExpire = 300;

    void LoadFurnitureShop(RL_FurnitureShopUI shopUi, int shopId)
    {
		if (RplSession.Mode() == RplMode.Dedicated && Replication.IsServer())
			return;

        m_furnitureShopMenu = shopUi;
        Rpc(Rpc_LoadFurnitureShop, shopId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_LoadFurnitureShop(int shopId)
	{
		if ((m_aFurnitureShopCacheData.Count()) <= shopId)
			m_aFurnitureShopCacheData.Resize(shopId + 2);

		if ((m_iFurnitureShopCacheTimeStamp.Count()) <= shopId)
			m_iFurnitureShopCacheTimeStamp.Resize(shopId + 2);

		Print("Rpc_LoadFurnitureShop shopid=" + shopId + " cachets=" + m_iFurnitureShopCacheTimeStamp[shopId]);

		if (System.GetUnixTime() > m_iFurnitureShopCacheTimeStamp[shopId] || m_aFurnitureShopCacheData[shopId] == string.Empty)
		{
            m_iFurnitureShopId = shopId;
			if (!m_furnitureShopHelper) m_furnitureShopHelper = new RL_BaseShopDbHelper();
			m_furnitureShopHelper.LoadFurnitureShop(this, "LoadFurnitureShopCallback", shopId);
		} else {
			Rpc(RpcDo_LoadFurnitureShopCallback, true, m_aFurnitureShopCacheData[shopId]);
		}

	}
    void LoadFurnitureShopCallback(bool success, string results, int shopId)
	{
		Print("LoadFurnitureShopCallback shopid=" + m_iFurnitureShopId);
        int m_iNewTs = System.GetUnixTime();
		m_iNewTs += m_iFurnitureShopCacheExpire;
		m_aFurnitureShopCacheData[m_iFurnitureShopId] = results;
		m_iFurnitureShopCacheTimeStamp[m_iFurnitureShopId] = m_iNewTs;
		Print("LoadFurnitureShopCallback shopid=" + m_iFurnitureShopId + " new cachets=" + m_iFurnitureShopCacheTimeStamp[m_iFurnitureShopId]);

		Rpc(RpcDo_LoadFurnitureShopCallback, success, results);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_LoadFurnitureShopCallback(bool success, string results)
	{
        GetGame().GetScriptModule().Call(m_furnitureShopMenu, "OnShopLoaded", true, null, success, results);
	}
}