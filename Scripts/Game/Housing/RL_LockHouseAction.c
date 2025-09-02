[BaseContainerProps]
class RL_LockHouseAction : RL_BasePropertyAction
{
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		SCR_ChimeraCharacter character = GetCharacterFromUser(pUserEntity);
		if (!character)
			return;
			
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
			return;

		string reason;
		if (!IsOwner(pUserEntity, reason))
			return;
		
		houseComponent.ToggleLock();
		
		string message;
		if (houseComponent.IsLocked())
			message = "House locked successfully";
		else
			message = "House unlocked successfully";
		
		character.Notify(message, "HOUSING");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (houseComponent)
		{
			if (houseComponent.IsLocked())
				outName = "Unlock House";
			else
				outName = "Lock House";
		}
		else
		{
			outName = "Lock House";
		}
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
	

} 