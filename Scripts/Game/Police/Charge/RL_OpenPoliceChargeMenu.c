class RL_OpenPoliceChargeMenu : ScriptedUserAction
{
	SCR_ChimeraCharacter m_ownerCharacter;
	SCR_ChimeraCharacter m_userCharacter;
    protected RL_PoliceChargeUI m_chargeMenu;

	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_PoliceChargeMenu); 
        m_chargeMenu = RL_PoliceChargeUI.Cast(menuBase);
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		m_chargeMenu.SetEntities(m_ownerCharacter, m_userCharacter);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Ticket/Jail";

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
		m_userCharacter = SCR_ChimeraCharacter.Cast(user);
		return ( 
			m_userCharacter.IsPolice() && 
			m_userCharacter != m_ownerCharacter &&
			m_ownerCharacter.GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE
			
		);
 	}
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
	
}