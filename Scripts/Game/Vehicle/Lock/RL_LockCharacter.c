modded class SCR_ChimeraCharacter
{
    string m_sLockSound = "SOUND_DOORLOCK";
    protected array<IEntity> m_nearbyVehicles;
    protected int m_iLockRadius = 2;

    void OnLockVehicleHotkey()
    {
        if(!this || this.IsSpamming()) return;
        SCR_ChimeraCharacter stupidDupeCharacter =  SCR_ChimeraCharacter.Cast(this);
        if(!stupidDupeCharacter) return;
        array<IEntity> allNearbyVehicles = RL_Utils.GetNearbyVehicles(this.GetOrigin(), m_iLockRadius);
        foreach (IEntity vehicle : allNearbyVehicles)
		{
            Print("[RL_LockCharacter] OnLockVehicleHotkey foreach");
            RL_VehicleManagerComponent vehicleComponent = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
            if(vehicleComponent && vehicleComponent.HasVehicleAccess(this))
            {
                Print("Toggle Lock");
                if(vehicleComponent.IsLocked())
                {
                    RL_Utils.Notify("Unlocked", "VEHICLE");
                    stupidDupeCharacter.PlayCommonSoundLocally(m_sLockSound);
                } else { 
                    RL_Utils.Notify("Locked", "VEHICLE");
                    stupidDupeCharacter.PlayCommonSoundLocally(m_sLockSound);
                }
                int vehicleRplId = EPF_NetworkUtils.GetRplId(vehicle);
                Rpc(RpkAsk_SetLockedState, vehicleRplId);
                return;
            }
    
        }
        RL_Utils.Notify("No vehicle nearby to lock", "VEHICLE");
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpkAsk_SetLockedState(int vehicleRplId)
    {
        Print("RpkAsk_SetLockedState");
        IEntity targetEntity = EPF_NetworkUtils.FindEntityByRplId(vehicleRplId);
        if(!targetEntity) return;
        Print(targetEntity);
        RL_VehicleManagerComponent vehicleComponent = RL_VehicleManagerComponent.Cast(targetEntity.FindComponent(RL_VehicleManagerComponent));
        if(vehicleComponent)
            vehicleComponent.SetLockedState(!vehicleComponent.IsLocked());
    }
}