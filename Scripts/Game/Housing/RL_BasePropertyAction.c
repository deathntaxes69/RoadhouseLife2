// Base action class to reduce code duplication across housing actions
[BaseContainerProps]
class RL_BasePropertyAction : ScriptedUserAction
{
	protected IEntity m_ownerEntity;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerEntity = pOwnerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	protected RL_HouseComponent FindHouseComponent()
	{
		IEntity houseManager = GetOwner();
		if (!houseManager)
			return null;
		
		IEntity parent = houseManager.GetParent();
		if (!parent)
			return null;

		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
		return houseComponent;
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_ChimeraCharacter GetCharacterFromUser(IEntity user)
	{
		return SCR_ChimeraCharacter.Cast(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_ChimeraCharacter GetValidatedCharacter(IEntity user, out string reason = "")
	{
		SCR_ChimeraCharacter character = GetCharacterFromUser(user);
		if (!character)
		{
			reason = "Invalid character";
			return null;
		}
		
		if (!RL_Utils.CanPerformAction(user))
		{
			reason = "Character cannot perform actions";
			return null;
		}
		
		return character;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsValidCharacter(IEntity user, out string reason = "")
	{
		return GetValidatedCharacter(user, reason) != null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsValidHouseComponent(out string reason = "")
	{
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
		{
			reason = "House component not found";
			return false;
		}
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsOwner(IEntity user, out string reason = "")
	{
		SCR_ChimeraCharacter character = GetValidatedCharacter(user, reason);
		if (!character)
			return false;
		
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
		{
			reason = "House component not found";
			return false;
		}
		
		string characterId = character.GetCharacterId();
		if (!characterId)
		{
			reason = "Invalid character ID";
			return false;
		}
		
		if (!houseComponent.IsPlayerOwner(characterId.ToInt()))
		{
			reason = "You are not the owner of this house";
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsHouseOwned(out string reason = "")
	{
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
		{
			reason = "House component not found";
			return false;
		}
		
		if (!houseComponent.IsOwned())
		{
			reason = "House is not owned";
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsHouseNotOwned(out string reason = "")
	{
		RL_HouseComponent houseComponent = FindHouseComponent();
		if (!houseComponent)
		{
			reason = "House component not found";
			return false;
		}
		
		if (houseComponent.IsOwned())
		{
			reason = "House is already owned";
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool HasSufficientFunds(IEntity user, int requiredAmount, out string reason = "")
	{
		SCR_ChimeraCharacter character = GetValidatedCharacter(user, reason);
		if (!character)
			return false;
		
		int playerBank = character.GetBank();
		if (playerBank < requiredAmount)
		{
			reason = string.Format("Insufficient bank funds. Need %1, have %2", 
				RL_Utils.FormatMoney(requiredAmount), RL_Utils.FormatMoney(playerBank));
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ValidateBaseConditions(IEntity user, out string reason = "")
	{
		return IsValidCharacter(user, reason) && IsValidHouseComponent(reason);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		string reason;
		if (!ValidateBaseConditions(user, reason))
		{
			SetCannotPerformReason(reason);
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		string reason;
		return ValidateBaseConditions(user, reason);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
} 