class RL_UnflipVehicleAction : RL_ProgressBarAction
{
	protected RL_VehicleManagerComponent m_VehicleManagerComponent;
	protected Vehicle m_Vehicle;
	protected SCR_BaseCompartmentManagerComponent m_CompartmentManager;
	
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
		
		m_fProgressTime = 5000;
		m_fMaxMoveDistance = 2;
		m_sProgressText = "Unflipping vehicle...";
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_Vehicle || !m_VehicleManagerComponent)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
			
		
		if (IsVehicleOccupied())
		{
			RL_Utils.Notify("Cannot unflip vehicle while people are inside.", "VEHICLE");
			return;
		}
		
		UnflipVehicle();
		
		RL_Utils.Notify("Vehicle successfully unflipped!", "VEHICLE");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UnflipVehicle()
	{
		if (!m_Vehicle)
			return;
			
		vector currentPos = m_Vehicle.GetOrigin();
		
		Physics physics = m_Vehicle.GetPhysics();
		if (physics)
		{
			physics.SetVelocity(vector.Zero);
			physics.SetAngularVelocity(vector.Zero);
		}
		
		vector currentRotation = m_Vehicle.GetYawPitchRoll();
		vector uprightRotation = Vector(currentRotation[0], 0, 0);
		
		vector newPos = currentPos;
		newPos[1] = newPos[1] + 0.5;
		
		m_Vehicle.SetYawPitchRoll(uprightRotation);
		m_Vehicle.SetOrigin(newPos);
		
		if (physics)
		{
			physics.ClearForces();
			physics.SetActive(ActiveState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsVehicleFlipped()
	{
		if (!m_Vehicle)
			return false;
			
		vector rotation = m_Vehicle.GetYawPitchRoll();
		
		float pitch = Math.AbsFloat(rotation[1]);
		float roll = Math.AbsFloat(rotation[2]);
		
		// Normalize angles
		if (pitch > 180)
			pitch = 360 - pitch;
		if (roll > 180)
			roll = 360 - roll;
		
		return (pitch > 30 || roll > 30);
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
		outName = "Unflip Vehicle";
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
		if (!m_Vehicle || !m_VehicleManagerComponent)
		{
			SetCannotPerformReason("Vehicle components not found");
			return false;
		}
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("Invalid user");
			return false;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			return false;
		}
		
		if (IsVehicleOccupied())
		{
			SetCannotPerformReason("Cannot unflip vehicle while people are inside");
			return false;
		}
		//Only only randos to flip if actually flipped
		if (!m_VehicleManagerComponent.HasVehicleAccess(user) && !IsVehicleFlipped())
		{
			SetCannotPerformReason("Vehicle is not flipped");
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