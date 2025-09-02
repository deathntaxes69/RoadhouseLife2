class RL_PullOutVehicle : ScriptedUserAction
{
	SCR_BaseCompartmentManagerComponent compartmentManager;
			
	
	//------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
		compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_BaseCompartmentManagerComponent));
		
    }
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
				
		SCR_BaseCompartmentManagerComponent compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_BaseCompartmentManagerComponent));
		array<IEntity> occupants = {}; 
		compartmentManager.GetOccupants(occupants);
		foreach (IEntity occupant : occupants)
		{
			Print("[RL_PullOutVehicle] PerformAction foreach");
            SCR_ChimeraCharacter occupantCharacter = SCR_ChimeraCharacter.Cast(occupant);
            
            if(occupantCharacter)
                occupantCharacter.GetOutVehicle();
		}
        return;
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Pull out from Vehicle";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		array<IEntity> occupants = {};
		compartmentManager.GetOccupants(occupants);
		
        SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		
		return !occupants.IsEmpty() && userCharacter && userCharacter.IsPolice();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
	}
}