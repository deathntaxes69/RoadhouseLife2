modded class CharacterCamera3rdPersonErc : CharacterCamera3rdPersonBase
{
	override void OnActivate(ScriptedCameraItem pPrevCamera, ScriptedCameraItemResult pPrevCameraResult)
	{
		super.OnActivate(pPrevCamera, pPrevCameraResult);

		RL_CameraStateComponent state = RL_CameraStateComponent.Cast(m_OwnerCharacter.FindComponent(RL_CameraStateComponent));
		if (!state || !state.IsCustomCameraActive())
			return;

		m_fDistance = 0.6;
		m_CameraOffsetMS = "0.075 1.5 0.0";
		m_CameraOffsetLS = "0.0 0.0 0.0";
		m_fShoulderWidth = 0.33;
		m_fLeanDistance = 0;
	}
}

modded class CharacterCamera3rdPersonCrouch : CharacterCamera3rdPersonBase
{
	override void OnActivate(ScriptedCameraItem pPrevCamera, ScriptedCameraItemResult pPrevCameraResult)
	{
		super.OnActivate(pPrevCamera, pPrevCameraResult);

		RL_CameraStateComponent state = RL_CameraStateComponent.Cast(m_OwnerCharacter.FindComponent(RL_CameraStateComponent));
		if (!state || !state.IsCustomCameraActive())
			return;

		m_fDistance = 0.65;
		m_CameraOffsetMS = "0.0 0.0 0.0";
		m_CameraOffsetLS = "0.0 0.45 0.0";
		m_fShoulderWidth = 0.4;
		m_fLeanDistance = 0.4;

		CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(m_CharacterAnimationComponent.FindComponent(CharacterCommandHandlerComponent));
		m_CommandMove = cmdHandler.GetCommandMove();
	}
}

modded class CharacterCamera3rdPersonProne : CharacterCamera3rdPersonBase
{
	override void OnActivate(ScriptedCameraItem pPrevCamera, ScriptedCameraItemResult pPrevCameraResult)
	{
		super.OnActivate(pPrevCamera, pPrevCameraResult);

		RL_CameraStateComponent state = RL_CameraStateComponent.Cast(m_OwnerCharacter.FindComponent(RL_CameraStateComponent));
		if (!state || !state.IsCustomCameraActive())
			return;

		m_fDistance = 0.2;
		m_CameraOffsetMS = "0.0 0.5 0.0";
		m_CameraOffsetLS = "0.0 0.0 0.0";
		m_fShoulderWidth = 0.5;
	}
}

modded class CharacterCamera3rdPersonVehicle : CharacterCamera3rdPersonVehicle
{
	override void InitCameraData()
	{
		super.InitCameraData();

		RL_CameraStateComponent state = RL_CameraStateComponent.Cast(m_OwnerCharacter.FindComponent(RL_CameraStateComponent));
		if (!state || !state.IsCustomCameraActive())
			return;

		m_fDist_Desired = 3;
		m_fDist_Min = 2;
		m_fDist_Max = 4;
		m_fHeight = 0.7;
	}
}
