class RL_ChopVehicleAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(pOwnerEntity.FindComponent(RL_ChopShopComponent));
		if (!chopShopComp || !chopShopComp.CanStartChop())
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;

		array<IEntity> vehiclesInBounds = chopShopComp.GetVehiclesInBounds();
		if (vehiclesInBounds.Count() == 0)
		{
			character.Notify("No vehicles found in the chop shop area.", "CHOP SHOP");
			return;
		}
		
		if (vehiclesInBounds.Count() > 1)
		{
			character.Notify("Too many vehicles in the area. There must only be one vehicle to chop.", "CHOP SHOP");
			return;
		}
		
		IEntity targetVehicle = vehiclesInBounds[0];
		if (!chopShopComp.IsVehicleAllowed(targetVehicle))
		{
			character.Notify("This vehicle type cannot be chopped here. Bring something more valuable.", "CHOP SHOP");
			return;
		}
		
		Vehicle vehicle = Vehicle.Cast(targetVehicle);
		if (vehicle && vehicle.IsOccupied())
		{
			character.Notify("Cannot chop vehicle while people are inside it.", "CHOP SHOP");
			return;
		}
		
		if (chopShopComp.StartChop(targetVehicle))
		{
			character.Notify("Started chopping vehicle.", "CHOP SHOP");
		}
		else
		{
			character.Notify("Failed to start chopping process.", "CHOP SHOP");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
			
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (!chopShopComp)
			return false;
			
		if (chopShopComp.IsChopping() || chopShopComp.IsChopComplete())
			return false;
			
		array<IEntity> vehiclesInBounds = chopShopComp.GetVehiclesInBounds();
		return vehiclesInBounds.Count() > 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
		{
			SetCannotPerformReason("Invalid entity");
			return false;
		}
		
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (!chopShopComp)
		{
			SetCannotPerformReason("No chop shop component");
			return false;
		}
		
		if (!chopShopComp.CanStartChop())
		{
			if (chopShopComp.IsChopping())
			{
				int progress = Math.Floor(chopShopComp.GetChopProgress() * 100);
				SetCannotPerformReason(string.Format("Still chopping... %1%", progress));
			}
			else if (chopShopComp.IsChopComplete())
				SetCannotPerformReason("Chop complete - take reward first");
			else
				SetCannotPerformReason("Cannot start chop");
			return false;
		}
		
		array<IEntity> vehiclesInBounds = chopShopComp.GetVehiclesInBounds();
		if (vehiclesInBounds.Count() == 0)
		{
			SetCannotPerformReason("No vehicles in chop area");
			return false;
		}
		
		if (vehiclesInBounds.Count() > 1)
		{
			SetCannotPerformReason("Too many vehicles - only one allowed");
			return false;
		}
		
		IEntity targetVehicle = vehiclesInBounds[0];
		if (!chopShopComp.IsVehicleAllowed(targetVehicle))
		{
			SetCannotPerformReason("Vehicle type not allowed");
			return false;
		}
		
		Vehicle vehicle = Vehicle.Cast(targetVehicle);
		if (vehicle && vehicle.IsOccupied())
		{
			SetCannotPerformReason("Vehicle occupied - cannot chop");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (chopShopComp)
		{
			array<IEntity> vehiclesInBounds = chopShopComp.GetVehiclesInBounds();
			
			if (vehiclesInBounds.Count() == 1)
			{
				outName = "Chop Vehicle";
			}
			else
			{
				outName = "Chop Vehicle";
			}
		}
		else
		{
			outName = "Chop Vehicle";
		}
		return true;
	}
}
