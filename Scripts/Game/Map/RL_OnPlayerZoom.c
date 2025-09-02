class RL_MapPlayerIndicator: SCR_MapUIBaseComponent {
  protected SCR_MapToolEntry m_mapToolEntry;

  override void Init() {
    SCR_MapToolMenuUI toolMenu = SCR_MapToolMenuUI.Cast(m_MapEntity.GetMapUIComponent(SCR_MapToolMenuUI));
    if (toolMenu) {
      m_mapToolEntry = toolMenu.RegisterToolMenuEntry("{1872FFA1133724A2}UI/Textures/Chat/chat.imageset", "system", 99);
      m_mapToolEntry.m_OnClick.Insert(CenterMapOnPlayer);
    }
  }

  void CenterMapOnPlayer() {
    ChimeraCharacter playerCharacter = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
    if (!playerCharacter)
      return;

    vector playerPosition = playerCharacter.GetOrigin();
    vector mapDimensions = m_MapEntity.GetMapWidget().GetSizeInUnits();
    float baseZoom = mapDimensions[0] / (mapDimensions[0] * m_MapEntity.GetMapWidget().PixelPerUnit());
	float zoomFactor = baseZoom * 0.15;

    m_MapEntity.ZoomPanSmooth(zoomFactor, playerPosition[0], playerPosition[2]);
  }
}