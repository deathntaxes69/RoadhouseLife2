class RL_VoiceDisplay: SCR_InfoDisplay {
  protected ImageWidget m_wVoiceIcon;
  protected SCR_FadeUIComponent m_FadeComponent;
  protected ResourceName m_ImagesetResource = "{E5158F41596A4EE2}Assets/VoiceRange/RL_VoiceLevels.imageset";
  protected Resource m_PersistentImageset;
  protected bool m_ImagesetLoadAttempted = false;

  protected TextWidget m_wWhisperingRange;
  protected TextWidget m_wNormalRange;
  protected TextWidget m_wYellingRange;

  protected SCR_FadeUIComponent m_FadeWhispering;
  protected SCR_FadeUIComponent m_FadeNormal;
  protected SCR_FadeUIComponent m_FadeYelling;

  static ref ScriptInvokerVoid OnVoiceStateChangeInvoker = new ScriptInvokerVoid();

  static ScriptInvokerVoid GetOnVoiceStateChangeInvoker() {
    return OnVoiceStateChangeInvoker;
  }

  protected override event void OnStartDraw(IEntity owner) {
    super.OnStartDraw(owner);

    if (!m_wRoot) {
      m_wRoot = GetGame().GetWorkspace().CreateWidgets("{C01CA60342EBDE31}UI/Layouts/VoiceRange/RL_OnRangeChange.layout");
      if (!m_wRoot)
        return;
    }

    m_wVoiceIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("VoiceStateIcon"));
    if (!m_wVoiceIcon)
      return;

    m_wWhisperingRange = TextWidget.Cast(m_wRoot.FindAnyWidget("5m"));
    m_wNormalRange = TextWidget.Cast(m_wRoot.FindAnyWidget("25m"));
    m_wYellingRange = TextWidget.Cast(m_wRoot.FindAnyWidget("50m"));

    m_FadeComponent = SCR_FadeUIComponent.Cast(m_wVoiceIcon.FindHandler(SCR_FadeUIComponent));

    m_FadeWhispering = SCR_FadeUIComponent.Cast(m_wWhisperingRange.FindHandler(SCR_FadeUIComponent));
    m_FadeNormal = SCR_FadeUIComponent.Cast(m_wNormalRange.FindHandler(SCR_FadeUIComponent));
    m_FadeYelling = SCR_FadeUIComponent.Cast(m_wYellingRange.FindHandler(SCR_FadeUIComponent));

    if (!m_FadeComponent)
      return;

    m_wVoiceIcon.SetVisible(false);
    HideAllRangeWidgetsWithFade();

    RL_VoiceDisplay.GetOnVoiceStateChangeInvoker().Insert(UpdateStateIcon);
  }

  void UpdateStateIcon() {
    RL_VoiceComponent voiceComponent = RL_VoiceComponent.GetInstance();
    if (!voiceComponent)
      return;

    string iconName = "";
    TextWidget activeRangeWidget = null;
    SCR_FadeUIComponent activeFadeComponent = null;

    switch (voiceComponent.GetState()) {
    case RL_eVoiceState.Whispering:
      iconName = "whispering";
      activeRangeWidget = m_wWhisperingRange;
      activeFadeComponent = m_FadeWhispering;
      break;
    case RL_eVoiceState.Normal:
      iconName = "normal";
      activeRangeWidget = m_wNormalRange;
      activeFadeComponent = m_FadeNormal;
      break;
    case RL_eVoiceState.Yell:
      iconName = "yelling";
      activeRangeWidget = m_wYellingRange;
      activeFadeComponent = m_FadeYelling;
      break;
    }

    UpdateIcon(iconName, activeRangeWidget, activeFadeComponent);
  }

  protected void UpdateIcon(string iconName, TextWidget activeRangeWidget, SCR_FadeUIComponent activeFadeComponent) {
    if (iconName != "") {
      m_wVoiceIcon.LoadImageFromSet(0, m_ImagesetResource, iconName);
      m_wVoiceIcon.SetVisible(true);
      m_FadeComponent.FadeIn(true);

      ShowRangeWidget(activeRangeWidget, activeFadeComponent);

      GetGame().GetCallqueue().Remove(StartFadeOut);
      GetGame().GetCallqueue().CallLater(StartFadeOut, 3000, false);
    }
  }

  protected void StartFadeOut() {
    if (m_FadeComponent)
      m_FadeComponent.FadeOut(false);

    HideAllRangeWidgetsWithFade();
  }

  protected void ShowRangeWidget(TextWidget widget, SCR_FadeUIComponent fadeComponent) {
    HideAllRangeWidgetsWithFade();
    if (fadeComponent)
      fadeComponent.FadeIn(true);
  }

  protected void HideAllRangeWidgetsWithFade() {
    FadeOutWidget(m_FadeWhispering);
    FadeOutWidget(m_FadeNormal);
    FadeOutWidget(m_FadeYelling);
  }

  protected void FadeOutWidget(SCR_FadeUIComponent fadeComponent) {
    if (!fadeComponent)
      return;

    fadeComponent.FadeOut(false);
  }
}