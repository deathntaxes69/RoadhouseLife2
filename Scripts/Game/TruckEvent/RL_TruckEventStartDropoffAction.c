class RL_TruckEventStartDropoffAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(pOwnerEntity.FindComponent(RL_TruckEventDropoffComponent));
		if (!dropoffComponent)
			return;
		
		if (!dropoffComponent.CanStartDefend())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
				character.Notify("Cannot break into truck at this time");
			return;
		}
		
		array<IEntity> trucksInBounds = dropoffComponent.GetTrucksInBounds();
		if (trucksInBounds.Count() == 0)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
				character.Notify("No truck inside");
			return;
		}
		
		IEntity truck = trucksInBounds[0];
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		string errorMessage = ValidateTruckForDropoff(truck, pOwnerEntity);
		if (errorMessage != "")
		{
			if (character)
				character.Notify(errorMessage);
			return;
		}
		
		bool success = dropoffComponent.StartDefend(truck);
		
		if (character)
		{
			if (success)
			{
				int defendTime = dropoffComponent.GetDefendTimeSeconds();
				int minutes = defendTime / 60;
				character.Notify(string.Format("Started breaking into the truck! Defend for %1 minutes", minutes));
			}
			else
			{
				character.Notify("Failed to start breaking into the truck");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected string ValidateTruckForDropoff(IEntity truck, IEntity dropoffEntity)
	{
		if (!truck)
			return "No truck found";
			
		RL_TruckEventComponent truckEventComponent = RL_TruckEventComponent.Cast(truck.FindComponent(RL_TruckEventComponent));
		if (!truckEventComponent || !truckEventComponent.HasDropoffLocation())
			return "This truck is not part of an active event";
		
		RL_TruckEventDropoffLocationComponent currentDropoffLocation = RL_TruckEventDropoffLocationComponent.Cast(dropoffEntity.FindComponent(RL_TruckEventDropoffLocationComponent));
		if (!currentDropoffLocation)
			return "Invalid dropoff location";
		
		if (!currentDropoffLocation.IsActiveDropoff())
			return "This dropoff location is not currently active";
		
		vector truckAssignedDropoff = truckEventComponent.GetDropoffLocation();
		vector currentDropoffPosition = currentDropoffLocation.GetWorldPosition();
		float distance = vector.Distance(truckAssignedDropoff, currentDropoffPosition);
		
		if (distance > 50.0)
			return "This truck must be delivered to a different location";
			
		return "";
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(GetOwner().FindComponent(RL_TruckEventDropoffComponent));
		if (!dropoffComponent)
			return false;
		
		return dropoffComponent.CanStartDefend();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
		
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(GetOwner().FindComponent(RL_TruckEventDropoffComponent));
		if (!dropoffComponent)
			return false;
		
		if (!dropoffComponent.CanStartDefend())
		{
			SetCannotPerformReason("Cannot break into truck at this time");
			return false;
		}
		
		array<IEntity> trucksInBounds = dropoffComponent.GetTrucksInBounds();
		if (trucksInBounds.Count() == 0)
		{
			SetCannotPerformReason("No truck inside");
			return false;
		}
		
		IEntity truck = trucksInBounds[0];
		string errorMessage = ValidateTruckForDropoff(truck, GetOwner());
		if (errorMessage != "")
		{
			SetCannotPerformReason(errorMessage);
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Start Breaking Into Truck";
		return true;
	}
}