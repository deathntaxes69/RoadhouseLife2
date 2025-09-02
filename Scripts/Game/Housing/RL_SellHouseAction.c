[BaseContainerProps]
class RL_SellHouseAction : RL_BasePropertyAction
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

		string uniqueId = houseComponent.GetHouseId();
		if (uniqueId.IsEmpty())
			return;

		houseComponent.SetOwnerCid(-1);
		character.SellHouse(uniqueId);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (houseComponent)
		{
			int sellPrice = houseComponent.GetSellPrice();
			outName = string.Format("Sell House - %1", RL_Utils.FormatMoney(sellPrice));
		}
		else
		{
			outName = "";
		}
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		string reason;
		if (!ValidateBaseConditions(user, reason))
			return false;
			
		return IsHouseOwned(reason) && IsOwner(user, reason);
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