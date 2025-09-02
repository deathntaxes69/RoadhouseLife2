modded class SCR_ChimeraCharacter
{
	protected ref RL_BaseShopDbHelper m_clothingShopHelper;
	protected RL_ClothingShopUI m_clothingShopMenu;

    protected ref array<string> m_aClothingShopCacheData = {};
    protected ref array<int> m_iClothingShopCacheTimeStamp = {};
    protected int m_iClothingShopId = 0;
	protected static const int m_iClothingShopCacheExpire = 300; // seconds

	void LoadClothingShop(RL_ClothingShopUI shopUi, int shopId)
	{
		if (RplSession.Mode() == RplMode.Dedicated && Replication.IsServer())
			return;

        m_clothingShopMenu = shopUi;
        Rpc(Rpc_LoadClothingShop, shopId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_LoadClothingShop(int shopId)
	{
		if ((m_aClothingShopCacheData.Count()) <= shopId)
			m_aClothingShopCacheData.Resize(shopId + 2);

		if ((m_iClothingShopCacheTimeStamp.Count()) <= shopId)
			m_iClothingShopCacheTimeStamp.Resize(shopId + 2);

		Print("Rpc_LoadClothingShop shopid=" + shopId + " cachets=" + m_iClothingShopCacheTimeStamp[shopId]);

		if (System.GetUnixTime() > m_iClothingShopCacheTimeStamp[shopId] || m_aClothingShopCacheData[shopId] == string.Empty)
		{
            m_iClothingShopId = shopId;
        	if (!m_clothingShopHelper) m_clothingShopHelper = new RL_BaseShopDbHelper();
        	m_clothingShopHelper.LoadClothingShop(this, "LoadShopCallback", shopId);
		} else {
			//Print("Rpc_LoadClothingShop Cache shopid=" + shopId + " data=" + m_aClothingShopCacheData[shopId]);
			Rpc(RpcDo_LoadShopCallback, true, m_aClothingShopCacheData[shopId]);
		}
	}
	void LoadShopCallback(bool success, string results, int shopId)
	{
		//m_iClothingShopId = shopId; // always returns 0?
		Print("LoadShopCallback shopid=" + m_iClothingShopId);
        //Print("LoadShopCallback shopid=" + m_iClothingShopId + " results added cache=" + results + " cachets=" + m_iClothingShopCacheTimeStamp[m_iClothingShopId]);

        int m_iNewTs = System.GetUnixTime();
		m_iNewTs += m_iClothingShopCacheExpire;
		m_aClothingShopCacheData[m_iClothingShopId] = results;
		m_iClothingShopCacheTimeStamp[m_iClothingShopId] = m_iNewTs;
		Print("LoadShopCallback shopid=" + m_iClothingShopId + " new cachets=" + m_iClothingShopCacheTimeStamp[m_iClothingShopId]);

		Rpc(RpcDo_LoadShopCallback, success, results);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_LoadShopCallback(bool success, string results)
	{
        GetGame().GetScriptModule().Call(m_clothingShopMenu, "OnShopLoaded", true, null, success, results);

	}
}
