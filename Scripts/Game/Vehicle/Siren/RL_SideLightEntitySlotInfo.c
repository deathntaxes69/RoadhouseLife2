class RL_SideLightEntitySlotInfo : EntitySlotInfo
{
	[Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_SIREN_LIGHT_SIDE_POS), category: "Side Light Settings")]
	private RL_SIREN_LIGHT_SIDE_POS m_sirenLightSidePos;

	//------------------------------------------------------------------------------------------------
	RL_SIREN_LIGHT_SIDE_POS GetSidePos()
	{
		return m_sirenLightSidePos;
	}
}

sealed enum RL_SIREN_LIGHT_SIDE_POS {
    LEFT,
	MIDDLE = 1,
	RIGHT = 2,
}
