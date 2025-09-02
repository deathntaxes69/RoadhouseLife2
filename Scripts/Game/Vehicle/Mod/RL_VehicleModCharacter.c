modded class SCR_ChimeraCharacter
{
    protected ref RL_GarageDbHelper m_garageHelper;
    
    void SetVehicleColors(RplId vehicleRplId, array<string> slotNames, array<int> slotColors, int bodyColor = -1)
    {
        Rpc(RpcAsk_SetVehicleColors, vehicleRplId, slotNames, slotColors, bodyColor, RL_Utils.GetPlayerId());
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetVehicleColors(RplId vehicleRplId, array<string> slotNames, array<int> slotColors, int bodyColor, int playerId)
    {
        Print("RpcAsk_SetVehicleColors");
        
        RplComponent rplComponent = RplComponent.Cast(Replication.FindItem(vehicleRplId));
        if (!rplComponent)
        {
            Rpc(RpcDo_SetVehicleColorsCallback, false);
            return;
        }
        
        IEntity vehicleEntity = rplComponent.GetEntity();
        if (!vehicleEntity)
        {
            Rpc(RpcDo_SetVehicleColorsCallback, false);
            return;
        }
        
        RL_VehicleModsComponent modsComponent = RL_VehicleModsComponent.Cast(vehicleEntity.FindComponent(RL_VehicleModsComponent));
        if (!modsComponent)
        {
            Rpc(RpcDo_SetVehicleColorsCallback, false);
            return;
        }
        
        modsComponent.ApplyColorsWithAuthority(slotNames, slotColors, bodyColor);
        RL_VehicleManagerComponent vehicleManager = RL_VehicleManagerComponent.Cast(vehicleEntity.FindComponent(RL_VehicleManagerComponent));
        if (vehicleManager)
        {
            int garageId = vehicleManager.GetGarageId();
            if (garageId > 0)
            {
                SaveVehicleColors(garageId, slotNames, slotColors, bodyColor);
            }
        }
        
        Rpc(RpcDo_SetVehicleColorsCallback, true);
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_SetVehicleColorsCallback(bool success)
    {
        if(success)
            RL_Utils.Notify("Vehicle Colors Applied", "VEHICLE");
        else
            RL_Utils.Notify("Failed to Apply Colors", "VEHICLE");
    }
    
    protected void SaveVehicleColors(int garageId, array<string> slotNames, array<int> slotColors, int bodyColor)
    {
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("slotNames", slotNames);
        jsonCtx.WriteValue("slotColors", slotColors);
        jsonCtx.WriteValue("bodyColor", bodyColor);
        
        if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
        m_garageHelper.UpdateVehicleColors(garageId, jsonCtx.ExportToString());
    }
}
