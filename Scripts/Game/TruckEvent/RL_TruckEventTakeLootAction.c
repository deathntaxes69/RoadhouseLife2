class RL_TruckEventTakeLootAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(pOwnerEntity.FindComponent(RL_TruckEventDropoffComponent));
		if (!dropoffComponent)
			return;
		
		if (!dropoffComponent.CanTakeLoot())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
				character.Notify("No goods available to take");
			return;
		}
		
		bool success = dropoffComponent.TakeLoot(pUserEntity);
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (character)
		{
			if (success)
			{
				character.Notify("Stolen goods have been added to your inventory");
			}
			else
			{
				character.Notify("Failed to take stolen goods");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(GetOwner().FindComponent(RL_TruckEventDropoffComponent));
		if (!dropoffComponent)
			return false;
		
		return dropoffComponent.IsDropoffActive() || dropoffComponent.IsDefendComplete() || dropoffComponent.CanTakeLoot();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
		{
			SetCannotPerformReason("Invalid entity");
			return false;
		}
		
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(GetOwner().FindComponent(RL_TruckEventDropoffComponent));
		if (!dropoffComponent)
		{
			SetCannotPerformReason("No dropoff component");
			return false;
		}
		
		if (!dropoffComponent.CanTakeLoot())
		{
			if (dropoffComponent.IsDropoffActive())
			{
				int progress = Math.Floor(dropoffComponent.GetDefendProgress() * 100);
				SetCannotPerformReason(string.Format("Breaking into truck... %1%%", progress));
			}
			else if (dropoffComponent.IsDefendComplete())
			{
				SetCannotPerformReason("Breaking into truck...");
			}
			else
			{
				SetCannotPerformReason("No goods available");
			}
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_TruckEventDropoffComponent dropoffComponent = RL_TruckEventDropoffComponent.Cast(GetOwner().FindComponent(RL_TruckEventDropoffComponent));
		if (dropoffComponent)
		{
			if (dropoffComponent.IsDropoffActive())
			{
				int progress = Math.Floor(dropoffComponent.GetDefendProgress() * 100);
				outName = string.Format("Breaking into truck... %1%%", progress);
			}
			else if (dropoffComponent.IsDefendComplete() && !dropoffComponent.CanTakeLoot())
			{
				outName = "Breaking into truck...";
			}
			else if (dropoffComponent.CanTakeLoot())
			{
				outName = "Take Stolen Goods";
			}
			else
			{
				outName = "Take Stolen Goods";
			}
		}
		else
		{
			outName = "Take Stolen Goods";
		}
		return true;
	}
}