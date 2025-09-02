class RL_PickupSeizeAction : ScriptedUserAction
{
	protected RL_PickupComponent m_PickupComponent;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) return;
		
		if (!m_PickupComponent)
			return;
	
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (userCharacter)
		{
			if (m_PickupComponent.IsDynamicUpgrade())
			{
				string upgradeUid = m_PickupComponent.GetUpgradeUid();
				string ownerUid = m_PickupComponent.GetOwnerUid();
				
				if (!upgradeUid.IsEmpty() && !ownerUid.IsEmpty())
				{
					SCR_ChimeraCharacter ownerCharacter = RL_Utils.FindCharacterById(ownerUid);
					if (ownerCharacter)
						ownerCharacter.RemoveDynamicUpgrade(upgradeUid);
				}
			}
			
			SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
		}
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
		if (!m_PickupComponent)
			return false;
			
		if (m_PickupComponent.IsBeingPlaced())
			return false;
			
		return CanBePerformedScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user) return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);

		if (!character) return false;
		if (!m_PickupComponent) return false;
		if (!character.IsPolice()) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_PickupComponent = RL_PickupComponent.Cast(pOwnerEntity.FindComponent(RL_PickupComponent));
	}
}