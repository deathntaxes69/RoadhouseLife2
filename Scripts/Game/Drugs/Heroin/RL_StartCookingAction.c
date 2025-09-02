class RL_StartCookingAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(pOwnerEntity.FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp || !campfireComp.CanStartCooking())
			return;
			
		bool cookingStarted = campfireComp.StartCooking();
		
		if (cookingStarted)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
			{
				int poppyCount = campfireComp.GetPoppyCount();
				string message = string.Format("Started cooking %1 poppies", poppyCount);
				character.Notify(message, "HEROIN");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
			return false;
			
		return campfireComp.CanStartCooking();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
		{
			SetCannotPerformReason("Invalid entity");
			return false;
		}
		
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
		{
			SetCannotPerformReason("No campfire component");
			return false;
		}
		
		if (!campfireComp.CanStartCooking())
		{
			if (campfireComp.IsCooking())
				SetCannotPerformReason("Already cooking");
			else if (campfireComp.IsCookingComplete())
				SetCannotPerformReason("Take paste first");
			else if (campfireComp.GetPoppyCount() <= 0)
				SetCannotPerformReason("No poppies to cook");
			else
				SetCannotPerformReason("Cannot start cooking");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (campfireComp)
		{
			int poppyCount = campfireComp.GetPoppyCount();
			outName = string.Format("Start Cooking (%1 poppies)", poppyCount);
		}
		else
		{
			outName = "Start Cooking";
		}
		return true;
	}
}
