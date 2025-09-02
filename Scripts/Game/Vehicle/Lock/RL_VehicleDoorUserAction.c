modded class SCR_OpenVehicleDoorUserAction 
{ 
    override bool CanBeShownScript(IEntity user) 
    { 
        if(!super.CanBeShownScript(user)) return false;
        const Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(GetOwner(), true));
        if(vehicle && vehicle.GetManagerComp() && vehicle.GetManagerComp().IsLocked(user)) { 
            SetCannotPerformReason("Locked"); 
            return false; 
        } 
        return true; 
    } 
} 
modded class SCR_CloseVehicleDoorUserAction 
{ 
    override bool CanBeShownScript(IEntity user) 
    { 
        if(!super.CanBeShownScript(user)) return false; 
        const Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(GetOwner(), true));
        if(vehicle && vehicle.GetManagerComp() && vehicle.GetManagerComp().IsLocked(user)) { 
            SetCannotPerformReason("Locked"); 
            return false; 
        } 
        return true; 
    } 
}