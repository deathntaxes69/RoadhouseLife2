[BaseContainerProps]
class RL_BankBlowUpAction : ScriptedUserAction
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
			Print("[RL_BankBlowUpAction] No SCR_BankDoorComponent found on entity");
			return;
		}
		
		if (bankDoor.IsBlownUp() || bankDoor.IsDrilling() || bankDoor.IsExplosionCountdown() || bankDoor.IsOpen())
		{
			Print("[RL_BankBlowUpAction] Cannot blow up, door state prevents it");
			return;
		}
		
		SCR_BankDoorComponentClass settings = SCR_BankDoorComponentClass.Cast(bankDoor.GetComponentData(bankDoor.GetOwner()));
		if (settings && settings.m_ExplosiveItemPrefab != "")
		{
			int explosiveCount = RL_InventoryUtils.GetAmount(pUserEntity, settings.m_ExplosiveItemPrefab);
			if (explosiveCount < 1)
			{
				Print("[RL_BankBlowUpAction] User does not have required explosive item");
				return;
			}
		}
		
		bankDoor.StartOperation(RL_BankOperationType.EXPLOSION, pUserEntity);
		
		Print("[RL_BankBlowUpAction] Planted explosives on bank door");
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_OwnerEntity)
		{
			outName = "Plant Explosives";
			return true;
		}
		
		SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(m_OwnerEntity.FindComponent(SCR_BankDoorComponent));
		if (!bankDoor)
		{
			outName = "Plant Explosives";
			return true;
		}
		
		if (bankDoor.IsBlownUp() || bankDoor.IsOpen())
		{
			outName = "Door Destroyed";
		}
		else if (bankDoor.IsExplosionCountdown())
		{
			int remainingTime = bankDoor.GetOperationTimeRemaining();
			if (remainingTime > 0)
			{
				outName = string.Format("EXPLOSION IN %1", RL_Utils.FormatTimeDisplay(remainingTime));
			}
			else
			{
				outName = "EXPLODING NOW!";
			}
		}
		else if (bankDoor.IsDrilling())
		{
			outName = "Too Late - Drilling Started";
		}
		else
		{
			outName = "Plant Explosives";
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
			SetCannotPerformReason("Door already destroyed");
			return false;
		}
		
		if (bankDoor.IsExplosionCountdown())
		{
			SetCannotPerformReason("Explosives already planted");
			return false;
		}
		
		if (bankDoor.IsDrilling())
		{
			SetCannotPerformReason("Too late - drilling has already started");
			return false;
		}
		
		if (bankDoor.IsOpen())
		{
			SetCannotPerformReason("Door is already open");
			return false;
		}
		
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
		if (settings && settings.m_ExplosiveItemPrefab != "")
		{
			int explosiveCount = RL_InventoryUtils.GetAmount(user, settings.m_ExplosiveItemPrefab);
			if (explosiveCount < 1)
			{
				SetCannotPerformReason("You need explosives to perform this action");
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