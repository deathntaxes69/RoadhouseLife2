class RL_CheckIDAction : ScriptedUserAction
{
	protected SCR_CharacterControllerComponent m_charController;
    protected SCR_ChimeraCharacter m_ownerCharacter;
	protected SCR_ChimeraCharacter m_userCharacter;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) return;

        m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
        m_charController = SCR_CharacterControllerComponent.Cast(m_ownerCharacter.GetCharacterController());
		
        if(!m_charController)
            return;
			
		RL_IdentityUtils.ShowIdentityToUser(m_ownerCharacter, m_userCharacter);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Check ID";
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
		if(!m_ownerCharacter)
			return false;

		if(m_ownerCharacter == SCR_ChimeraCharacter.Cast(user))
			return false;
		
		if (m_ownerCharacter.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
			return false;
		
		if (!m_ownerCharacter.IsRestrained())
			return false;
			
		return true;
 	}
 	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
        m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
        m_charController = SCR_CharacterControllerComponent.Cast(m_ownerCharacter.GetCharacterController());
	}
} 