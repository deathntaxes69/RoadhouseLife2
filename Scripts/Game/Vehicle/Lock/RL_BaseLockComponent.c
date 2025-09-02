modded class SCR_BaseLockComponent : ScriptComponent { 
    private bool m_bLocked = false; 

    void UpdateStorageLock(bool lockState) {
        SCR_UniversalInventoryStorageComponent storageComp = SCR_UniversalInventoryStorageComponent.Cast(GetOwner().FindComponent(SCR_UniversalInventoryStorageComponent));
        if (storageComp) {
            storageComp.SetStorageLock(lockState);
        }
    }

    override bool IsLocked(IEntity user, BaseCompartmentSlot compartmentSlot) 
    { 
        if(super.IsLocked(user, compartmentSlot)) return true; 
        RL_VehicleManagerComponent comp = RL_VehicleManagerComponent.Cast(this.GetOwner().FindComponent(RL_VehicleManagerComponent)); 
        if(comp) m_bLocked = comp.IsLocked(user); 
        return m_bLocked; 
    } 
    override LocalizedString GetCannotPerformReason(IEntity user) 
    { 
        if(m_bLocked) return "Vehicle is locked"; 
        return super.GetCannotPerformReason(user); 
    } 
} 