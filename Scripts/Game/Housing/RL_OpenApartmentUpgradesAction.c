[BaseContainerProps]
class RL_OpenApartmentUpgradesAction : ScriptedUserAction
{
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		IEntity apartmentEntity = GetOwner().GetParent();
		if (!apartmentEntity)
			return;
		
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_HouseUpgradeMenu);
		RL_HouseUpgradesUI upgradesUI = RL_HouseUpgradesUI.Cast(menuBase);
		if (upgradesUI)
		{
			upgradesUI.SetHouseEntity(apartmentEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Upgrade Apartment";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;
			
		return !character.IsRestrained();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
		{
			SetCannotPerformReason("Invalid character");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
} 