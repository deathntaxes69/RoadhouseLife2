class RL_OpenAtmMenu : ScriptedUserAction
{
	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_character;
    protected RL_AtmUI m_atmMenu;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
        m_character = SCR_ChimeraCharacter.Cast(pUserEntity);
        if(!m_character || m_character.IsSpamming()) return;

        if (m_character.GetHasWarrant())
        {
            string messageTitle = "ATM Alert";
			string message = string.Format("%1 accessed an ATM with an active warrant.", m_character.GetCharacterName());
			string markerText = string.Format("Wanted person %1 accessed an ATM.", m_character.GetCharacterName());
            m_character.SendEmergencyToPolice(messageTitle, message, markerText);
        }
        
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_AtmMenu); 
        m_atmMenu = RL_AtmUI.Cast(menuBase);
		m_atmMenu.SetEntities(pOwnerEntity, pUserEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open ATM";

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
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