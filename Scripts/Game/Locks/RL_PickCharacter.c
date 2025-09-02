modded class SCR_ChimeraCharacter
{

    void AskCancelLockPickingStorage(int storageRplId)
    {
        Print("AskCancelLockPickingStorage");
        int ownerRplId = EPF_NetworkUtils.GetRplId(this);
        Rpc(RpcAsk_CancelLockPickingStorage, storageRplId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_CancelLockPickingStorage(int storageRplId)
    {
        Print("RpcAsk_CancelLockPickingStorage");
        IEntity storageEntity = EPF_NetworkUtils.FindEntityByRplId(storageRplId);
		if (!storageEntity)
			return;

        auto storageComp = SCR_UniversalInventoryStorageComponent.Cast(storageEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
        if (storageComp)
			storageComp.SetLockPickingCharacter("");
    }
    void AskCancelLockPickingVehicle(int vehicleRplId)
    {
        Print("AskCancelLockPickingVehicle");
        int ownerRplId = EPF_NetworkUtils.GetRplId(this);
        Rpc(RpcAsk_CancelLockPickingVehicle, vehicleRplId);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_CancelLockPickingVehicle(int vehicleRplId)
    {
        Print("RpcAsk_CancelLockPickingVehicle");
        IEntity vehicleEntity = EPF_NetworkUtils.FindEntityByRplId(vehicleRplId);
		if (!vehicleEntity)
			return;

        auto vehicleComp = RL_VehicleManagerComponent.Cast(vehicleEntity.FindComponent(RL_VehicleManagerComponent));
        if (vehicleComp)
			vehicleComp.SetLockPickingCharacter("");
    }


}