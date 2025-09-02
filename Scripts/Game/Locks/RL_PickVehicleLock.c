class RL_PickVehicleLock : RL_ProgressBarAction
{
	[Attribute(defvalue: "0.4", category: "Lockpicking", desc: "Chance of successful lockpicking (0.0 to 1.0)")]
	protected float m_fLockpickChance;
	
	[Attribute(defvalue: "30000", category: "Lockpicking", desc: "Time to pick lock in milliseconds")]
	protected float m_fPickTime;
	
	[Attribute(defvalue: "{5C4A0FDD6DD35009}Prefabs/Items/Lockpick.et", category: "Lockpicking", desc: "Lockpick prefab to consume on failure")]
	protected ResourceName m_sLockpickPrefab;
	
	protected RL_VehicleManagerComponent m_VehicleManagerComponent;
	protected Vehicle m_Vehicle;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_Vehicle = Vehicle.Cast(pOwnerEntity);
		if (m_Vehicle)
		{
			m_VehicleManagerComponent = RL_VehicleManagerComponent.Cast(m_Vehicle.FindComponent(RL_VehicleManagerComponent));
		}
		
		m_fMaxMoveDistance = 2;
		m_fProgressTime = m_fPickTime;
		m_sProgressText = "Picking vehicle lock...";
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		
		if (m_VehicleManagerComponent)
		{
			SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (userCharacter)
				m_VehicleManagerComponent.SetLockPickingCharacter(userCharacter.GetCharacterId());
		}
	}
	
	override void EndProgressBar()
	{
		super.EndProgressBar();
		
		int vehicleRplId = EPF_NetworkUtils.GetRplId(this.GetOwner());
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character)
			character.AskCancelLockPickingVehicle(vehicleRplId);
	}
	
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.OnActionCanceled(pOwnerEntity, pUserEntity);
		if (m_VehicleManagerComponent)
			m_VehicleManagerComponent.SetLockPickingCharacter("");
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_VehicleManagerComponent || !m_Vehicle)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
		
		if (!HasLockpickInInventory(userCharacter))
		{
			userCharacter.Notify("You need a lockpick to pick this lock", "LOCKPICKING");
			m_VehicleManagerComponent.SetLockPickingCharacter("");
			return;
		}
		
		float randomValue = Math.RandomFloat01();
		bool success = randomValue <= m_fLockpickChance;
		
		if (success)
		{
			m_VehicleManagerComponent.SetLockedState(false);
			userCharacter.Notify("Vehicle lock picked successfully!", "LOCKPICKING");
		}
		else
		{
			ConsumeLockpick(userCharacter);
			userCharacter.Notify("Lockpick failed and broke!", "LOCKPICKING");
			//PlayCarAlarmSound();
		}
		PlayCarAlarmSound();

		m_VehicleManagerComponent.SetLockPickingCharacter("");
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool HasLockpickInInventory(SCR_ChimeraCharacter character)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return false;
			
		return RL_InventoryUtils.GetAmount(inventoryManager, m_sLockpickPrefab) > 0;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ConsumeLockpick(SCR_ChimeraCharacter character)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return;
			
		RL_InventoryUtils.RemoveAmount(inventoryManager, m_sLockpickPrefab, 1);
	}
	
	//------------------------------------------------------------------------------------------------
	void PlayCarAlarmSound()
	{
		if (m_VehicleManagerComponent)
			m_VehicleManagerComponent.PlayCarAlarmRpc();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Pick Vehicle Lock";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		if (!m_VehicleManagerComponent || !m_Vehicle)
			return false;
		
		if (!m_VehicleManagerComponent.IsLocked())
			return false;
		
		if (m_VehicleManagerComponent.IsOwner(user))
			return false;

		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
			return false;
			
		string currentLockPickerId = m_VehicleManagerComponent.GetLockPickingCharacterId();
		if (!currentLockPickerId.IsEmpty() && currentLockPickerId != userCharacter.GetCharacterId())
			return false;
		
		return true;
	}
	protected bool m_bLastResult = false;
	protected int m_fUpdateInterval = 1;
	protected int m_fLastUpdateTime = 0;
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (((System.GetUnixTime() - m_fLastUpdateTime) < m_fUpdateInterval) && m_fLastUpdateTime != 0)
		{
			return m_bLastResult;
		}
		if (!m_VehicleManagerComponent || !m_Vehicle)
		{
			SetCannotPerformReason("Vehicle component not found");
			m_bLastResult = false;
			return false;
		}
		
		if (!m_VehicleManagerComponent.IsLocked())
		{
			SetCannotPerformReason("Vehicle is already unlocked");
			m_bLastResult = false;
			return false;
		}

		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("No user");
			m_bLastResult = false;
			return false;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			m_bLastResult = false;
			return false;
		}
		
		if (!HasLockpickInInventory(userCharacter))
		{
			SetCannotPerformReason("You need a lockpick to pick this lock");
			m_bLastResult = false;
			return false;
		}
		if(IsInLockpickSafeZone(user))
		{
			SetCannotPerformReason("You cannot pick locks in this area");
			m_bLastResult = false;
			return false;
		}
		m_bLastResult = true;
		return true;
	}
	protected vector m_LockpickSafeZoneMin = "8437 -2 6323";
	protected vector m_LockpickSafeZoneMax = "8563 20 6714";
	bool IsInLockpickSafeZone(IEntity user)
	{
		vector userPos = user.GetOrigin();
		
		// Debug logging
		Print(string.Format("Lockpick SafeZone Debug - User Pos: [%1, %2, %3]", userPos[0], userPos[1], userPos[2]));
		Print(string.Format("Lockpick SafeZone Debug - Min Bounds: [%1, %2, %3]", m_LockpickSafeZoneMin[0], m_LockpickSafeZoneMin[1], m_LockpickSafeZoneMin[2]));
		Print(string.Format("Lockpick SafeZone Debug - Max Bounds: [%1, %2, %3]", m_LockpickSafeZoneMax[0], m_LockpickSafeZoneMax[1], m_LockpickSafeZoneMax[2]));
		
		bool inSafeZone = (userPos[0] >= m_LockpickSafeZoneMin[0] && userPos[0] <= m_LockpickSafeZoneMax[0] &&
				userPos[1] >= m_LockpickSafeZoneMin[1] && userPos[1] <= m_LockpickSafeZoneMax[1] &&
				userPos[2] >= m_LockpickSafeZoneMin[2] && userPos[2] <= m_LockpickSafeZoneMax[2]);
		
		
		Print(string.Format("Lockpick SafeZone Debug - In Safe Zone: %1", inSafeZone));
		
		return inSafeZone;
	}
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}