class RL_TakeChopMoneyAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(pOwnerEntity.FindComponent(RL_ChopShopComponent));
		if (!chopShopComp || !chopShopComp.CanTakeReward())
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		int rewardAmount = chopShopComp.TakeReward(character);
		if (rewardAmount > 0)
		{
			character.TransactMoney(rewardAmount, 0);
			string message = string.Format("Received %1 from chopping vehicle", RL_Utils.FormatMoney(rewardAmount));
			character.Notify(message, "CHOP SHOP");
		}
		else
		{
			character.Notify("No reward available.", "CHOP SHOP");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
			
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (!chopShopComp)
			return false;
			
		return chopShopComp.IsChopComplete() || chopShopComp.CanTakeReward();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
		{
			SetCannotPerformReason("Invalid entity");
			return false;
		}
		
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (!chopShopComp)
		{
			SetCannotPerformReason("No chop shop component");
			return false;
		}
		
		if (!chopShopComp.CanTakeReward())
		{
			if (chopShopComp.IsChopping())
			{
				int progress = Math.Floor(chopShopComp.GetChopProgress() * 100);
				SetCannotPerformReason(string.Format("Still chopping... %1%", progress));
			}
			else if (!chopShopComp.IsChopComplete())
				SetCannotPerformReason("No completed chop");
			else
				SetCannotPerformReason("No reward available");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (chopShopComp)
		{
			if (chopShopComp.IsChopping())
			{
				int progress = Math.Floor(chopShopComp.GetChopProgress() * 100);
				int rewardAmount = chopShopComp.GetRewardAmount();
				outName = string.Format("Chopping... %1%% (%2 reward)", progress, RL_Utils.FormatMoney(rewardAmount));
			}
			else if (chopShopComp.IsChopComplete())
			{
				outName = "Take Money";
			}
			else
			{
				outName = "Take Chop Money";
			}
		}
		else
		{
			outName = "Take Chop Money";
		}
		return true;
	}
}
