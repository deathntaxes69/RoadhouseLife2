class RL_OpenGarageMenu : ScriptedUserAction
{
	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_character;
	protected RL_VehicleSystemComponent m_spawnComponent;
    protected RL_GarageUI m_garageMenu;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_character = SCR_ChimeraCharacter.Cast(pUserEntity);
		m_ownerEntity = pOwnerEntity;

		if(!m_character || m_character.IsSpamming()) return;
		
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_GarageMenu); 
        m_garageMenu = RL_GarageUI.Cast(menuBase);
		m_garageMenu.SetEntities(m_ownerEntity, pUserEntity);

		m_character.LoadGarage(m_garageMenu, m_character.GetCharacterId());
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Garage";

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