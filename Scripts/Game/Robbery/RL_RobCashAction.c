class RL_RobCashAction : ScriptedUserAction
{
	protected SCR_ChimeraCharacter m_ownerCharacter;
	protected SCR_ChimeraCharacter m_userCharacter;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) return;

		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);

		if (!m_ownerCharacter || !m_userCharacter)
			return;

		int targetCash = m_ownerCharacter.GetCash();
		
		if (targetCash <= 0)
		{
			m_userCharacter.Notify("Victim has no cash to rob.", "ROBBERY");
			return;
		}

		m_userCharacter.RobCashSvr(m_userCharacter.GetCharacterId(), m_ownerCharacter.GetCharacterId(), targetCash);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Rob Cash";
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
		if (!m_ownerCharacter)
			return false;
			
		m_userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!m_userCharacter)
			return false;

		if (m_ownerCharacter == m_userCharacter)
		{
			SetCannotPerformReason("Cannot rob yourself");
			return false;
		}

		if (m_ownerCharacter.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
		{
			SetCannotPerformReason("Target must be alive");
			return false;
		}

		if (!m_ownerCharacter.IsZiptied())
		{
			SetCannotPerformReason("Target must be ziptied");
			return false;
		}

		if (m_ownerCharacter.IsHandcuffed())
		{
			SetCannotPerformReason("Cannot rob police detainees");
			return false;
		}

		if (m_ownerCharacter.GetCash() <= 0)
		{
			SetCannotPerformReason("Target has no cash");
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}