class RL_WaterPlantAction : SCR_ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!pOwnerEntity || !pUserEntity)
			return;

		RL_GrowingComponent comp = RL_GrowingComponent.Cast(pOwnerEntity.FindComponent(RL_GrowingComponent));
		if (!comp || comp.IsWatered())
			return;

		ResourceName expected = comp.GetWateringItem();

		CharacterControllerComponent controller = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
		if (!controller)
			return;

		if (!RL_ResourceUtils.HasCorrectGadgetInHand(pUserEntity, expected))
			return;

		IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
		if (!handItem)
			return;

		RL_WateringCanComponent can = RL_WateringCanComponent.Cast(handItem.FindComponent(RL_WateringCanComponent));
		if (!can)
			return;

		int usage = comp.GetWaterUsage();
		if (!can.UseWater(usage))
			return;

		comp.SetWatered();
	}

	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;

		RL_GrowingComponent comp = RL_GrowingComponent.Cast(GetOwner().FindComponent(RL_GrowingComponent));
		if (!comp || comp.IsWatered())
		{
			SetCannotPerformReason("Already watered");
			return false;
		}

		ResourceName expected = comp.GetWateringItem();

		if (!RL_ResourceUtils.HasCorrectGadgetInHand(user, expected))
		{
			SetCannotPerformReason(RL_ResourceUtils.GetDisplayNameFromResource(expected) + " is not in your hand");
			return false;
		}

		CharacterControllerComponent controller = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
		if (!controller)
			return false;

		IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
		if (!handItem)
			return false;

		RL_WateringCanComponent can = RL_WateringCanComponent.Cast(handItem.FindComponent(RL_WateringCanComponent));
		if (!can || can.GetWaterAmount() < comp.GetWaterUsage())
		{
			SetCannotPerformReason("Not enough water");
			return false;
		}

		return true;
	}

	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}

	override bool GetActionNameScript(out string outName)
	{
		if (!GetOwner())
			return false;

		RL_GrowingComponent comp = RL_GrowingComponent.Cast(GetOwner().FindComponent(RL_GrowingComponent));
		if (!comp)
			return false;

		if (comp.IsWatered())
		{
			outName = "Already Watered";
			return true;
		}

		int waterLeft = -1;

		if (GetGame().GetPlayerController())
		{
			IEntity userEntity = GetGame().GetPlayerController().GetControlledEntity();
			if (userEntity)
			{
				CharacterControllerComponent controller = CharacterControllerComponent.Cast(userEntity.FindComponent(CharacterControllerComponent));
				if (controller)
				{
					IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
					if (handItem)
					{
						RL_WateringCanComponent can = RL_WateringCanComponent.Cast(handItem.FindComponent(RL_WateringCanComponent));
						if (can)
							waterLeft = can.GetWaterAmount();
					}
				}
			}
		}

		if (waterLeft >= 0)
			outName = "Water Plant (" + waterLeft + " Liters left)";
		else
			outName = "You are out of Water";

		return true;
	}
}
