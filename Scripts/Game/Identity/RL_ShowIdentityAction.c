class RL_ShowIdentityAction : ScriptedUserAction
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
		
        if(!m_charController || m_userCharacter.IsSpamming())
            return;
		
		RL_IdentityUtils.ShowIdentityToUser(m_userCharacter, m_ownerCharacter);
	}
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Show Identity";
		return true;
	}
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return CanBePerformedScript(user);
	}
	//------------------------------------------------------------------------------------------------
	// Add something here, maybe something to stop spam // add varaible to check if they already looking at ID
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		if(!m_ownerCharacter)
			return false;

		if(m_ownerCharacter == SCR_ChimeraCharacter.Cast(user))
			return false;
			
		return m_ownerCharacter.GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE;
 	}
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
        m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
        m_charController = SCR_CharacterControllerComponent.Cast(m_ownerCharacter.GetCharacterController());
	}
	
}