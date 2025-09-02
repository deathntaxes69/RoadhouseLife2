class RL_VehicleLockUserAction : VehicleDoorUserAction
{	
	string m_sLockSound = "SOUND_DOORLOCK";
	protected DamageManagerComponent m_DamageManager;
	protected RL_VehicleManagerComponent m_vehicleComponent;
	protected bool m_bIsMaster;

	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		Print("RL_VehicleLockUserAction Init");
		GetGame().GetCallqueue().CallLater(DelayedInit, 1000, false, pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	private void DelayedInit(IEntity pOwnerEntity)
	{
		Print("RL_VehicleLockUserAction DelayedInit");
		RplComponent rplComponent = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		m_bIsMaster = rplComponent && rplComponent.IsMaster();
		
		IEntity vehicle = SCR_EntityHelper.GetMainParent(pOwnerEntity, true);
		if (!vehicle)
			return;

		m_DamageManager = DamageManagerComponent.Cast(vehicle.FindComponent(DamageManagerComponent));
		m_vehicleComponent = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{	
		if (!m_vehicleComponent)
			return false;
		
		if (m_vehicleComponent.IsLocked())
			outName = "Unlock vehicle";
		else
			outName = "Lock vehicle";
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;

		if(!RL_Utils.IsDedicatedServer())
			character.PlayCommonSoundLocally(m_sLockSound);
		
		if (!m_bIsMaster || !pOwnerEntity || !pUserEntity)
			return;
		

		if(Replication.IsServer())
			m_vehicleComponent.SetLockedState(!m_vehicleComponent.IsLocked());
	
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (m_DamageManager && m_DamageManager.GetState() == EDamageState.DESTROYED)
			return false;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		if (!Replication.IsServer())
		{
			if (m_vehicleComponent && !m_vehicleComponent.HasVehicleAccess(user))
			{
				SetCannotPerformReason("You don't have access to this vehicle");
				return false;
			}
		}

		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (m_DamageManager && m_DamageManager.GetState() == EDamageState.DESTROYED)
			return false;

		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if (!character)
			return false;
		
		if (!m_vehicleComponent)
			return false;
	
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
};
