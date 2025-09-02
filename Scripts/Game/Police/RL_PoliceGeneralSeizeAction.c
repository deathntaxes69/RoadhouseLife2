class RL_PoliceGeneralSeizeAction : RL_ProgressBarAction
{
	protected IEntity m_TargetEntity;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_TargetEntity = pOwnerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_TargetEntity)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
			
		if (!userCharacter.IsPolice())
		{
			userCharacter.Notify("Only police can seize items", "POLICE");
			return;
		}

		DeleteEntity(m_TargetEntity);

		userCharacter.Notify("Item successfully seized", "POLICE");
	}
	
	protected void DeleteEntity(IEntity entity)
	{
		if (!entity)
			return;
			
		SCR_EntityHelper.DeleteEntityAndChildren(entity);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Seize and Destroy";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_TargetEntity)
		{
			SetCannotPerformReason("Target entity not found");
			return false;
		}
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("No user");
			return false;
		}
		
		if (!userCharacter.IsPolice())
		{
			SetCannotPerformReason("Only police can seize items");
			return false;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
} 