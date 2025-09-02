modded class SCR_CharacterControllerComponent
{
    
    override void OnPrepareControls(IEntity owner, ActionManager am, float dt, bool player)
	{

        if( GetCharacter().IsRestrained() || GetCharacter().IsSurendering())
        {
            GetGame().GetInputManager().ActivateContext("RL_RestrainedContext");
            
            // Slow movement
            am.SetActionValue("CharacterForward", (am.GetActionValue("CharacterForward"))*0.3);
            am.SetActionValue("CharacterRight", (am.GetActionValue("CharacterRight"))*0.3);
            am.SetActionValue("CharacterAimLeft", (am.GetActionValue("CharacterAimLeft"))*0.1);
            am.SetActionValue("CharacterAimRight", (am.GetActionValue("CharacterAimRight"))*0.1);
        }
		super.OnPrepareControls(owner, am, dt, player);
	}

}