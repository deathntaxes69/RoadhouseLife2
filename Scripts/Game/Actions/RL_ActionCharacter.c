


modded class SCR_ChimeraCharacter
{
    //protected float m_fProgressStartTime = 0;
    // Intercept calls to DoPerformObjectAction
    // if action is intended to have progress bar
    void CustomDoPerformObjectAction(BaseUserAction pAction)
	{
		//Print("--- CustomDoPerformObjectAction");
		RL_ProgressBarAction castedAction = RL_ProgressBarAction.Cast(pAction);

		//Do orginal workflow if it didnt cast
		if(!castedAction)
        {
            DoPerformObjectAction(pAction);
            return;
        }
        
        Widget progressBar = castedAction.StartProgressBar(this);
        if(!progressBar) return;
        protected float progressStartTime = GetWorld().GetTimestamp().DiffMilliseconds(null);
        GetGame().GetCallqueue().CallLater(ProgressLoop, 500, false, castedAction, progressStartTime, this.GetOrigin());

	}
    void ProgressLoop(RL_ProgressBarAction action, float startTime, vector startPos)
    { 

        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
        vector currentPos = this.GetOrigin();
        float distance = vector.Distance(startPos, currentPos);
        float maxMoveDistance = 3.0;
        if(action) maxMoveDistance = action.GetMaxMoveDistance();
        if(distance > maxMoveDistance || !RL_Utils.CanPerformAction(this) || character.IsInVehicle())
		{
            action.EndProgressBar();
            RL_Utils.Notify("Canceled", "ACTION");
            return;
        }


        protected float currentTime = GetWorld().GetTimestamp().DiffMilliseconds(null);
        if((currentTime - startTime) >= (action.GetProgressTime()))
        {
            action.EndProgressBar();
            DoPerformObjectAction(action);
            return;
        }
        GetGame().GetCallqueue().CallLater(ProgressLoop, 500, false, action, startTime, startPos);



    }
}