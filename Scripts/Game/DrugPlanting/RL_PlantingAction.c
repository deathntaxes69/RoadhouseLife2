class RL_PlantingAction : SCR_ScriptedUserAction
{
	[Attribute(desc: "Required item to trigger greenhouse build")]
	ResourceName m_ItemToCheck;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!pUserEntity)
			return;

		if (!RL_ResourceUtils.HasCorrectGadgetInHand(pUserEntity, m_ItemToCheck))
			return;

		CharacterControllerComponent controller = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
		if (!controller)
			return;

		IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
		if (!handItem)
			return;

		RL_SeedBagComponent seedBag = RL_SeedBagComponent.Cast(handItem.FindComponent(RL_SeedBagComponent));
		if (!seedBag)
			return;

		if (!seedBag.UseSeed())
			return;

		if (seedBag.IsEmpty())
			SCR_EntityHelper.DeleteEntityAndChildren(handItem);

		RL_PlantSpawnerComponent spawner = RL_PlantSpawnerComponent.Cast(pOwnerEntity.FindComponent(RL_PlantSpawnerComponent));
		if (!spawner)
			return;

		if (spawner.alreadyPlanted)
			return;

		vector origin = pOwnerEntity.GetOrigin();
		spawner.RpcAsk_SpawnGreenhouse(origin);
	}

	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;

		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
		{
			SetCannotPerformReason("Missing inventory");
			return false;
		}

		SCR_ResourceNamePredicate predicate = new SCR_ResourceNamePredicate(m_ItemToCheck);
		IEntity inventoryItem = inv.FindItem(predicate);
		if (!inventoryItem)
		{
			SetCannotPerformReason(RL_ResourceUtils.GetDisplayNameFromResource(m_ItemToCheck) + " is not found in inventory");
			return false;
		}

		RL_PlantSpawnerComponent spawner = RL_PlantSpawnerComponent.Cast(GetOwner().FindComponent(RL_PlantSpawnerComponent));
		if (spawner && spawner.alreadyPlanted)
		{
			SetCannotPerformReason("A plant is already growing here");
			return false;
		}

		if (!RL_ResourceUtils.HasCorrectGadgetInHand(user, m_ItemToCheck))
		{
			SetCannotPerformReason(RL_ResourceUtils.GetDisplayNameFromResource(m_ItemToCheck) + " must be in your hand");
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
		string displayName = RL_ResourceUtils.GetDisplayNameFromResource(m_ItemToCheck);
		int seedCount = -1;

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
						RL_SeedBagComponent seedBag = RL_SeedBagComponent.Cast(handItem.FindComponent(RL_SeedBagComponent));
						if (seedBag)
							seedCount = seedBag.GetSeedCount();
					}
				}
			}
		}

		if (seedCount >= 0)
			outName = "Plant " + displayName + " (" + seedCount + " left)";
		else
			outName = "Plant " + displayName;

		return true;
	}
}
