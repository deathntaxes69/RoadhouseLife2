//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class RL_EditorActionRagdoll : SCR_SelectedEntitiesContextAction
{
	
	override bool CanBeShown(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		return CanBePerformed(selectedEntity, cursorWorldPosition, flags);
	}
	
	override bool CanBePerformed(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		if (!selectedEntity)
			return false;
			
		IEntity entity = selectedEntity.GetOwner();
		if (!entity)
			return false;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if (!character)
			return false;
			
		CharacterAnimationComponent animComponent = CharacterAnimationComponent.Cast(entity.FindComponent(CharacterAnimationComponent));
		if (!animComponent)
			return false;
			
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!charController)
			return false;
			
		bool baseConditions = (
			charController.GetLifeState() != ECharacterLifeState.DEAD &&
			!character.IsInVehicle()
		);
		
		return baseConditions;
	}
	
		override void Perform(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition)
	{
		
		if (!Replication.IsServer())
			return;
		
		if (!selectedEntity)
			return;
			
		IEntity entity = selectedEntity.GetOwner();
		if (!entity)
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if (!character)
			return;
			
		CharacterAnimationComponent animComponent = CharacterAnimationComponent.Cast(entity.FindComponent(CharacterAnimationComponent));
		if (!animComponent)
			return;
			
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!charController)
			return;
			
		animComponent.AddRagdollEffectorDamage(Vector(0, 0, 0), Vector(0, 0, 0.5), 5, 20, 5);
		charController.Ragdoll(true);
	}
}; 