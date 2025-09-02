[BaseContainerProps(), SCR_AvailableActionContextTitle()]
class RL_SeatbeltAvailableActionContext : SCR_AvailableActionContext
{
	override string GetUIName()
	{
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character && character.GetSeatBelt())
			return "Remove Seatbelt";

		return m_sName;
	}
}
