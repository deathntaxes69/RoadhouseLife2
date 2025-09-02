class RL_CameraViewToggleWidgetComponent : SCR_ScriptedWidgetComponent
{
	protected int m_iLastToggleTime;
	protected const int TOGGLE_COOLDOWN_MS = 250;
	protected bool m_bLastState = false;

	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		ButtonWidget viewButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("View"));
		if (viewButton)
			viewButton.AddHandler(this);

		ButtonWidget resetButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ResetView"));
		if (resetButton)
			resetButton.AddHandler(this);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int currentTime = System.GetTickCount();
		if (currentTime - m_iLastToggleTime < TOGGLE_COOLDOWN_MS)
			return true;

		m_iLastToggleTime = currentTime;

		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!pc) return false;

		IEntity player = pc.GetControlledEntity();
		if (!player) return false;

		RL_CameraStateComponent cameraState = RL_CameraStateComponent.Cast(player.FindComponent(RL_CameraStateComponent));
		if (!cameraState) return false;

		if (w.GetName() == "View")
		{
			m_bLastState = !m_bLastState;
			cameraState.SetCustomCameraActive(m_bLastState);
			RL_CameraSettingsHelper.SaveCustomCameraState(m_bLastState);
		}
		else if (w.GetName() == "ResetView")
		{
			m_bLastState = false;
			cameraState.SetCustomCameraActive(false);
			RL_CameraSettingsHelper.SaveCustomCameraState(false);
		}

		SCR_CharacterCameraHandlerComponent camHandler = SCR_CharacterCameraHandlerComponent.Cast(player.FindComponent(SCR_CharacterCameraHandlerComponent));
		if (camHandler)
		{
			camHandler.SetThirdPerson(false);
			GetGame().GetCallqueue().CallLaterByName(this, "ForceToThird", 500, false, camHandler);
		}

		return true;
	}

	void ForceToThird(SCR_CharacterCameraHandlerComponent camHandler)
	{
		if (!camHandler)
			return;

		camHandler.SetThirdPerson(true);
	}
}
