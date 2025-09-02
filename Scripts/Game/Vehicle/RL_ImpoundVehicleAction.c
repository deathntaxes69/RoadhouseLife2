class RL_ImpoundVehicleAction : RL_ProgressBarAction
{
	protected RL_VehicleManagerComponent m_VehicleManagerComponent;
	protected Vehicle m_Vehicle;
	protected SCR_BaseCompartmentManagerComponent m_CompartmentManager;
	protected ref RL_GarageDbHelper m_garageHelper;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_Vehicle = Vehicle.Cast(pOwnerEntity);
		if (m_Vehicle)
		{
			m_VehicleManagerComponent = RL_VehicleManagerComponent.Cast(m_Vehicle.FindComponent(RL_VehicleManagerComponent));
			m_CompartmentManager = SCR_BaseCompartmentManagerComponent.Cast(m_Vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		}
		
		m_fProgressTime = 3000;
		m_fMaxMoveDistance = 2;
		m_sProgressText = "Impounding vehicle...";
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		Print("RL_ImpoundVehicleAction");
			
		if (!m_Vehicle)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
			
		if (!userCharacter.IsPolice())
		{
			userCharacter.Notify("Only police can impound vehicles", "POLICE");
			return;
		}
		
		if (IsVehicleOccupied())
		{
			userCharacter.Notify("Cannot impound vehicle while people are inside", "POLICE");
			return;
		}

		int garageId = 0;
		if (m_VehicleManagerComponent)
		{
			garageId = m_VehicleManagerComponent.GetGarageId();
		}

		DeleteVehicle(m_Vehicle);

		if (garageId > 0)
		{
			GetGame().GetCallqueue().CallLater(UpdateGarageState, 500, false, garageId, 2);
		}

		userCharacter.Notify("Vehicle successfully impounded", "POLICE");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateGarageState(int garageId, int active)
	{
		if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
		m_garageHelper.UpdateGarageState(garageId, active);
	}

	protected void DeleteVehicle(IEntity vehicle, int retryI = 0)
	{
		if (retryI > 10 || !vehicle)
			return;

		if (RL_Utils.EjectAllPlayers(vehicle))
		{
			retryI++;
			GetGame().GetCallqueue().CallLater(DeleteVehicle, 300, false, vehicle, retryI);
			return;
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(vehicle);
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsVehicleOccupied()
	{
		if (!m_Vehicle)
			return false;
			
		if (m_Vehicle.IsOccupied())
			return true;
			
		if (m_CompartmentManager)
		{
			array<IEntity> occupants = {};
			m_CompartmentManager.GetOccupants(occupants);
			return !occupants.IsEmpty();
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Impound Vehicle";
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
			SetCannotPerformReason("Vehicle components not found");
			return false;
		}
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("No user");
			return false;
		}
		
		if (!userCharacter.IsPolice())
		{
			SetCannotPerformReason("Only police can impound vehicles");
			return false;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			return false;
		}
		
		if (IsVehicleOccupied())
		{
			SetCannotPerformReason("Cannot impound vehicle while people are inside");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
} 