modded class SCR_PlayerController
{
	protected RL_CameraStateComponent m_pStateComponent;

	void ToggleCustomCamera()
	{
		IEntity player = GetControlledEntity();
		if (!player)
			return;

		m_pStateComponent = RL_CameraStateComponent.Cast(player.FindComponent(RL_CameraStateComponent));
		if (!m_pStateComponent)
			return;

		bool newState = !m_pStateComponent.IsCustomCameraActive();
		m_pStateComponent.SetCustomCameraActive(newState);

		SCR_CharacterCameraHandlerComponent camHandler = SCR_CharacterCameraHandlerComponent.Cast(player.FindComponent(SCR_CharacterCameraHandlerComponent));
		if (!camHandler)
			return;

		bool currentView = camHandler.IsInThirdPerson();
		camHandler.SetThirdPerson(!currentView);

		GetGame().GetCallqueue().CallLaterByName(this, "RestoreCameraState", 10, false, camHandler, currentView);
	}

	void RestoreCameraState(SCR_CharacterCameraHandlerComponent camHandler, bool viewState)
	{
		if (!camHandler)
			return;

		camHandler.SetThirdPerson(viewState);
	}
}
