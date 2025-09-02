modded class SCR_VONController {

  override void DeactivateVON(EVONTransmitType transmitType = EVONTransmitType.NONE) {
    super.DeactivateVON(transmitType);
  }
  override void SetVONProximityToggle(bool activate) {
    if (!m_VONComp)
      return;

    if (!m_DirectSpeechEntry || !m_DirectSpeechEntry.IsUsable())
      return;

    if (m_bIsToggledDirect == activate)
      return;

    m_bIsToggledDirect = activate;

    if (activate)
      ActivateVON(EVONTransmitType.DIRECT);
    else
      DeactivateVON(EVONTransmitType.DIRECT);

    m_OnVONActiveToggled.Invoke(m_bIsToggledDirect, false);
  }
}