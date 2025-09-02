class RL_CameraSettingsHelper
{
	static bool LoadCustomCameraState()
	{
		BaseContainer settings = GetGame().GetGameUserSettings().GetModule("RL_CameraSettings");
		if (settings)
		{
			bool enabled = false;
			settings.Get("UseCustomCamera", enabled);
			return enabled;
		}
		return false;
	}

	static void SaveCustomCameraState(bool state)
	{
		BaseContainer settings = GetGame().GetGameUserSettings().GetModule("RL_CameraSettings");
		if (settings)
		{
			settings.Set("UseCustomCamera", state);
			GetGame().UserSettingsChanged();
			GetGame().SaveUserSettings();
		}
	}
}
