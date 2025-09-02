class RL_PickHandcuffLock : RL_ProgressBarAction
{
	[Attribute(defvalue: "0.4", category: "Lockpicking", desc: "Chance of successful lockpicking (0.0 to 1.0)")]
	protected float m_fLockpickChance;
	
	[Attribute(defvalue: "60000", category: "Lockpicking", desc: "Time to pick handcuff lock in milliseconds")]
	protected float m_fPickTime;
	
	[Attribute(defvalue: "{5C4A0FDD6DD35009}Prefabs/Items/Lockpick.et", category: "Lockpicking", desc: "Lockpick prefab to consume on failure")]
	protected ResourceName m_sLockpickPrefab;
	
	protected SCR_ChimeraCharacter m_ownerCharacter;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		
		m_fMaxMoveDistance = 2;
		m_fProgressTime = m_fPickTime;
		m_sProgressText = "Picking handcuff lock...";
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_ownerCharacter)
			return;
			
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
		
		if (!HasLockpickInInventory(userCharacter))
		{
			userCharacter.Notify("You need a lockpick to pick handcuffs", "LOCKPICKING");
			return;
		}
		
		float randomValue = Math.RandomFloat01();
		bool success = randomValue <= m_fLockpickChance;
		
		if (success)
		{
			m_ownerCharacter.RestrainPlayer(false);
			userCharacter.Notify("Handcuff lock picked successfully!", "LOCKPICKING");
			m_ownerCharacter.Notify("Your handcuffs have been picked!", "LOCKPICKING");
		}
		else
		{
			ConsumeLockpick(userCharacter);
			userCharacter.Notify("Lockpick failed and broke!", "LOCKPICKING");
		}
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
		outName = "Pick Handcuff Lock";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		if (!m_ownerCharacter)
			return false;
		
		if (!m_ownerCharacter.IsHandcuffed())
			return false;
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
			return false;
		
		if (userCharacter == m_ownerCharacter)
			return false;
		
		if (userCharacter.IsPolice() || userCharacter.IsHandcuffed())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_ownerCharacter)
		{
			SetCannotPerformReason("Character not found");
			return false;
		}
		
		if (!m_ownerCharacter.IsHandcuffed())
		{
			SetCannotPerformReason("Character is not handcuffed");
			return false;
		}
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!userCharacter)
		{
			SetCannotPerformReason("No user");
			return false;
		}
		
		if (userCharacter == m_ownerCharacter)
		{
			SetCannotPerformReason("You cannot pick your own handcuffs");
			return false;
		}
		
		if (!HasLockpickInInventory(userCharacter))
		{
			SetCannotPerformReason("You need a lockpick to pick this lock");
			return false;
		}
		
		SCR_CharacterDamageManagerComponent dmc = EL_Component<SCR_CharacterDamageManagerComponent>.Find(m_ownerCharacter);
		if (!dmc || m_ownerCharacter.GetCharacterController().GetLifeState() == ECharacterLifeState.DEAD)
		{
			SetCannotPerformReason("Cannot pick locks on dead characters");
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
