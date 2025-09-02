class RL_HandcuffAction : ScriptedUserAction
{
	protected SCR_CharacterControllerComponent m_charController;
    protected SCR_ChimeraCharacter m_ownerCharacter;
	protected SCR_ChimeraCharacter m_userCharacter;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) return;

        m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
        m_charController = SCR_CharacterControllerComponent.Cast(m_ownerCharacter.GetCharacterController());
		m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		
        if(!m_charController)
        {
            Print("No Char Controller");
            return;
        }
		if (m_ownerCharacter.IsRestrained())
		{
			if (m_ownerCharacter.IsHandcuffed())
				m_ownerCharacter.RestrainPlayer(false);
			else if (m_ownerCharacter.IsZiptied())
				m_ownerCharacter.ZiptiePlayer(false);
			return;
		}
		// civilian zipties vs police handcuffs
		if (m_userCharacter.IsPolice())
		{
			m_ownerCharacter.RestrainPlayer(true);
		}
		else
		{
			if (!ConsumeZiptie(m_userCharacter))
			{
				return;
			}
			m_ownerCharacter.ZiptiePlayer(true);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_ownerCharacter && m_ownerCharacter.IsHandcuffed())
			outName = "Remove Handcuff";
		else if (m_ownerCharacter && m_ownerCharacter.IsZiptied())
			outName = "Remove Ziptie";
		else if (m_userCharacter && !m_userCharacter.IsPolice())
			outName = "Ziptie";
		else
			outName = "Handcuff";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return CanBePerformedScript(user);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		m_userCharacter = SCR_ChimeraCharacter.Cast(user);
		SCR_CharacterDamageManagerComponent dmc = EL_Component<SCR_CharacterDamageManagerComponent>.Find(m_ownerCharacter);
		bool baseConditions = (
			!m_ownerCharacter.GetEscortState() && 
			m_userCharacter != m_ownerCharacter && 
			(
				m_ownerCharacter.GetCharacterController().GetLifeState() != ECharacterLifeState.DEAD &&
				dmc
			) &&
			!m_ownerCharacter.IsInVehicle()
		);
		
		if (!baseConditions)
			return false;
			
		// police can remove handcuffs, anyone can remove zipties
		if (m_ownerCharacter.IsHandcuffed() && m_userCharacter.IsPolice())
			return true;
		if (m_ownerCharacter.IsZiptied())
			return true;
			
		// police can handcuff anyone
		if (m_userCharacter.IsPolice() && !m_ownerCharacter.IsRestrained())
			return true;
			
		// civs need zipties
		if (!m_userCharacter.IsPolice() && !m_ownerCharacter.IsRestrained() && m_ownerCharacter.IsSurendering() && HasZiptie(m_userCharacter))
			return true;
			
		return false;
 	}
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
        m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
        m_charController = SCR_CharacterControllerComponent.Cast(m_ownerCharacter.GetCharacterController());

	
	}
	
	protected bool HasZiptie(SCR_ChimeraCharacter character)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return false;
			
		int ziptieCount = RL_InventoryUtils.GetAmount(inventoryManager, "{E86FEE2686EA222F}Prefabs/Items/Zipties.et");
		return ziptieCount > 0;
	}
	
	protected bool ConsumeZiptie(SCR_ChimeraCharacter character)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return false;
			
		int amountRemoved = RL_InventoryUtils.RemoveAmount(inventoryManager, "{E86FEE2686EA222F}Prefabs/Items/Zipties.et", 1);
		return amountRemoved >= 1;
	}
	
}