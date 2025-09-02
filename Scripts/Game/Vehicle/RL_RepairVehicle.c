class RL_RepairVehicleAction : RL_ProgressBarAction
{
	protected Vehicle m_Vehicle;
	protected SCR_VehicleDamageManagerComponent m_VehicleDamageManager;
	protected SCR_BaseCompartmentManagerComponent m_CompartmentManager;
	
	protected static const string REPAIR_KIT_PREFAB = "{33B2DFDCD0EBA3DB}Prefabs/Items/Equipment/Kits/RepairKit_01/RepairKit_01_wrench.et";
	protected static const int REPAIR_TIME_WITH_KIT = 30000; // 30 seconds
	protected static const int REPAIR_TIME_WITHOUT_KIT = 360000; // 6 minutes
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_Vehicle = Vehicle.Cast(pOwnerEntity);
		if (m_Vehicle)
		{
			m_VehicleDamageManager = SCR_VehicleDamageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleDamageManagerComponent));
			m_CompartmentManager = SCR_BaseCompartmentManagerComponent.Cast(m_Vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		}
		
		m_fProgressTime = REPAIR_TIME_WITHOUT_KIT;
		m_fMaxMoveDistance = 2;
		m_sProgressText = "Repairing Vehicle";
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_Vehicle)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
			
		bool hasRepairKit = HasRequiredRepairKit(userCharacter);
		if (hasRepairKit)
		{
			if (!ConsumeRepairKit(userCharacter))
			{
				userCharacter.Notify("Failed to use repair kit", "REPAIR");
				return;
			}
		}

		if (m_VehicleDamageManager)
		{
			m_VehicleDamageManager.FullHeal(true);
			if (hasRepairKit)
				userCharacter.Notify("Vehicle successfully repaired with repair kit", "REPAIR");
			else
				userCharacter.Notify("Vehicle successfully repaired manually", "REPAIR");
		}
		else
		{
			userCharacter.Notify("Could not repair vehicle", "REPAIR");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool HasRequiredRepairKit(SCR_ChimeraCharacter character)
	{
		if (!character)
			return false;
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return false;
		
		int amountInInventory = RL_InventoryUtils.GetAmount(inventoryManager, REPAIR_KIT_PREFAB);
		return (amountInInventory >= 1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ConsumeRepairKit(SCR_ChimeraCharacter character)
	{
		if (!character)
			return false;
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return false;
		
		int removedAmount = RL_InventoryUtils.RemoveAmount(inventoryManager, REPAIR_KIT_PREFAB, 1);
		return (removedAmount >= 1);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Repair Vehicle";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Vehicle)
		{
			SetCannotPerformReason("Vehicle component not found");
			return false;
		}
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("No user");
			return false;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			return false;
		}
		
		if (!m_VehicleDamageManager || !m_VehicleDamageManager.CanBeHealed())
		{
			SetCannotPerformReason("Vehicle cannot be repaired");
			return false;
		}
		
		bool hasRepairKit = HasRequiredRepairKit(userCharacter);
		if (hasRepairKit)
		{
			m_fProgressTime = REPAIR_TIME_WITH_KIT;
			m_sProgressText = "Repairing Vehicle";
		}
		else
		{
			m_fProgressTime = REPAIR_TIME_WITHOUT_KIT;
			m_sProgressText = "Repairing Vehicle (no repair kit)";
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
}
