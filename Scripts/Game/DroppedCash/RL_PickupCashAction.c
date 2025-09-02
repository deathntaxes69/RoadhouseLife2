class RL_PickupCashAction : ScriptedUserAction
{

	RL_DroppedCashComponent m_cashComponent;
	protected ref RL_CharacterDbHelper m_characterHelper;

	protected SCR_CharacterControllerComponent m_charController;


	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) return;

		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		m_charController = SCR_CharacterControllerComponent.Cast(userCharacter.GetCharacterController());

		int cashValue = m_cashComponent.GetCashValue();

		// Set the value to 0 before we give cash, this might be flawed but should help prevent duping
		m_cashComponent.SetProps("", 0);
		string characterId = EPF_PersistenceComponent.GetPersistentId(pUserEntity);

		Tuple2<int, int> context(EPF_NetworkUtils.GetRplId(pOwnerEntity), cashValue);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, cashValue, 0, m_charController, "PickupCashCallback", context);
	}
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Pickup Cash";
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
		return !!m_cashComponent;
	}

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_cashComponent = RL_DroppedCashComponent.Cast(pOwnerEntity.FindComponent(RL_DroppedCashComponent));
	}

}
