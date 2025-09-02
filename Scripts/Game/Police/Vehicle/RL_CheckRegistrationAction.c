class RL_CheckRegistrationAction : ScriptedUserAction
{

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!pOwnerEntity || RL_Utils.IsDedicatedServer()) return;

        RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(pOwnerEntity.FindComponent(RL_VehicleManagerComponent));
        if(!vehicleManagerComponent) return;

		string ownerName = vehicleManagerComponent.GetOwnerName();
        RL_Utils.Notify("Vehicle Owner: "+ownerName, "REGISTRATION");	

	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Check Registration";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		
        SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		
		return userCharacter && userCharacter.IsPolice();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
	}
    override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
}