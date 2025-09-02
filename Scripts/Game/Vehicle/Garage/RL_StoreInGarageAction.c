class RL_StoreInGarageAction : ScriptedUserAction
{

	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_character;
    protected RL_GarageUI m_garageMenu;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (m_character.IsSpamming())
			return;

		m_character.StoreVehicle(m_character);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Store Vehicle";

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
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
	
}