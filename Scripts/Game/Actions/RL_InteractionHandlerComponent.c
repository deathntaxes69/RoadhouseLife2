
modded class SCR_InteractionHandlerComponent
{
	//Disable actions when handcuffed
	protected override bool GetCanInteractScript(IEntity controlledEntity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(controlledEntity);
		if (character && character.IsRestrained())
			return false;

		return super.GetCanInteractScript(controlledEntity);
	}
	// There is only a few lines changed here to call CustomDoPerformObjectAction
	// instead of DoPerformObjectAction since we cant override it
	// SEE "START CUSTOM" to "END CUSTOM"
	override protected void DoProcessInteraction(
		ChimeraCharacter user,
		UserActionContext context,
		BaseUserAction action,
		bool canPerform,
		bool performInput,
		float timeSlice,
		SCR_BaseInteractionDisplay display)
	{
		if (action)
			action.SetActiveContext(context);

		// Can action be performed?
		bool isOk = action && canPerform && action == m_pLastUserAction;
		// We want to perform and action is OK
		if (performInput && isOk)
		{
			// We want to be performing, but we're not yet.
			// Start the action and dispatch events.
			if (!m_bIsPerforming && !m_bLastInput)
			{
				if (!GetCanInteractScript(user))
					return;

				// UI
				if (display)
					display.OnActionStart(user, action);

				// Start the action. Calls action.OnActionStart
				user.DoStartObjectAction(action);

				// Set state
				m_bIsPerforming = true;
				m_fCurrentProgress = 0.0;
			}
			// We want to perform and we already started performing,
			// update continuous handler state until we're finished
			else if (m_bIsPerforming)
			{
				if (DiagMenu.GetValue(SCR_DebugMenuID.DEBUGUI_INTERACTION_SKIP_DURATION))
				{
					timeSlice += Math.AbsFloat(action.GetActionDuration());

					SCR_ScriptedUserAction scrAction = SCR_ScriptedUserAction.Cast(action);
					if (scrAction)
						timeSlice += scrAction.GetLoopActionHoldDuration();
				}

				// Update elapsed time
				m_fCurrentProgress = action.GetActionProgress(m_fCurrentProgress, timeSlice);

				// Tick action
				if (action.ShouldPerformPerFrame())
					user.DoPerformContinuousObjectAction(action, timeSlice);

				// Get action duration
				float duration = action.GetActionDuration();
				
				// Update UI
				if (display)
					display.OnActionProgress(user, action, m_fCurrentProgress, Math.AbsFloat(duration));
				
				// We are finished, dispatch events and reset state
				// TODO: Why are some actions set with negative duration? Why does using an abs duration check here causes those actions to break? Why Is this not using a proper event system from the actions themselves?!
				if (m_fCurrentProgress >= duration && duration >= 0)
				{
					// Update UI
					if (display)
						display.OnActionFinish(user, action, ActionFinishReason.FINISHED);

					// Finally perform action
					if (!action.ShouldPerformPerFrame())
					{
						//START CUSTOM PART
						SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
						//Print(character);
						if(character)
							character.CustomDoPerformObjectAction(action);
						else
							user.DoPerformObjectAction(action);
						//END CUSTOM PART
					}
					// Reset state
					m_fCurrentProgress = 0.0;
					m_bIsPerforming = false;
				}
			}
		}
		else
		{
			// Input was released, we were performing previously,
			// stop performing and dispatch necessary events.
			if (m_bIsPerforming)
			{
				// Update UI
				if (display)
					display.OnActionFinish(user, action, ActionFinishReason.INTERRUPTED);

				// Cancel the action. Calls action.OnActionCanceled
				user.DoCancelObjectAction(action);

				// Reset state
				m_bIsPerforming = false;
				m_fCurrentProgress = 0.0;
			}
		}
	}
}