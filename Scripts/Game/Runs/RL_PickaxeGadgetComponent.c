class RL_PickaxeGadgetComponentClass : SCR_GadgetComponentClass {}

class RL_PickaxeGadgetComponent : SCR_GadgetComponent
{

	override EGadgetType GetType()
	{
		return EGadgetType.CONSUMABLE;
	}

	override void OnToggleActive(bool state)
	{
		m_bActivated = state;
		//if (m_bActivated)
		//	ApplyEffect();
	}

	override void ActivateAction()
	{
		if (m_CharacterOwner && m_iMode == EGadgetMode.IN_HAND)
			ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
	}

	protected void ApplyEffect()
	{

	}



}
