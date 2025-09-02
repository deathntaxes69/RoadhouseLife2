
modded class SCR_CharacterCommandHandlerComponent : CharacterCommandHandlerComponent
{
	override protected SCR_CharacterCommandLoiter GetLoiterCommand()
	{
		if (!m_CharacterAnimComp)
			return null;

		AnimPhysCommandScripted currentCmdScripted = m_CharacterAnimComp.GetCommandScripted();
		if (!currentCmdScripted)
			return null;
		
		return SCR_CharacterCommandLoiter.Cast(currentCmdScripted);
	}
	
}
