class RL_CeilingBirdAction : ScriptedUserAction
{
	protected IEntity ceilingBird;
	protected RL_CeilingBirdComponentClass settings;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;

		if (!settings || settings.m_BatteryItemPrefab == "")
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		int removedCount = RL_InventoryUtils.RemoveAmount(pUserEntity, settings.m_BatteryItemPrefab, 1);
		if (removedCount < 1)
		{
			character.Notify("Failed to remove the battery from your inventory", "CEILING BIRD");
			return;
		}

		character.Notify("You have replaced the battery!", "CEILING BIRD");
	}

	override bool GetActionNameScript(out string outName)
	{
		outName = "Replace Battery";
		return true;
	}

	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}

	override bool CanBePerformedScript(IEntity user)
	{
		if (!settings || settings.m_BatteryItemPrefab == "")
		{
			SetCannotPerformReason("Battery item not set");
			return false;
		}

		int batteryCount = RL_InventoryUtils.GetAmount(user, settings.m_BatteryItemPrefab);
		if (batteryCount > 0)
			return true;

		SetCannotPerformReason("You need a 9v battery to perform this action");
		return false;
	}

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		ceilingBird = pOwnerEntity;
		RL_CeilingBirdComponent component = RL_CeilingBirdComponent.Cast(ceilingBird.FindComponent(RL_CeilingBirdComponent));
		settings = RL_CeilingBirdComponentClass.Cast(component.GetComponentData(component.GetOwner()));
	}

}
