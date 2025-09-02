[BaseContainerProps]
class RL_BankGrabCashAction : ScriptedUserAction
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
		if (!Replication.IsServer())
			return;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (character && character.IsPolice())
			return;
		
		RL_BankCashManagerComponent cashManager = GetBankCashManager();
		if (!cashManager)
		{
			Print("[RL_BankGrabCashAction] No RL_BankCashManagerComponent found");
			return;
		}
		
		SCR_BankDoorComponent bankDoor = GetBankDoorComponent();
		if (!bankDoor)
		{
			Print("[RL_BankGrabCashAction] No SCR_BankDoorComponent found");
			return;
		}
		
		if (!bankDoor.CanDoorBeOpened())
		{
			Print("[RL_BankGrabCashAction] Bank door is not open");
			return;
		}
		
		// Get the context name to determine which drawer
		string contextName = "";
		ActionsManagerComponent actionManager = ActionsManagerComponent.Cast(pOwnerEntity.FindComponent(ActionsManagerComponent));
		if (actionManager)
		{
			array<UserActionContext> contexts = {};
			actionManager.GetContextList(contexts);
			
			foreach (UserActionContext context : contexts)
			{
				Print("[RL_BankGrabCashAction] PerformAction foreach loop");
				array<BaseUserAction> actions = {};
				context.GetActionsList(actions);
				
				foreach (BaseUserAction action : actions)
				{
					Print("[RL_BankGrabCashAction] PerformAction foreach loop 2");
					if (action == this)
					{
						contextName = context.GetContextName();
						break;
					}
				}
				
				if (contextName != "")
					break;
			}
		}
		
		if (contextName == "")
		{
			Print("[RL_BankGrabCashAction] Could not determine context name");
			return;
		}
		
		// Check if this drawer has already been robbed
		if (cashManager.IsDrawerRobbed(contextName))
		{
			Print(string.Format("[RL_BankGrabCashAction] Drawer %1 already robbed", contextName));
			return;
		}
		
		// Mark drawer as robbed
		cashManager.RobDrawer(contextName);
		
		if (character && !character.IsSpamming())
		{
			InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
			if (inventoryManager)
			{
				RL_InventoryUtils.AddAmount(inventoryManager, "{AB91088C0EEF4181}Prefabs/BankRobbery/MoneyBag.et", 1);
				Print(string.Format("[RL_BankGrabCashAction] Added money bag to player %1 from drawer %2", character.GetName(), contextName));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_BankCashManagerComponent cashManager = GetBankCashManager();
		if (!cashManager)
		{
			outName = "Grab Cash";
			return true;
		}
		
		string contextName = GetContextName();
		if (contextName != "" && cashManager.IsDrawerRobbed(contextName))
		{
			outName = "Already Emptied";
		}
		else
		{
			outName = "Grab Cash";
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
			
		SCR_BankDoorComponent bankDoor = GetBankDoorComponent();
		if (!bankDoor)
			return false;
		
		return bankDoor.CanDoorBeOpened();
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
		
		SCR_BankDoorComponent bankDoor = GetBankDoorComponent();
		if (!bankDoor)
		{
			SetCannotPerformReason("Bank system error");
			return false;
		}
		
		if (!bankDoor.CanDoorBeOpened())
		{
			SetCannotPerformReason("Bank vault is locked");
			return false;
		}
		
		RL_BankCashManagerComponent cashManager = GetBankCashManager();
		if (!cashManager)
		{
			SetCannotPerformReason("Bank system error");
			return false;
		}
		
		string contextName = GetContextName();
		if (contextName != "" && cashManager.IsDrawerRobbed(contextName))
		{
			SetCannotPerformReason("This drawer is already empty");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_BankDoorComponent GetBankDoorComponent()
	{
		if (!m_OwnerEntity)
			return null;
		
		IEntity child = m_OwnerEntity.GetChildren();
		
		while (child)
		{
			Print("[BankGrabCashAction] GetBankDoorComponent while loop");
			SCR_BankDoorComponent doorComp = SCR_BankDoorComponent.Cast(child.FindComponent(SCR_BankDoorComponent));
			if (doorComp)
				return doorComp;
				
			IEntity grandChild = child.GetChildren();
			while (grandChild)
			{
				Print("[BankGrabCashAction] GetBankDoorComponent while loop 2");
				doorComp = SCR_BankDoorComponent.Cast(grandChild.FindComponent(SCR_BankDoorComponent));
				if (doorComp)
					return doorComp;
				
				grandChild = grandChild.GetSibling();
			}
			
			child = child.GetSibling();
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected RL_BankCashManagerComponent GetBankCashManager()
	{
		if (!m_OwnerEntity)
			return null;
		
		return RL_BankCashManagerComponent.Cast(m_OwnerEntity.FindComponent(RL_BankCashManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	protected string GetContextName()
	{
		if (!m_OwnerEntity)
			return "";
		
		ActionsManagerComponent actionManager = ActionsManagerComponent.Cast(m_OwnerEntity.FindComponent(ActionsManagerComponent));
		if (!actionManager)
			return "";
		
		array<UserActionContext> contexts = {};
		actionManager.GetContextList(contexts);
		
		foreach (UserActionContext context : contexts)
		{
			Print("[BankGrabCashAction] GetContextName foreach loop");
			array<BaseUserAction> actions = {};
			context.GetActionsList(actions);
			
			foreach (BaseUserAction action : actions)
			{
				Print("[BankGrabCashAction] GetContextName foreach loop 2");
				if (action == this)
				{
					return context.GetContextName();
				}
			}
		}
		
		return "";
	}
} 