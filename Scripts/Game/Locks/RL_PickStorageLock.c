class RL_PickStorageLock : RL_ProgressBarAction
{
	[Attribute(defvalue: "0.33", category: "Lockpicking", desc: "Chance of successful lockpicking (0.0 to 1.0)")]
	protected float m_fLockpickChance;
	
	[Attribute(defvalue: "{A81F501D3EDD4B40}Prefabs/Items/Equipment/Lockpick.et", category: "Lockpicking", desc: "Lockpick prefab to consume on failure")]
	protected ResourceName m_sLockpickPrefab;
	
	protected SCR_UniversalInventoryStorageComponent m_StorageComponent;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_StorageComponent = SCR_UniversalInventoryStorageComponent.Cast(pOwnerEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
		m_fMaxMoveDistance = 2;
		m_sProgressText = "Picking lock...";
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		
		if (m_StorageComponent && Replication.IsServer())
		{
			SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (userCharacter)
				m_StorageComponent.SetLockPickingCharacter(userCharacter.GetCharacterId());
		}
	}
	
	override void EndProgressBar()
	{
		Print("EndProgressBar");
		super.EndProgressBar();
		
		
		int storageRplId = EPF_NetworkUtils.GetRplId(this.GetOwner());
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character)
			character.AskCancelLockPickingStorage(storageRplId);
	}
	
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.OnActionCanceled(pOwnerEntity, pUserEntity);
		
		if (m_StorageComponent)
			m_StorageComponent.SetLockPickingCharacter("");
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_StorageComponent)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
		
		if (!HasLockpickInInventory(userCharacter))
		{
			userCharacter.Notify("You need a lockpick to pick this lock", "LOCKPICKING");
			m_StorageComponent.SetLockPickingCharacter("");
			return;
		}
		
		float randomValue = Math.RandomFloat01();
		bool success = randomValue <= m_fLockpickChance;
		
		if (success)
		{
			m_StorageComponent.SetStorageLock(false);
			userCharacter.Notify("Lock picked successfully!", "LOCKPICKING");
		}
		else
		{
			ConsumeLockpick(userCharacter);
			userCharacter.Notify("Lockpick failed and broke!", "LOCKPICKING");
		}
		
		m_StorageComponent.SetLockPickingCharacter("");
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
	override bool GetActionNameScript(out string outName)
	{
		outName = "Pick Lock";
		return true;
	}
	
		//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		if (!m_StorageComponent)
			return false;
		
		if (!m_StorageComponent.IsStorageLocked())
			return false;

		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
			return false;
			
		string currentLockPickerId = m_StorageComponent.GetLockPickingCharacterId();
		if (!currentLockPickerId.IsEmpty() && currentLockPickerId != userCharacter.GetCharacterId())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_StorageComponent)
		{
			SetCannotPerformReason("Storage component not found");
			return false;
		}
		
		if (!m_StorageComponent.IsStorageLocked())
		{
			SetCannotPerformReason("Storage is already unlocked");
			return false;
		}
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("No user");
			return false;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform actions right now");
			return false;
		}
		
		if (!HasLockpickInInventory(userCharacter))
		{
			SetCannotPerformReason("You need a lockpick to pick this lock");
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
