[BaseContainerProps]
class RL_OpenHouseUpgradesAction : RL_BasePropertyAction
{
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter character = GetCharacterFromUser(pUserEntity);
		if (!character)
			return;

		string reason;
		if (!IsOwner(pUserEntity, reason))
			return;
		
		IEntity houseEntity = GetOwner().GetParent();
		if (!houseEntity)
			return;
		
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_HouseUpgradeMenu);
		RL_HouseUpgradesUI upgradesUI = RL_HouseUpgradesUI.Cast(menuBase);
		if (upgradesUI)
		{
			upgradesUI.SetHouseEntity(houseEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Upgrade House";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		string reason;
		if (!ValidateBaseConditions(user, reason))
			return false;
			
		return IsOwner(user, reason);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		string reason;
		if (!ValidateBaseConditions(user, reason))
		{
			SetCannotPerformReason(reason);
			return false;
		}
		
		if (!IsHouseOwned(reason))
		{
			SetCannotPerformReason(reason);
			return false;
		}
		
		if (!IsOwner(user, reason))
		{
			SetCannotPerformReason(reason);
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