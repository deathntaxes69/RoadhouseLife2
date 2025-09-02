modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	protected const LocalizedString WRONG_JOB = "You don't have permission to enter this vehicle.";

	[Attribute(defvalue: "0", UIWidgets.ComboBox, desc: "Job required", enums: ParamEnumArray.FromEnum(RL_SHOP_TYPE))]
	protected RL_SHOP_TYPE m_RequiredJob;

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		if (!IsJobAllowed(character))
		{
			SetCannotPerformReason(WRONG_JOB);
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsJobAllowed(SCR_ChimeraCharacter character)
	{
		switch (m_RequiredJob)
		{
			case RL_SHOP_TYPE.Generic: return true; // CIV
			case RL_SHOP_TYPE.Police: return character.IsPolice();
			case RL_SHOP_TYPE.Medic: return character.IsMedic();
		}
		return false;
	}
}
