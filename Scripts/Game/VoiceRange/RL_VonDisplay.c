modded class SCR_VonDisplay {
  override void UpdateWidgets() {
    int count = m_aWidgetsIncomingVON.Count();

    if (count == m_iTransmissionSlots)
      return;

    if (count > m_iTransmissionSlots) {
      for (int i = count - 1; i > m_iTransmissionSlots; i--) {
        //Print("[RL_DroppedCashCharacterController] DoorCloseCheck for");
        Widget w = m_aWidgetsIncomingVON.Get(i);
        if (w) {
          w.RemoveFromHierarchy();
          m_aWidgetsIncomingVON.RemoveItem(w);
        }
      }
    } else {
      for (int i = count; i < m_iTransmissionSlots; i++) {
        //Print("[RL_DroppedCashCharacterController] DoorCloseCheck for 2");
        Widget w = GetGame().GetWorkspace().CreateWidgets(m_sReceivingTransmissionLayout, m_wVerticalLayout);
        if (w) {
          w.SetVisible(false);
          m_aWidgetsIncomingVON.Insert(w);
        }
      }
    }
  }

  //------------------------------------------------------------------------------------------------
  override protected bool UpdateTransmission(TransmissionData data, BaseTransceiver radioTransceiver, int frequency, bool IsReceiving)
  {
    bool result = super.UpdateTransmission(data, radioTransceiver, frequency, IsReceiving);
    if (result && IsReceiving && data.m_Widgets && data.m_Widgets.m_wName)
    {
      string currentText = data.m_Widgets.m_wName.GetText();
      if (currentText != LABEL_UNKNOWN_SOURCE)
      {
        data.m_Entity = GetGame().GetPlayerManager().GetPlayerControlledEntity(data.m_iPlayerID);
        if (data.m_Entity)
        {
          SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(data.m_Entity);
          if (character)
          {
            string characterId = character.GetCharacterId();
            if (characterId && characterId != "-1" && !characterId.IsEmpty())
            {
              data.m_Widgets.m_wName.SetText(characterId);
            }
          }
        }
      }
    }
    
    return result;
  }
}