class RL_DismantleHeroinCampfireAction : ScriptedUserAction
{
	protected static const ResourceName HEROIN_CAMPFIRE_KIT_PREFAB = "{0E838910D88704DF}Prefabs/Heroin/HeroinCampfireKit.et";
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(pOwnerEntity.FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
			return;
			
		if (!CanDismantleCampfire(campfireComp))
			return;
			
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(pUserEntity);
		if (!inventoryManager)
			return;
			
		int addedCount = RL_InventoryUtils.AddAmount(inventoryManager, HEROIN_CAMPFIRE_KIT_PREFAB, 1, true);
		
		if (addedCount > 0)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
			{
				character.Notify("Dismantled heroin campfire", "HEROIN");
			}
			IEntity parentEntity = pOwnerEntity.GetParent();
			if (parentEntity)
			{
				SCR_EntityHelper.DeleteEntityAndChildren(parentEntity);
			}
		}
		else
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
			{
				character.Notify("Inventory full - cannot dismantle", "HEROIN");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanDismantleCampfire(RL_HeroinCampfireComponent campfireComp)
	{
		if (campfireComp.IsCooking())
			return false;
			
		if (campfireComp.GetPoppyCount() > 0)
			return false;
			
		if (campfireComp.IsCookingComplete())
			return false;
			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
			return false;
			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
		{
			SetCannotPerformReason("Invalid entity");
			return false;
		}
		
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
		{
			SetCannotPerformReason("No campfire component");
			return false;
		}
		
		if (!CanDismantleCampfire(campfireComp))
		{
			if (campfireComp.IsCooking())
				SetCannotPerformReason("Cannot dismantle while cooking");
			else if (campfireComp.IsCookingComplete())
				SetCannotPerformReason("Take paste first");
			else if (campfireComp.GetPoppyCount() > 0)
				SetCannotPerformReason("Remove poppies first");
			else
				SetCannotPerformReason("Cannot dismantle");
			return false;
		}
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(user);
		if (!inventoryManager)
		{
			SetCannotPerformReason("No inventory");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Dismantle Campfire";
		return true;
	}
}
