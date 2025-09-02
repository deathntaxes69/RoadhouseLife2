[BaseContainerProps()]
class RL_VehicleSirenCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
        //Print("RL_VehicleSirenCondition IsAvailable");
		if (!data)
			return false;

		CarControllerComponent controller = CarControllerComponent.Cast(data.GetCurrentVehicleController());
		if(!controller)
			return false;
		
		Vehicle car = Vehicle.Cast(controller.GetOwner().GetRootParent());
		RL_SirenManagerComponent sirenManager = RL_SirenManagerComponent.Cast(car.FindComponent(RL_SirenManagerComponent));

		return !!sirenManager;
	}
};
