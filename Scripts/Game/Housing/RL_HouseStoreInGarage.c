class RL_HouseStoreInGarage : RL_StoreInGarageAction
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
	protected bool IsOwnerWithGarage(IEntity user)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;
			
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
			return false;
		
		if (!houseComponent.HasUpgradeType(HouseUpgrades.VEHICLE_SPAWN))
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
		
		return IsOwnerWithGarage(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		if (!IsOwnerWithGarage(user))
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
			if (!character)
			{
				SetCannotPerformReason("Invalid character");
				return false;
			}
			
			RL_HouseComponent houseComponent = FindHouseComponent();
			if (!houseComponent)
			{
				SetCannotPerformReason("House component not found");
				return false;
			}
			
			if (!houseComponent.IsOwned())
			{
				SetCannotPerformReason("House is not owned");
				return false;
			}
			
			if (!houseComponent.HasUpgradeType(HouseUpgrades.VEHICLE_SPAWN))
			{
				SetCannotPerformReason("House does not have vehicle garage upgrade");
				return false;
			}
			
			SetCannotPerformReason("You are not the owner of this house");
			return false;
		}
		
		return true;
	}
} 