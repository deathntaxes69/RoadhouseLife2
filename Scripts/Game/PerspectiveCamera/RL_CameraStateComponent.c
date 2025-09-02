[ComponentEditorProps(category: "GameScripted", description: "Controls custom camera toggle")]
class RL_CameraStateComponentClass : ScriptComponentClass {}

class RL_CameraStateComponent : ScriptComponent
{
	protected bool m_bCustomCameraEnabled = false;

	void SetCustomCameraActive(bool state)
	{
		if (m_bCustomCameraEnabled == state)
			return;

		m_bCustomCameraEnabled = state;
	}

	bool IsCustomCameraActive()
	{
		return m_bCustomCameraEnabled;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		bool saved = RL_CameraSettingsHelper.LoadCustomCameraState();
		SetCustomCameraActive(saved);
	}
}
