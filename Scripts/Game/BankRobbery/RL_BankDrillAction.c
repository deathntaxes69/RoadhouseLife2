[BaseContainerProps]
class RL_BankDrillAction : ScriptedUserAction
{
	protected IEntity m_OwnerEntity;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_OwnerEntity = pOwnerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(pOwnerEntity.FindComponent(SCR_BankDoorComponent));
		if (!bankDoor)
		{
			Print("[RL_BankDrillAction] No SCR_BankDoorComponent found on entity");
			return;
		}
		
		if (bankDoor.IsDrilling() || bankDoor.IsBlownUp() || bankDoor.IsExplosionCountdown() || bankDoor.IsOpen())
		{
			Print("[RL_BankDrillAction] Cannot drill, door state prevents it");
			return;
		}
		
		SCR_BankDoorComponentClass settings = SCR_BankDoorComponentClass.Cast(bankDoor.GetComponentData(bankDoor.GetOwner()));
		if (settings && settings.m_DrillItemPrefab != "")
		{
			int drillCount = RL_InventoryUtils.GetAmount(pUserEntity, settings.m_DrillItemPrefab);
			if (drillCount < 1)
			{
				Print("[RL_BankDrillAction] User does not have required drill item");
				return;
			}
		}
		
		bankDoor.StartOperation(RL_BankOperationType.DRILL, pUserEntity);
		
		Print("[RL_BankDrillAction] Started drilling on bank door");
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_OwnerEntity)
		{
			outName = "Start Drill";
			return true;
		}
		
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(m_OwnerEntity.FindComponent(SCR_BankDoorComponent));
		if (!bankDoor)
		{
			outName = "Start Drill";
			return true;
		}
		
		if (bankDoor.IsBlownUp() || bankDoor.IsOpen())
		{
			outName = "Door Destroyed";
		}
		else if (bankDoor.IsExplosionCountdown())
		{
			outName = "Too Late - Explosives Planted";
		}
		else if (bankDoor.IsDrilling())
		{
			int remainingTime = bankDoor.GetOperationTimeRemaining();
			if (remainingTime > 0)
			{
				outName = string.Format("Drilling... %1", RL_Utils.FormatTimeDisplay(remainingTime));
			}
			else
			{
				outName = "Drilling Complete";
			}
		}
		else
		{
			outName = "Start Drill";
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user)
			return false;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (character && character.IsPolice())
			return false;
			
		if (!m_OwnerEntity)
			return false;
			
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(m_OwnerEntity.FindComponent(SCR_BankDoorComponent));
		if (!bankDoor)
			return false;
		
		return !bankDoor.IsOpen();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user)
		{
			SetCannotPerformReason("Invalid user");
			return false;
		}
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (character && character.IsPolice())
		{
			SetCannotPerformReason("Law enforcement cannot perform this action");
			return false;
		}
		
		if (!m_OwnerEntity)
			return false;
			
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(m_OwnerEntity.FindComponent(SCR_BankDoorComponent));
		if (!bankDoor)
			return false;
		
		if (bankDoor.IsBlownUp())
		{
			SetCannotPerformReason("Door has been destroyed");
			return false;
		}
		
		if (bankDoor.IsExplosionCountdown())
		{
			SetCannotPerformReason("Too late - explosives already planted");
			return false;
		}
		
		if (bankDoor.IsDrilling() || bankDoor.IsOpen())
			return false;
		
		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_MajorCrimeManagerComponent majorCrimeManager = RL_MajorCrimeManagerComponent.Cast(gameMode.FindComponent(RL_MajorCrimeManagerComponent));
		if (majorCrimeManager && !majorCrimeManager.CanAnyMajorCrimeBePerformed())
		{
			int remainingTime = majorCrimeManager.GetMajorCrimeCooldownRemaining();
			string timeText = RL_Utils.FormatTimeDisplay(remainingTime);
			SetCannotPerformReason(string.Format("Major crime cooldown active (%1 remaining)", timeText));
			return false;
		}
		
		SCR_BankDoorComponentClass settings = SCR_BankDoorComponentClass.Cast(bankDoor.GetComponentData(bankDoor.GetOwner()));
		if (settings && settings.m_DrillItemPrefab != "")
		{
			int drillCount = RL_InventoryUtils.GetAmount(user, settings.m_DrillItemPrefab);
			if (drillCount < 1)
			{
				SetCannotPerformReason("You need a drill to perform this action");
				return false;
			}
		}
		
		if(!RL_Utils.IsDevServer())
		{
			int policeCount = RL_Utils.GetPoliceOnlineCount();
			if (policeCount < 7)
			{
				SetCannotPerformReason(string.Format("Need at least 7 police online (%1/7 currently online)", policeCount));
				return false;
			}
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}