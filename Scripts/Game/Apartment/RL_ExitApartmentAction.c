class RL_ExitApartmentAction : ScriptedUserAction
{
	SCR_ChimeraCharacter m_ownerCharacter;
	SCR_ChimeraCharacter m_userCharacter;

	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
        m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if(!m_userCharacter || m_userCharacter.IsSpamming()) return;
        m_userCharacter.ExitApartment();
		
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Exit Apartment";

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
			true
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