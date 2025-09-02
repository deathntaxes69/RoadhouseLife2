modded class SCR_ChimeraCharacter
{
    protected ref RL_BaseShopDbHelper m_vehicleShopHelper;
    protected RL_VehicleShopUI m_vehicleShopMenu;

    protected ref array<string> m_aVehicleShopCacheData = {};
    protected ref array<int> m_iVehicleShopCacheTimeStamp = {};
    protected int m_iVehicleShopId = 0;
	protected static const int m_iVehicleShopCacheExpire = 300; // seconds

    void LoadVehicleShop(RL_VehicleShopUI shopUi, int shopId)
    {
		if (RplSession.Mode() == RplMode.Dedicated && Replication.IsServer())
			return;

        m_vehicleShopMenu = shopUi;
        Rpc(Rpc_LoadVehicleShop, shopId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_LoadVehicleShop(int shopId)
	{
		if ((m_aVehicleShopCacheData.Count()) <= shopId)
			m_aVehicleShopCacheData.Resize(shopId + 2);

		if ((m_iVehicleShopCacheTimeStamp.Count()) <= shopId)
			m_iVehicleShopCacheTimeStamp.Resize(shopId + 2);

		Print("Rpc_LoadVehicleShop shopid=" + shopId + " cachets=" + m_iVehicleShopCacheTimeStamp[shopId]);

		if (System.GetUnixTime() > m_iVehicleShopCacheTimeStamp[shopId] || m_aVehicleShopCacheData[shopId] == string.Empty)
		{
            m_iVehicleShopId = shopId;
			if (!m_vehicleShopHelper) m_vehicleShopHelper = new RL_BaseShopDbHelper();
			m_vehicleShopHelper.LoadVehicleShop(this, "LoadVehicleShopCallback", shopId);
		} else {
			//Print("Rpc_LoadVehicleShop Cache shopid=" + shopId + " data=" + m_aVehicleShopCacheData[shopId]);
			Rpc(RpcDo_LoadVehicleShopCallback, true, m_aVehicleShopCacheData[shopId]);
		}

	}
    void LoadVehicleShopCallback(bool success, string results, int shopId)
	{
		//m_iVehicleShopId = shopId; // always returns 0?
		Print("LoadVehicleShopCallback shopid=" + m_iVehicleShopId);
        //Print("LoadVehicleShopCallback shopid=" + m_iVehicleShopId + " results added cache=" + results + " cachets=" + m_iVehicleShopCacheTimeStamp[m_iVehicleShopId]);

        int m_iNewTs = System.GetUnixTime();
		m_iNewTs += m_iVehicleShopCacheExpire;
		m_aVehicleShopCacheData[m_iVehicleShopId] = results;
		m_iVehicleShopCacheTimeStamp[m_iVehicleShopId] = m_iNewTs;
		Print("LoadVehicleShopCallback shopid=" + m_iVehicleShopId + " new cachets=" + m_iVehicleShopCacheTimeStamp[m_iVehicleShopId]);

		Rpc(RpcDo_LoadVehicleShopCallback, success, results);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_LoadVehicleShopCallback(bool success, string results)
	{
        GetGame().GetScriptModule().Call(m_vehicleShopMenu, "OnShopLoaded", true, null, success, results);
	}
}