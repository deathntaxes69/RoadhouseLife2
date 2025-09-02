class RL_PickupAction : ScriptedUserAction
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
			int addedAmount = RL_InventoryUtils.AddAmount(userCharacter, m_PickupComponent.GetBlueprintPrefab(), 1, true);
			if (addedAmount > 0)
			{
				m_PickupComponent.OnPickedUp();
				SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
			}
		}
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
		
		bool allowAnyone = m_PickupComponent.AllowAnyone();
		bool allowOnlyPolice = m_PickupComponent.AllowOnlyPolice();

		if (allowAnyone && allowOnlyPolice)
			return character.IsPolice();
		
		if (allowOnlyPolice)
			return character.IsPolice();

		if (allowAnyone)
			return true;
		
		string userCharacterId = character.GetCharacterId();
		string ownerUid = m_PickupComponent.GetOwnerUid();

		if (userCharacterId != ownerUid) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_PickupComponent = RL_PickupComponent.Cast(pOwnerEntity.FindComponent(RL_PickupComponent));
	}
}
