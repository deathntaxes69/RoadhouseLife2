/*
modded class SCR_VONEntryRadio
{
    override bool IsUsable()
	{
        SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
        SCR_VONController vonController = SCR_VONController.Cast(GetGame().GetPlayerController().FindComponent(SCR_VONController));
        if(character && vonController && vonController.GetPhoneEntry() == this && !character.GetPhoneStatus() )
            return false;
		
        return true;
	}
 
}
*/