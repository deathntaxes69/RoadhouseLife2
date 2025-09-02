class RL_PoliceSearchVehicleAction : RL_ProgressBarAction
{
	protected RL_VehicleManagerComponent m_VehicleManagerComponent;
	protected Vehicle m_Vehicle;
	protected SCR_BaseCompartmentManagerComponent m_CompartmentManager;

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);

		m_Vehicle = Vehicle.Cast(pOwnerEntity);
		if (m_Vehicle)
		{
			m_VehicleManagerComponent = RL_VehicleManagerComponent.Cast(m_Vehicle.FindComponent(RL_VehicleManagerComponent));
			m_CompartmentManager = SCR_BaseCompartmentManagerComponent.Cast(m_Vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		}

		m_fProgressTime = 6000;
		m_fMaxMoveDistance = 2;
		m_sProgressText = "Searching vehicle...";
	}

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		Print("RL_PoliceSearchVehicleAction");

		if (!m_Vehicle)
			return;

		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;

		if (!userCharacter.IsPolice())
		{
			userCharacter.Notify("Only police can Search vehicles", "POLICE");
			return;
		}

		if (m_VehicleManagerComponent)
		{
			m_VehicleManagerComponent.SetPoliceSearched(true);
			userCharacter.Notify("Vehicle successfully searched and storage has been unlocked", "POLICE");
		}
	}

	override bool GetActionNameScript(out string outName)
	{
		outName = "Search Vehicle";
		return true;
	}

	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}

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
			SetCannotPerformReason("Only police can Search vehicles");
			return false;
		}

		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			return false;
		}

		if (m_VehicleManagerComponent && m_VehicleManagerComponent.HavePoliceSearched())
			return false;

		return true;
	}

	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}
