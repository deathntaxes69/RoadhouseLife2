class RL_GiveMoneyAction : ScriptedUserAction
{
	protected SCR_ChimeraCharacter m_ownerCharacter;
	protected SCR_ChimeraCharacter m_userCharacter;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) return;

		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (m_userCharacter.IsSpamming())
			return;

		m_userCharacter.GiveMoney(m_ownerCharacter.GetCharacterId());
	}

	override bool GetActionNameScript(out string outName)
	{
		outName = "Give Money";
		return true;
	}

	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}

	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_ownerCharacter)
			return false;
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);

		if (m_ownerCharacter == userCharacter || userCharacter.GetCash() < 1)
			return false;

		return m_ownerCharacter.GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE;
	}
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
	}

}
