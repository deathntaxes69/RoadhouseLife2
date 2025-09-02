class RL_PutInVehicle : ScriptedUserAction
{
	protected float m_fVehicleSearchRadius = 4;

	protected IEntity m_NearVehicle;
    protected SCR_ChimeraCharacter m_character;
    protected SCR_ChimeraCharacter m_ownerCharacter;
	protected SCR_CharacterControllerComponent m_CharacterController;

	//------------------------------------------------------------------------------------------------
	bool AddVehicle(IEntity ent)
	{
		m_NearVehicle = ent;
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool FilterVehicles(IEntity ent)
	{
		return (ent.IsInherited(Vehicle));
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
        
        if(m_character.GetEscortState())
        {
            m_character.ForceStopEscortAction();
        }

        m_NearVehicle = null;
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), m_fVehicleSearchRadius, AddVehicle, FilterVehicles);

		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(pOwnerEntity.FindComponent(SCR_CompartmentAccessComponent));
		compartmentAccess.MoveInVehicle(m_NearVehicle, ECompartmentType.CARGO);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Put in Vehicle";
		
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
        SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!m_character || m_CharacterController.IsDead() || userCharacter == m_character)
			return false;

		if(userCharacter.IsInVehicle() || m_character.IsInVehicle())
			return false;

		// Police can put handcuffed people in vehicles
		bool canPutInVehicle = false;
		if (userCharacter.IsPolice() && m_character.IsHandcuffed())
			canPutInVehicle = true;
		// Civs can put ziptied people in vehicles
		else if (m_character.IsZiptied())
			canPutInVehicle = true;
		
		if (!canPutInVehicle)
			return false;

		m_NearVehicle = null;
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), m_fVehicleSearchRadius, AddVehicle, FilterVehicles);

		return (m_NearVehicle);
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
        m_character = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		m_CharacterController = SCR_CharacterControllerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterControllerComponent));
	
	}   
}
