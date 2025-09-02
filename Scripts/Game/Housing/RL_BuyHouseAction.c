[BaseContainerProps]
class RL_BuyHouseAction : RL_BasePropertyAction
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
		
		character.BuyHouse(uniqueId);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (houseComponent)
		{
			int buyPrice = houseComponent.GetBuyPrice();
			outName = string.Format("Buy House - %1", RL_Utils.FormatMoney(buyPrice));
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
		
		return IsHouseNotOwned(reason);
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
		
		if (!IsHouseNotOwned(reason))
		{
			SetCannotPerformReason(reason);
			return false;
		}
		
		RL_HouseComponent houseComponent = FindHouseComponent();
		int buyPrice = houseComponent.GetBuyPrice();
		
		if (!HasSufficientFunds(user, buyPrice, reason))
		{
			SetCannotPerformReason(reason);
			return false;
		}
		
		return true;
	}
	

} 