[BaseContainerProps]
class RL_ControlZoneAction : ScriptedUserAction
{
	[Attribute("1", desc: "Zone ID this action is for (must match RL_ControlZoneEntity zoneId)")]
	protected int m_iZoneId;
	
	protected RL_ControlZoneEntity m_controlZone;
	
	//------------------------------------------------------------------------------------------------
	RL_ControlZoneEntity GetControlZone()
	{
		if (!m_controlZone)
		{
			RL_ControlZoneManager manager = RL_ControlZoneManager.GetInstance();
			if (manager)
				m_controlZone = manager.FindControlZoneById(m_iZoneId);
		}
		return m_controlZone;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
		{
			return;
		}
		
		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(pUserEntity);
		if (!characterController || !characterController.GetWeaponManagerComponent().GetCurrentWeapon())
		{
			character.Notify("Requires weapon in hands.", "CONTROL ZONE");
			return;
		}
		
		RL_ControlZoneEntity controlZone = GetControlZone();
		if (!controlZone)
		{
			character.Notify("Control zone not found.", "CONTROL ZONE");
			return;
		}
		
		if (controlZone.GetZoneId() != m_iZoneId)
		{
			character.Notify("Zone configuration error.", "CONTROL ZONE");
			return;
		}
		
		int playerGangId = character.GetGangId();
		
		if (playerGangId == -1)
		{
			character.Notify("You must be in a gang to capture control zones.", "CONTROL ZONE");
			return;
		}
		
		if (controlZone.IsCaptureInProgress())
		{
			character.Notify("Zone capture is already in progress.", "CONTROL ZONE");
			return;
		}
		
		if (controlZone.GetControllingGangId() == playerGangId)
		{
			character.Notify("Your gang already controls this zone.", "CONTROL ZONE");
			return;
		}
		
		if (!controlZone.CanStartCapture(character))
		{
			character.Notify("Cannot start capture right now.", "CONTROL ZONE");
			return;
		}
		
		controlZone.StartCapture(playerGangId, character);
		character.NotifyGangMembers(controlZone.GetControllingGangId(), "Cartel base is under attack from a rival gang.");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_ControlZoneEntity controlZone = GetControlZone();
		if (controlZone)
		{
			if (controlZone.IsCaptureInProgress())
			{
				outName = string.Format("Capture %1 (In Progress)", controlZone.GetZoneName());
			}
			else
			{
				outName = string.Format("Capture %1", controlZone.GetZoneName());
			}
		}
		else
		{
			outName = "Capture Control Zone";
		}
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
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
		{
			SetCannotPerformReason("Must be a character");
			return false;
		}

		RL_ControlZoneEntity controlZone = GetControlZone();
		if (!controlZone)
		{
			SetCannotPerformReason("Control zone not found");
			return false;
		}

		if (!RL_Utils.CanPerformAction(user))
		{
			SetCannotPerformReason("Cannot perform action right now");
			return false;
		}

		if (character.GetGangId() == -1)
		{
			SetCannotPerformReason("Must be in a gang");
			return false;
		}

		if (controlZone.IsCaptureInProgress())
		{
			SetCannotPerformReason("Capture already in progress");
			return false;
		}

		if (controlZone.GetControllingGangId() == character.GetGangId())
		{
			SetCannotPerformReason("Your gang already controls this zone");
			return false;
		}

		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(user);
		if (!characterController || !characterController.GetWeaponManagerComponent().GetCurrentWeapon())
		{
			SetCannotPerformReason("Requires weapon in hands");
			return false;
		}

		return true;
	}
	
	override void SetCannotPerformReason(string reason)
	{
		m_sCannotPerformReason = reason;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
} 