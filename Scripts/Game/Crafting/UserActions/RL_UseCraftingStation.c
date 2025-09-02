[BaseContainerProps]
class RL_UseCraftingStation : ScriptedUserAction
{

	//------------------------------------------------------------------------------------------------
	protected RL_CraftingStationComponent FindCraftingStationComponent()
	{
        IEntity craftingTable = GetOwner();
        if (!craftingTable)
            return null;

        RL_CraftingStationComponent craftingStationComponent = RL_CraftingStationComponent.Cast(craftingTable.FindComponent(RL_CraftingStationComponent));

        return craftingStationComponent;
	}

	//------------------------------------------------------------------------------------------------
	protected RL_HouseComponent FindHouseComponent()
	{
        IEntity craftingTable = GetOwner();
        if (!craftingTable)
            return null;
        
        IEntity parent = craftingTable.GetParent();
        if (!parent)
            return null;

        RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));

        return houseComponent;
	}

	//------------------------------------------------------------------------------------------------
	protected RL_ApartmentComponent FindApartmentComponent()
	{
        IEntity craftingTable = GetOwner();
        if (!craftingTable)
            return null;
        
        IEntity parent = craftingTable.GetParent();
        if (!parent)
            return null;

        RL_ApartmentComponent apartmentComponent = RL_ApartmentComponent.Cast(parent.FindComponent(RL_ApartmentComponent));

        return apartmentComponent;
	}

	//------------------------------------------------------------------------------------------------
	protected RL_PickupComponent FindPickupComponent()
	{
        IEntity craftingTable = GetOwner();
        if (!craftingTable)
            return null;

        RL_PickupComponent pickupComponent = RL_PickupComponent.Cast(craftingTable.FindComponent(RL_PickupComponent));

        return pickupComponent;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RL_CraftingStationComponent craftingStationComponent = FindCraftingStationComponent();
		if (!craftingStationComponent)
			return;
		
		if (!craftingStationComponent.HasValidSettings())
			return;
		
		craftingStationComponent.OpenCraftingMenu();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		RL_ApartmentComponent apartmentComponent = FindApartmentComponent();
		if (apartmentComponent)
			return true;
			
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (houseComponent)
		{
			int characterId = character.GetCharacterId().ToInt();
			return houseComponent.IsPlayerOwner(characterId);
		}
		
		RL_PickupComponent pickupComponent = FindPickupComponent();
		if (pickupComponent)
		{
			string characterIdStr = character.GetCharacterId().ToInt().ToString();
			return pickupComponent.GetOwnerUid() == characterIdStr;
		}
		
		return true;
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
		
		RL_ApartmentComponent apartmentComponent = FindApartmentComponent();
		if (apartmentComponent)
		{
			return true;
		}
		
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (houseComponent)
		{
			if (!houseComponent.IsOwned())
			{
				SetCannotPerformReason("House is not owned");
				return false;
			}
			
			int characterId = character.GetCharacterId().ToInt();
			if (!houseComponent.IsPlayerOwner(characterId))
			{
				SetCannotPerformReason("You are not the owner of this house");
				return false;
			}
			
			return true;
		}
		
		RL_PickupComponent pickupComponent = FindPickupComponent();
		if (pickupComponent)
		{
			string characterIdStr = character.GetCharacterId().ToInt().ToString();
			if (pickupComponent.GetOwnerUid() != characterIdStr)
			{
				SetCannotPerformReason("You are not the owner of this crafting station");
				return false;
			}
			
			return true;
		}
		
		SetCannotPerformReason("House, apartment, or pickup component not found");
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}