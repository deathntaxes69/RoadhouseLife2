class RL_OpenHouseGarage : RL_OpenGarageMenu
{
	//------------------------------------------------------------------------------------------------
	protected RL_HouseComponent FindHouseComponent()
	{
        IEntity houseManager = GetOwner();
        if (!houseManager)
            return null;
        
        IEntity parent = houseManager.GetParent();
        if (!parent)
            return null;

        RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
        return houseComponent;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsOwner(IEntity user)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;
			
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
			return false;
		
		string characterId = character.GetCharacterId();
		if (!characterId)
			return false;

		return houseComponent.IsPlayerOwner(characterId.ToInt());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		if (!super.CanBeShownScript(user))
			return false;
		
		if (!IsOwner(user))
			return false;
			
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
			return false;
        
		return houseComponent.HasUpgradeType(HouseUpgrades.VEHICLE_SPAWN);
 	}
} 