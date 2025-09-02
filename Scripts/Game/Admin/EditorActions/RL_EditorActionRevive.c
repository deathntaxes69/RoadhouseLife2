//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class RL_EditorActionRevive : SCR_SelectedEntitiesContextAction
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
		if (!dmc)
			return false;
			
		bool canRevive = (
			!character.GetEscortState() && 
			charController.GetLifeState() != ECharacterLifeState.DEAD &&
			dmc.IsWaitingForCpr() &&
			!character.IsInVehicle()
		);
		
		return canRevive;
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
			
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!controller)
			return;
			
		if (controller.GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return;
			
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageMgr)
			return;
		
		//This will also reset CPR props
		RL_MedicalUtils.Heal(damageMgr, 1, 1, true);

		RL_MetabolismComponent metabolism = RL_MetabolismComponent.Cast(entity.FindComponent(RL_MetabolismComponent));
		if (metabolism)
		{
			metabolism.SetFoodLevel(1.0);
			metabolism.SetThirstLevel(1.0);
		}
	}
};