class RL_AddPoppiesAction : ScriptedUserAction
{
	// Prefab for opium poppy
	protected static const ResourceName OPIUM_POPPY_PREFAB = "{3D2A8ABA4B0E98F4}Prefabs/Supplies/Poppy_Flower.et";
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(pOwnerEntity.FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp || !campfireComp.CanAddPoppies())
			return;
			
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(pUserEntity);
		if (!inventoryManager)
			return;
			
		int poppiesInInventory = RL_InventoryUtils.GetAmount(inventoryManager, OPIUM_POPPY_PREFAB);
		if (poppiesInInventory <= 0)
			return;
			
		int maxCanAdd = campfireComp.GetMaxPoppies() - campfireComp.GetPoppyCount();
		int amountToAdd = Math.Min(poppiesInInventory, maxCanAdd);
		
		int actualAmountRemoved = RL_InventoryUtils.RemoveAmount(inventoryManager, OPIUM_POPPY_PREFAB, amountToAdd);
		if (actualAmountRemoved > 0)
		{
			int actualAmountAdded = campfireComp.AddPoppies(actualAmountRemoved);
			
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
			{
				string message = string.Format("Added %1 opium poppies to campfire", actualAmountAdded);
				character.Notify(message, "HEROIN");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
			return false;
			
		return campfireComp.CanAddPoppies();
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
		
		if (!campfireComp.CanAddPoppies())
		{
			if (campfireComp.IsCooking())
				SetCannotPerformReason("Already cooking");
			else if (campfireComp.IsCookingComplete())
				SetCannotPerformReason("Take paste first");
			else if (campfireComp.GetPoppyCount() >= campfireComp.GetMaxPoppies())
				SetCannotPerformReason("Campfire is full");
			else
				SetCannotPerformReason("Cannot add poppies");
			return false;
		}
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(user);
		if (!inventoryManager)
		{
			SetCannotPerformReason("No inventory");
			return false;
		}
		
		int poppiesInInventory = RL_InventoryUtils.GetAmount(inventoryManager, OPIUM_POPPY_PREFAB);
		if (poppiesInInventory <= 0)
		{
			SetCannotPerformReason("No opium poppies");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (campfireComp)
		{
			int currentCount = campfireComp.GetPoppyCount();
			int maxCount = campfireComp.GetMaxPoppies();
			outName = string.Format("Add Opium Poppies (%1/%2)", currentCount, maxCount);
		}
		else
		{
			outName = "Add Opium Poppies";
		}
		return true;
	}
}
