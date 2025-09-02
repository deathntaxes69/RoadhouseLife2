class RL_CancelChopVehicleAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(pOwnerEntity.FindComponent(RL_ChopShopComponent));
		if (!chopShopComp || !chopShopComp.CanCancelChop())
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		if (chopShopComp.CancelChop())
		{
			character.Notify("Vehicle chop process has been cancelled.", "CHOP SHOP");
		}
		else
		{
			character.Notify("Failed to cancel chop process.", "CHOP SHOP");
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
			
		return chopShopComp.CanCancelChop();
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
		
		if (!chopShopComp.CanCancelChop())
		{
			if (!chopShopComp.IsChopping())
				SetCannotPerformReason("No active chop to cancel");
			else if (chopShopComp.IsChopComplete())
				SetCannotPerformReason("Chop already complete");
			else
				SetCannotPerformReason("Cannot cancel chop");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_ChopShopComponent chopShopComp = RL_ChopShopComponent.Cast(GetOwner().FindComponent(RL_ChopShopComponent));
		if (chopShopComp && chopShopComp.IsChopping())
		{
			float progress = chopShopComp.GetChopProgress() * 100;
			outName = string.Format("Cancel Chop (%1%%)", Math.Round(progress));
		}
		else
		{
			outName = "Cancel Chop";
		}
		return true;
	}
}
