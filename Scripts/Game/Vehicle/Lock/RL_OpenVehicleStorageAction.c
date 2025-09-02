modded class SCR_OpenVehicleStorageAction : SCR_InventoryAction {
#ifndef DISABLE_INVENTORY
	RL_VehicleManagerComponent m_vehicleComponent;

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		GetGame().GetCallqueue().CallLater(GetVehicleCompDelayed, 1000, false, pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	private void GetVehicleCompDelayed(IEntity pOwnerEntity)
	{
		const IEntity vehicle = SCR_EntityHelper.GetMainParent(pOwnerEntity, true);
		if (!vehicle)
			return;

		m_vehicleComponent = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{

		
		if (!super.CanBePerformedScript(user))
			return false;
		
		if (m_vehicleComponent && m_vehicleComponent.IsLocked(user))
		{
			SetCannotPerformReason("Vehicle is locked");
			return false;
		}
		
		return true;
	}
#endif
};