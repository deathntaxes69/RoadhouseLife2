class RL_EarplugToggleDisplay : SCR_InfoDisplay {
	protected ImageWidget m_wEarplug;

	protected const float EARPLUG_VOLUME = 0.25;
	protected const float NORMAL_VOLUME = 1.0;

	protected ResourceName m_ImagesetResource = "{435F8EFC9F1F1C11}UI/Layouts/Earplugs/Earplug_Stages.imageset";
	protected Resource m_PersistentImageset;
	protected bool m_ImagesetLoadAttempted = false;
	protected bool m_ListenerAdded = false;

	protected SCR_ChimeraCharacter m_character;

	//------------------------------------------------------------------------------------------------
	protected override event void OnStartDraw(IEntity owner) {
		super.OnStartDraw(owner);

		EnsureImagesetLoaded();

		if (!m_wRoot) {
			m_wRoot = GetGame().GetWorkspace().CreateWidgets("{58C540F9E2D12E72}UI/Layouts/HUD/HUD.layout");
			if (!m_wRoot) return;
		}

		m_wEarplug = ImageWidget.Cast(m_wRoot.FindAnyWidget("Earplug"));
		if (m_wEarplug) m_wEarplug.SetVisible(false);

		if (!m_ListenerAdded) {
			InputManager inputManager = GetGame().GetInputManager();
			if (inputManager) {
				inputManager.AddActionListener("ToggleEarplugs", EActionTrigger.DOWN, OnToggleEarplugs);
				m_ListenerAdded = true;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void OnToggleEarplugs() {
		if(!m_character)
			m_character = RL_Utils.GetLocalCharacter();

		m_character.ToggleEarplugs();

		if (m_character.IsEarplugged()) {
			AudioSystem.SetMasterVolume(AudioSystem.SFX, EARPLUG_VOLUME);
			UpdateEarplugIcon("50");
			SetVisibility(true);
		} else {
			AudioSystem.SetMasterVolume(AudioSystem.SFX, NORMAL_VOLUME);
			UpdateEarplugIcon("");
			SetVisibility(false);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void SetVisibility(bool state) {
		if (m_wEarplug) m_wEarplug.SetVisible(state);
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateEarplugIcon(string iconName) {
		EnsureImagesetLoaded();
		if (m_wEarplug) {
			if (iconName != "") {
				m_wEarplug.LoadImageFromSet(0, m_ImagesetResource, iconName);
				m_wEarplug.SetVisible(true);
			} else {
				m_wEarplug.SetVisible(false);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void EnsureImagesetLoaded() {
		if (!m_ImagesetLoadAttempted && (!m_PersistentImageset || !m_PersistentImageset.IsValid())) {
			m_PersistentImageset = Resource.Load(m_ImagesetResource);
			m_ImagesetLoadAttempted = true;
		}
	}

	//------------------------------------------------------------------------------------------------
	protected override event void OnStopDraw(IEntity owner) {
		super.OnStopDraw(owner);
	}
}
