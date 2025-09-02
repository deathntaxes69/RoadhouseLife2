[BaseContainerProps]
class RL_BankResetAction : ScriptedUserAction
{
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter || !userCharacter.IsPolice())
		{
			Print("[RL_BankResetAction] Only police can reset the bank door");
			return;
		}
		
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(pOwnerEntity.FindComponent(SCR_BankDoorComponent));
		if (!bankDoor)
		{
			Print("[RL_BankResetAction] No SCR_BankDoorComponent found on entity");
			return;
		}
		
		bankDoor.Reset();
		
		Print("[RL_BankResetAction] Bank door has been reset by police");
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter || !userCharacter.IsPolice())
			return false;
		
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(GetOwner().FindComponent(SCR_BankDoorComponent));
		if (!bankDoor || !bankDoor.IsOpen())
			return false;
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter || !userCharacter.IsPolice())
		{
			SetCannotPerformReason("Only police can reset the bank door");
			return false;
		}
		
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(GetOwner().FindComponent(SCR_BankDoorComponent));
		if (!bankDoor || !bankDoor.IsOpen())
		{
			SetCannotPerformReason("Bank door must be opened before it can be reset");
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