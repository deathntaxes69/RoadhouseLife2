class RL_TruckEventRevealDropoffAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (Replication.IsServer())
			return;
			
		RL_TruckEventComponent truckEventComponent = RL_TruckEventComponent.Cast(pOwnerEntity.FindComponent(RL_TruckEventComponent));
		if (!truckEventComponent)
			return;
		
		if (!truckEventComponent.HasDropoffLocation())
			return;
		
		vector dropoffPosition = truckEventComponent.GetDropoffLocation();
		string markerText = "Chemical Truck Dropoff";
		RL_MapUtils.CreateMarkerClient(dropoffPosition, markerText, "TRUCK", Color.Orange, 1800000, RL_MARKER_TYPE.Generic);
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (character)
			character.Notify("Dropoff location marked on your map", "TRUCK");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		Vehicle truck = Vehicle.Cast(GetOwner());
		if (!truck)
			return false;
		
		RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(truck.FindComponent(RL_VehicleManagerComponent));
		if (vehicleManagerComponent && vehicleManagerComponent.IsLocked())
			return false;
		
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(truck.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager && damageManager.GetState() == EDamageState.DESTROYED)
			return false;
		
		RL_TruckEventComponent truckEventComponent = RL_TruckEventComponent.Cast(truck.FindComponent(RL_TruckEventComponent));
		if (!truckEventComponent)
			return false;
		
		return truckEventComponent.HasDropoffLocation();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return CanBeShownScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Reveal Dropoff Location";
		return true;
	}
}