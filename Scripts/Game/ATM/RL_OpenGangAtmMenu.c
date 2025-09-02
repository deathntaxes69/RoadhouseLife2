class RL_OpenGangAtmMenu : ScriptedUserAction
{
	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_character;
    protected RL_GangAtmUI m_gangAtmMenu;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!m_character)
			return;
			
		if (m_character.GetGangId() == -1)
		{
			RL_Utils.Notify("You must be in a gang to use this ATM.", "GANG ATM");
			return;
		}
		
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_GangAtmMenu); 
        m_gangAtmMenu = RL_GangAtmUI.Cast(menuBase);
		if (m_gangAtmMenu)
			m_gangAtmMenu.SetEntities(pOwnerEntity, pUserEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Gang ATM";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;
			
		return (character.GetGangId() != -1);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;
			
		return (character.GetGangId() != -1);
 	}
	
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
} 