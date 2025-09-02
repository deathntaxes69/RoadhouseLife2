modded class SCR_GetInUserAction
{	
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;

		const Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(GetOwner(), true));
        if(vehicle && vehicle.GetManagerComp() && vehicle.GetManagerComp().IsLocked(user)) { 
            SetCannotPerformReason("Locked"); 
            return false; 
        } 
		
		return true;
	}
};
