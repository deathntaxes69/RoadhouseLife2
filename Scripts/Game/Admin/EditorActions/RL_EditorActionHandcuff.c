//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class RL_EditorActionHandcuff : SCR_SelectedEntitiesContextAction
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
			
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!charController)
			return false;
			
		SCR_CharacterDamageManagerComponent dmc = EL_Component<SCR_CharacterDamageManagerComponent>.Find(character);
		bool baseConditions = (
			!character.GetEscortState() && 
			(
				charController.GetLifeState() != ECharacterLifeState.DEAD &&
				dmc && !dmc.IsWaitingForCpr()
			) &&
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
			
		if (character.IsHandcuffed())
		{
			character.RestrainPlayer(false);
		}
		else
		{
			if (character.IsZiptied())
				character.ZiptiePlayer(false);
			character.RestrainPlayer(true);
		}
	}
};