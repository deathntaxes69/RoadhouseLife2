enum RL_eVoiceState {
  Whispering,
  Normal,
  Yell
}

[ComponentEditorProps(category: "GameScripted/", description: "Modular Voice Mod")]
class RL_VoiceComponentClass: ScriptComponentClass {}

class RL_VoiceComponent: ScriptComponent {
  protected RL_eVoiceState m_eVoiceState = RL_eVoiceState.Normal;
  ref array < typename > m_vonComps = {
    RL_VoNWhispering,
    RL_VoNNormal,
    RL_VoNLoud
  };

  protected int currentVolume = 0;

  static RL_VoiceComponent GetInstance() {
    return RL_VoiceComponent.Cast(GetGame().GetPlayerController().FindComponent(RL_VoiceComponent));
  }

  void RL_VoiceComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
    SetEventMask(ent, EntityEvent.INIT);
  }

  protected override void EOnInit(IEntity owner) {
    GetGame().GetInputManager().AddActionListener("RL_VoiceVolumeCycle", EActionTrigger.DOWN, VolumeCyle);
  }

  void VolumeCyle()
  {
    currentVolume = currentVolume + 1;
    
    if(currentVolume >= m_vonComps.Count()-1)
        currentVolume = 0;
    
    Print("VolumeCyle");
    Print(currentVolume);
    VolumeChange(currentVolume);
  }

  void VolumeChange(int value) {
    int currentState = EnumToInt(m_eVoiceState);
    int newState = (currentState + value) % 3;
    if (newState < 0)
      newState = 2;

    if (currentState == newState)
      return;

    m_eVoiceState = IntToEnum(newState);

    IEntity player = GetGame().GetPlayerController().GetControlledEntity();
    if (!player)
      return;

    SCR_VONController vonContr = SCR_VONController.Cast(GetOwner().FindComponent(SCR_VONController));
    if (!vonContr)
      return;

    SCR_VoNComponent currentVonComp = vonContr.GetVONComponent();
    if (currentVonComp) {
      vonContr.DeactivateVON();
      vonContr.SetVONProximityToggle(false);
    }

    SCR_VoNComponent newVonComp = SCR_VoNComponent.Cast(player.FindComponent(m_vonComps[newState]));
    if (!newVonComp)
      return;

    vonContr.SetVONComponent(newVonComp);

    SCR_HUDManagerComponent hud = SCR_HUDManagerComponent.Cast(GetOwner().FindComponent(SCR_HUDManagerComponent));
    if (!hud)
      return;

    RL_VoiceDisplay voiceDisplay = RL_VoiceDisplay.Cast(hud.FindInfoDisplay(RL_VoiceDisplay));
    if (voiceDisplay)
      voiceDisplay.UpdateStateIcon();

    UpdateDisplayAfterNewCompIsSet();
  }

  RL_eVoiceState GetState() {
    return m_eVoiceState;
  }

  int EnumToInt(RL_eVoiceState state) {
    switch (state) {
    case RL_eVoiceState.Whispering:
      return 0;
    case RL_eVoiceState.Normal:
      return 1;
    case RL_eVoiceState.Yell:
      return 2;
    }
    return 1;
  }

  RL_eVoiceState IntToEnum(int state) {
    switch (state) {
    case 0:
      return RL_eVoiceState.Whispering;
    case 1:
      return RL_eVoiceState.Normal;
    case 2:
      return RL_eVoiceState.Yell;
    }
    return RL_eVoiceState.Normal;
  }

  void UpdateDisplayAfterNewCompIsSet() {
    SCR_HUDManagerComponent hud = SCR_HUDManagerComponent.Cast(GetOwner().FindComponent(SCR_HUDManagerComponent));
    IEntity player = GetGame().GetPlayerController().GetControlledEntity();
    SCR_VonDisplay vonDisplay = SCR_VonDisplay.Cast(hud.FindInfoDisplay(SCR_VonDisplay));

    if (!vonDisplay)
      return;

    vonDisplay.UpdateWidgets();
  }
}