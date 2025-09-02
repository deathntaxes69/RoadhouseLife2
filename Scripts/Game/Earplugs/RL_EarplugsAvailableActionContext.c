[BaseContainerProps(), SCR_AvailableActionContextTitle()]
class RL_EarplugsAvailableActionContext : SCR_AvailableActionContext
{
	override string GetUIName()
	{
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if(character && character.IsEarplugged())
			return "Remove Earplugs";
			
		return m_sName;
	}
}