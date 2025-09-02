class RL_AddFentanylAction : ScriptedUserAction
{
	protected static const ResourceName FENTANYL_PREFAB = "{51F5F5E5E6A0C0EB}Prefabs/Supplies/Fentanyl_Bottle.et";
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(pOwnerEntity.FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp || !campfireComp.CanAddFentanyl())
			return;
			
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(pUserEntity);
		if (!inventoryManager)
			return;
			
		int fentanylInInventory = RL_InventoryUtils.GetAmount(inventoryManager, FENTANYL_PREFAB);
		if (fentanylInInventory <= 0)
			return;
			
		int actualAmountRemoved = RL_InventoryUtils.RemoveAmount(inventoryManager, FENTANYL_PREFAB, 1);
		if (actualAmountRemoved > 0)
		{
			bool fentanylAdded = campfireComp.AddFentanyl();
			
			if (fentanylAdded)
			{
				SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
				if (character)
				{
					string message = "Added fentanyl to batch, quality increased to 50%";
					character.Notify(message, "HEROIN");
				}
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
			
		return campfireComp.CanAddFentanyl();
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
		
		if (!campfireComp.CanAddFentanyl())
		{
			if (campfireComp.HasFentanyl())
				SetCannotPerformReason("Already added fentanyl");
			else if (campfireComp.IsCookingComplete())
				SetCannotPerformReason("Cooking already finished");
			else if (campfireComp.GetPoppyCount() <= 0)
				SetCannotPerformReason("Add poppies first");
			else
				SetCannotPerformReason("Cannot add fentanyl");
			return false;
		}
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(user);
		if (!inventoryManager)
		{
			SetCannotPerformReason("No inventory");
			return false;
		}
		
		int fentanylInInventory = RL_InventoryUtils.GetAmount(inventoryManager, FENTANYL_PREFAB);
		if (fentanylInInventory <= 0)
		{
			SetCannotPerformReason("No fentanyl");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (campfireComp && campfireComp.HasFentanyl())
		{
			outName = "Fentanyl Added (50% Quality)";
		}
		else
		{
			outName = "Add Fentanyl (+30% Quality)";
		}
		return true;
	}
}
