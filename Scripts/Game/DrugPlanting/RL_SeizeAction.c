class RL_SeizeAction : SCR_ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
			
		if (!userCharacter.IsPolice())
			return;
			
		RL_OwnerSpawnerReferenceComponent refComp = RL_OwnerSpawnerReferenceComponent.Cast(pOwnerEntity.FindComponent(RL_OwnerSpawnerReferenceComponent));
		if (refComp)
		{
			RL_PlantSpawnerComponent spawner = refComp.GetSpawner();
			if (spawner)
			{
				spawner.ResetPlanted();
			}
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
		userCharacter.Notify("Plant seized and destroyed", "POLICE");
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("Invalid user");
			return false;
		}
		
		if (!userCharacter.IsPolice())
		{
			SetCannotPerformReason("Only police can seize plants");
			return false;
		}
		
		return true;
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = "Seize Plant";
		return true;
	}
} 