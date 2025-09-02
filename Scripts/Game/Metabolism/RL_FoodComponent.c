class RL_FoodWaterGadgetComponentClass : SCR_GadgetComponentClass {}

class RL_FoodWaterGadgetComponent : SCR_GadgetComponent
{
	[Attribute(defvalue: "0.1", category: "Food Values")]
	protected float m_iFoodValue;

	[Attribute(defvalue: "0.1", category: "Food Values")]
	protected float m_iThirstValue;

	[Attribute(defvalue: "", category: "Food Values")]
	protected string m_sSoundName;


	override void OnToggleActive(bool state)
	{
		m_bActivated = state;
		if (m_bActivated)
			ApplyEffect();
	}

	override void ActivateAction()
	{
		if (!m_CharacterOwner)
			return;

		ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
	}

	protected void ApplyEffect()
	{
		if (!Replication.IsServer())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_CharacterOwner);
			if (m_sSoundName != "" && character)
				character.PlayCommonSoundLocally(m_sSoundName);

			return;
		}

		Print("FoodComponent ApplyEffect");
		/*
		IEntity owner = GetOwner();
		IEntity characterOwner = owner;
		if (!SCR_CharacterControllerComponent.Cast(owner.FindComponent(SCR_CharacterControllerComponent)))
			characterOwner = owner.GetParent();
*/
		if (m_CharacterOwner) {
			RL_MetabolismComponent metabolismComponent = RL_MetabolismComponent.Cast(m_CharacterOwner.FindComponent(RL_MetabolismComponent));
			if (metabolismComponent)
				metabolismComponent.TriggerFoodDrinkIncrease(m_iFoodValue, m_iThirstValue);
		}

		Delete();
	}

	void Delete()
	{
		if (m_CharacterOwner) {
			SCR_EntityHelper.DeleteEntityAndChildren(GetOwner());
			m_bActivated = false;
		}
	}

	override EGadgetType GetType()
    {
        return EGadgetType.FOOD_WATER;
    }

}
