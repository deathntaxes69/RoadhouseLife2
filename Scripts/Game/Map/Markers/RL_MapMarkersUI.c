modded class SCR_MapMarkersUI
{
	override protected void CreateMarkerEditDialog(bool isEditing = false, int tabID = 0, int selectedIconEntry = -1, int selectedColorEntry = -1)
	{
	}

	override protected void CreateMilitaryMarkerEditDialog(bool isEditing = false, int selectedFactionEntry = -1, int selectedDimensionEntry = -1)
	{
	}

	override protected void OnRadialMenuOpen(SCR_RadialMenuController controller)
	{
	}

	override protected void OnInputQuickMarkerMenu(float value, EActionTrigger reason)
	{
	}

	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);

		CreateStaticMarkers();
		CreateDynamicMarkers();

		m_MarkerMgr.EnableUpdate(true);		// run frame update manager side

		GetGame().GetInputManager().AddActionListener("MapMarkerDelete", EActionTrigger.DOWN, OnInputMarkerDelete);
		GetGame().GetInputManager().AddActionListener("MapSelect", EActionTrigger.DOWN, OnInputMapSelect);
		GetGame().GetInputManager().AddActionListener(UIConstants.MENU_ACTION_SELECT, EActionTrigger.DOWN, OnInputMenuConfirm);
		GetGame().GetInputManager().AddActionListener("MenuRefresh", EActionTrigger.DOWN, OnInputMenuConfirmAlter);
		GetGame().GetInputManager().AddActionListener(UIConstants.MENU_ACTION_BACK, EActionTrigger.DOWN, OnInputMenuBack);
		GetGame().GetInputManager().AddActionListener(UIConstants.MENU_ACTION_DOWN, EActionTrigger.DOWN, OnInputMenuDown);
		GetGame().GetInputManager().AddActionListener(UIConstants.MENU_ACTION_UP, EActionTrigger.DOWN, OnInputMenuUp);
		GetGame().GetInputManager().AddActionListener(UIConstants.MENU_ACTION_RIGHT, EActionTrigger.DOWN, OnInputMenuRight);
		GetGame().GetInputManager().AddActionListener(UIConstants.MENU_ACTION_LEFT, EActionTrigger.DOWN, OnInputMenuLeft);

		if (SCR_MapToolInteractionUI.Cast(m_MapEntity.GetMapUIComponent(SCR_MapToolInteractionUI)))	// if dragging available, add callback
		{
			SCR_MapToolInteractionUI.GetOnDragWidgetInvoker().Insert(OnDragWidget);
			SCR_MapToolInteractionUI.GetOnDragEndInvoker().Insert(OnDragEnd);
		}

		SCR_MapMarkerConfig markerConfig = m_MarkerMgr.GetMarkerConfig();
		if (markerConfig)
		{
			foreach (SCR_MapMarkerEntryConfig entryType : markerConfig.GetMarkerEntryConfigs())
			{
				Print("[RL_MapMarkersUI] OnMapOpen foreach loop");
				entryType.OnMapOpen(m_MapEntity, this);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{
		SCR_MapMarkerConfig markerConfig = m_MarkerMgr.GetMarkerConfig();
		if (!markerConfig)
			return;

		foreach (SCR_MapMarkerEntryConfig entryType : markerConfig.GetMarkerEntryConfigs())
		{
			Print("[RL_MapMarkersUI] OnMapClose foreach loop");
			entryType.OnMapClose(m_MapEntity, this);
		}

		CleanupMarkerEditWidget();
		GetGame().GetInputManager().RemoveActionListener("MapMarkerDelete", EActionTrigger.DOWN, OnInputMarkerDelete);
		GetGame().GetInputManager().RemoveActionListener("MapSelect", EActionTrigger.DOWN, OnInputMapSelect);
		GetGame().GetInputManager().RemoveActionListener(UIConstants.MENU_ACTION_SELECT, EActionTrigger.DOWN, OnInputMenuConfirm);
		GetGame().GetInputManager().RemoveActionListener("MenuRefresh", EActionTrigger.DOWN, OnInputMenuConfirmAlter);
		GetGame().GetInputManager().RemoveActionListener(UIConstants.MENU_ACTION_BACK, EActionTrigger.DOWN, OnInputMenuBack);
		GetGame().GetInputManager().RemoveActionListener(UIConstants.MENU_ACTION_DOWN, EActionTrigger.DOWN, OnInputMenuDown);
		GetGame().GetInputManager().RemoveActionListener(UIConstants.MENU_ACTION_UP, EActionTrigger.DOWN, OnInputMenuUp);
		GetGame().GetInputManager().RemoveActionListener(UIConstants.MENU_ACTION_RIGHT, EActionTrigger.DOWN, OnInputMenuRight);
		GetGame().GetInputManager().RemoveActionListener(UIConstants.MENU_ACTION_LEFT, EActionTrigger.DOWN, OnInputMenuLeft);

		m_MarkerMgr.EnableUpdate(false);
		super.OnMapClose(config);
	}

	override void Init()
	{
		m_CursorModule = SCR_MapCursorModule.Cast(m_MapEntity.GetMapModule(SCR_MapCursorModule));

		m_MarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		SCR_MapMarkerConfig markerConfig = m_MarkerMgr.GetMarkerConfig();
		if (!markerConfig)
			return;

		array<ref SCR_MapMarkerEntryConfig> entryConfigs = markerConfig.GetMarkerEntryConfigs();

		foreach (SCR_MapMarkerEntryConfig entryType : entryConfigs)
		{
			Print("[RL_MapMarkersUI] Init foreach loop");
			entryType.OnMapInit(m_MapEntity, this);
		}
	}

}
