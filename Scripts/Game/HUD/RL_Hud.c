class RL_Hud : SCR_InfoDisplayExtended
{
	protected float m_fUpdateInterval = 1;
	protected float m_fLastUpdateTime = 0;
	
	protected float m_fFastUpdateInterval = 0.01;
	protected float m_fFastUpdateTime = 0;

	protected static RL_Hud s_pThisHud;
	protected SCR_ChimeraCharacter m_character;

	protected VerticalLayoutWidget m_wPlayerStatsHUD;
	protected VerticalLayoutWidget m_wProgressContainer;
	protected const string m_progressBarLayout = "{6B1D57E373E1F338}UI/Layouts/HUD/ProgressBarWidget.layout";
	protected ref array<ref Widget> m_aProgressBarWidgets = {};

	protected ImageWidget m_wHealthProgress;
	protected TextWidget m_wHealthPercent;
	protected ImageWidget m_wThirstProgress;
	protected ImageWidget m_wHungerProgress;

	protected VerticalLayoutWidget m_notificationContainer;
	protected const string m_notificationLayout = "{295D5AD18A6CE8E1}UI/Layouts/HUD/Notification.layout";
	protected ref array<ref Widget> m_activeNotifications = {};

	protected VerticalLayoutWidget m_adminWarningContainer;
	protected const string m_adminWarningLayout = "{61E666526514E1C1}UI/Layouts/HUD/AdminWarning.layout";
	protected ref array<ref Widget> m_activeAdminWarnings = {};

	protected TextWidget m_wActionProgressTitle;
	protected TextWidget m_wMoneyIndicator;

	protected HorizontalLayoutWidget m_wJailContainer;
	protected TextWidget m_wJailTime;

	protected SCR_CharacterDamageManagerComponent m_DMC;
	protected RL_MetabolismComponent m_MC;
	
	protected TextWidget m_wPoliceOnlineIndicator;
	protected TextWidget m_wEMSOnlineIndicator;
	protected TextWidget m_wTimestamp;
	protected VerticalLayoutWidget m_wWaypoint;
	protected TextWidget m_wWaypointText;

	protected bool m_bUsingProgressWidget;

	protected int m_iPreviousHealth = -1;
	protected float m_fPreviousThirst = -1.0;
	protected float m_fPreviousHunger = -1.0;
	protected int m_iPreviousCash = -1;
	protected int m_iPreviousJailTime = -1;

	protected ImageWidget m_wEarplug;
	protected bool m_EarplugsIn = false;
	protected const float EARPLUG_VOLUME = 0.25;
	protected const float NORMAL_VOLUME = 1.0;
	protected bool m_EarplugListenerAdded = false;
	

	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		super.DisplayStartDraw(owner);
		s_pThisHud = this;

		m_wPlayerStatsHUD = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("playerStatsHUDv1"));
		if (!m_wPlayerStatsHUD) return;

		// Earplug setup
		AudioSystem.SetMasterVolume(AudioSystem.SFX, NORMAL_VOLUME);
		m_EarplugsIn = false;

		if (!m_wEarplug)
			m_wEarplug = ImageWidget.Cast(m_wRoot.FindAnyWidget("Earplug"));
		if (m_wEarplug)
			m_wEarplug.SetVisible(false);

		if (!m_EarplugListenerAdded)
		{
			InputManager inputManager = GetGame().GetInputManager();
			if (inputManager)
			{
				inputManager.AddActionListener("ToggleEarplugs", EActionTrigger.DOWN, OnToggleEarplugs);
				m_EarplugListenerAdded = true;
			}
		}

		m_notificationContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("notificationContainer"));
		m_adminWarningContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("adminWarningContainer"));
		m_wProgressContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("progressContainer"));

		m_wJailContainer = HorizontalLayoutWidget.Cast(m_wRoot.FindAnyWidget("jailContainer"));
		if (!m_wJailContainer) return;
		m_wJailTime = TextWidget.Cast(m_wJailContainer.FindAnyWidget("jailTime"));
		m_wPoliceOnlineIndicator = TextWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_policeOnlineIndicator"));
		m_wEMSOnlineIndicator = TextWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_emsOnlineIndicator"));
		m_wTimestamp = TextWidget.Cast(m_wRoot.FindAnyWidget("timestamp"));
		m_wWaypoint = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("waypoint"));
		m_wWaypointText = TextWidget.Cast(m_wRoot.FindAnyWidget("waypointText"));
		UpdateTimestamp();
	}

	//------------------------------------------------------------------------------------------------
	protected void OnToggleEarplugs()
	{
		m_EarplugsIn = !m_EarplugsIn;

		if (m_EarplugsIn)
		{
			AudioSystem.SetMasterVolume(AudioSystem.SFX, EARPLUG_VOLUME);
			EarplugSetVisibility(true);
		}
		else
		{
			AudioSystem.SetMasterVolume(AudioSystem.SFX, NORMAL_VOLUME);
			EarplugSetVisibility(false);
		}
	}

	protected void EarplugSetVisibility(bool state)
	{
		if (m_wEarplug)
			m_wEarplug.SetVisible(state);
	}

	protected void EarplugUpdateIcon(string iconName)
	{
		if (m_wEarplug)
			m_wEarplug.SetVisible(iconName != "");
	}

	//------------------------------------------------------------------------------------------------
	override protected void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		m_character = SCR_ChimeraCharacter.Cast(to);
		if (!m_character) return;

		m_character.GetOnAccountUpdated().Insert(OnAccountUpdated);

		m_DMC = SCR_CharacterDamageManagerComponent.Cast(m_character.FindComponent(SCR_CharacterDamageManagerComponent));
		m_MC = RL_MetabolismComponent.Cast(m_character.FindComponent(RL_MetabolismComponent));
		m_world = GetGame().GetWorld();
	}

	void OnAccountUpdated()
	{
		if (!s_pThisHud || !m_wPlayerStatsHUD) return;

		OnCashChange(m_character.GetCash());
		OnJailTimeChange(m_character.GetJailTime());
	}

	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		DisplayUpdateFast(owner, timeSlice);

		if (!m_DMC || !m_MC) return;

		m_fLastUpdateTime += timeSlice;
		if (m_fLastUpdateTime < m_fUpdateInterval)
			return;
		m_fLastUpdateTime = 0;

		bool shouldDisplayHud = (m_character && !m_character.GetCharacterId().IsEmpty() && !m_DMC.IsWaitingForCpr());
		if (m_wPlayerStatsHUD.IsVisible() != shouldDisplayHud)
			m_wPlayerStatsHUD.SetVisible(shouldDisplayHud);

		if (!shouldDisplayHud) return;

		OnHealthChange(Math.Floor(m_DMC.GetHealth()));
		OnHungerChange(m_MC.GetFoodLevel());
		OnThirstChange(m_MC.GetThirstLevel());

		UpdateTimestamp();

	}
	void DisplayUpdateFast(IEntity owner, float timeSlice)
	{
		m_fFastUpdateTime += timeSlice;
		if (m_fFastUpdateTime < m_fFastUpdateInterval)
			return;
		m_fFastUpdateTime = 0;
		
		OnWaypoint();
	}

	bool m_bIsVisible = false;
	protected int m_iWaypointPadding = 60;
	protected World m_world;
	protected vector m_vWaypointPos = vector.Zero;
	protected vector m_vPreviousWaypointPos = vector.Zero;
	void OnWaypoint()
	{
	
		if(!m_world || !m_character || !m_vWaypointPos || m_vWaypointPos == vector.Zero)
		{	if(m_bIsVisible)
			{
				m_wWaypoint.SetVisible(false);
				m_bIsVisible = false; 
			}
			return;
		}
		vector waypointHudPos = GetGame().GetWorkspace().ProjWorldToScreen(m_vWaypointPos, m_world);
		m_vPreviousWaypointPos = m_vWaypointPos;

		float x = waypointHudPos[0] - m_iWaypointPadding;
        float y = waypointHudPos[1] - m_iWaypointPadding;
        FrameSlot.SetPos(m_wWaypoint, x, y);
		float z = waypointHudPos[2];
		bool shouldBeVisble = z > 0;
		bool isMapOpen = SCR_MapEntity.GetMapInstance() && SCR_MapEntity.GetMapInstance().IsOpen();
		if(isMapOpen)
			shouldBeVisble = false;
		if(shouldBeVisble != m_bIsVisible)
		{
        	m_wWaypoint.SetVisible(shouldBeVisble);
			m_bIsVisible = shouldBeVisble;
		}
		
		if (!m_character)
			return;
		vector characterPos = m_character.GetOrigin();
		float f_distance = vector.Distance(characterPos, m_vWaypointPos);

		string distanceText = "";
		if (f_distance < 1000)
		{
			distanceText = string.Format("%1 m", Math.Round(f_distance).ToString());
		}
		else
		{
			float km = f_distance / 1000.0;
			float simpleKm = Math.Round(km * 10.0) * 0.1;
			distanceText = string.Format("%1 km", simpleKm.ToString());
		}
		m_wWaypointText.SetText(distanceText);
	}
	void SetWaypointPos(vector newPos)
	{
		m_vWaypointPos = newPos;
	}
	void UpdateTimestamp()
	{
		if (!m_wTimestamp)
			return;
			
		int year, month, day, hour, minute, second;
		System.GetYearMonthDayUTC(year, month, day);
		System.GetHourMinuteSecondUTC(hour, minute, second);
		string monthStr = month.ToString();
		if (month < 10)
			monthStr = "0" + monthStr;
			
		string dayStr = day.ToString();
		if (day < 10)
			dayStr = "0" + dayStr;
			
		string hourStr = hour.ToString();
		if (hour < 10)
			hourStr = "0" + hourStr;
			
		string minuteStr = minute.ToString();
		if (minute < 10)
			minuteStr = "0" + minuteStr;
		
		string chracterId = "";
		if(m_character)
			chracterId = m_character.GetCharacterId();

		string timeString = string.Format("%1 - %2-%3-%4 %5:%6 UTC", 
			chracterId, year.ToString(), monthStr, dayStr, hourStr, minuteStr);
			
		m_wTimestamp.SetText(timeString);
	}

	void OnHealthChange(int value)
	{
		if (!m_wHealthProgress)
			m_wHealthProgress = ImageWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_healthProgress"));
		if (!m_wHealthPercent)
			m_wHealthPercent = TextWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_healthPercentIndicator"));

		if (m_iPreviousHealth != value)
		{
			m_iPreviousHealth = value;
			m_wHealthProgress.SetMaskProgress(value / 100);
			m_wHealthPercent.SetText(string.Format("%1", value));
		}
	}

	void OnThirstChange(float value)
	{
		if (!m_wThirstProgress)
			m_wThirstProgress = ImageWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_thirstProgress"));

		if (m_wThirstProgress && m_fPreviousThirst != value)
		{
			//Print("OnThirstChange " + value + " " + m_fPreviousThirst);
			m_fPreviousThirst = value;
			m_wThirstProgress.SetMaskProgress(value);
		}
	}

	void OnHungerChange(float value)
	{
		if (!m_wHungerProgress)
			m_wHungerProgress = ImageWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_hungerProgress"));

		if (m_wHungerProgress && m_fPreviousHunger != value)
		{
			//Print("OnHungerChange " + value + " " + m_fPreviousHunger);
			m_fPreviousHunger = value;
			m_wHungerProgress.SetMaskProgress(value);
		}
	}

	void OnCashChange(int value)
	{
		if (!m_wMoneyIndicator)
			m_wMoneyIndicator = TextWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_moneyIndicator"));

		if (m_wMoneyIndicator && m_iPreviousCash != value)
		{
			m_iPreviousCash = value;
			m_wMoneyIndicator.SetText(RL_Utils.FormatMoney(value));
		}
	}

	void OnJailTimeChange(int value)
	{
		if (!m_wJailContainer || !m_wJailTime) return;

		bool isInJail = (value > 0);
		if (m_wJailContainer.IsVisible() != isInJail)
			m_wJailContainer.SetVisible(isInJail);

		if (!isInJail) return;

		if (m_iPreviousJailTime != value)
		{
			m_iPreviousJailTime = value;
			m_wJailTime.SetText(string.Format("%1 MONTHS", value));
		}
	}

	void AddNotification(string content, string title = "", int duration = 5)
	{
		Widget notiWidget = GetGame().GetWorkspace().CreateWidgets(m_notificationLayout, m_notificationContainer);
		TextWidget titleWidget = TextWidget.Cast(notiWidget.FindAnyWidget("title"));
		TextWidget contentWidget = TextWidget.Cast(notiWidget.FindAnyWidget("content"));
		titleWidget.SetText(title);
		contentWidget.SetText(content);
		m_activeNotifications.Insert(notiWidget);
		GetGame().GetCallqueue().CallLater(DeleteNotification, duration * 1000, false);
	}

	void DeleteNotification()
	{
		int count = m_activeNotifications.Count();
		if (count < 1)
			return;
		m_notificationContainer.RemoveChild(m_activeNotifications[0]);
		m_activeNotifications.Remove(0);
	}

	void AddIdentity(Widget identityWidget, int duration = 5)
	{
		m_notificationContainer.AddChild(identityWidget);
		m_activeNotifications.Insert(identityWidget);
		GetGame().GetCallqueue().CallLater(DeleteNotification, duration * 1000, false);
	}

	void SetProgressColor(Widget bar, float value)
	{
		if (value >= 0.65)
			bar.SetColor(Color.White);
		else if (value > 0.35)
			bar.SetColor(Color.Yellow);
		else
			bar.SetColor(Color.Red);
	}

	Widget CreateActionProgressBar(string title, int totalTime, bool allowConcurrentAction = false)
	{
		if (totalTime <= 0)
		{
			RL_Utils.Notify("Action failed", "ACTION");
			return null;
		}
		ReconcileProgressWidgets(); // idk maybe fix bug
		if (!allowConcurrentAction && m_aProgressBarWidgets.Count() > 0)
		{
			RL_Utils.Notify("Action already in progress.", "ACTION");
			return null;
		}
		Widget progressBar = GetGame().GetWorkspace().CreateWidgets(m_progressBarLayout, m_wProgressContainer);
		RL_ProgressBarWidget progressWidget = RL_ProgressBarWidget.Cast(progressBar.FindHandler(RL_ProgressBarWidget));
		if (!progressWidget) return null;
		m_aProgressBarWidgets.Insert(progressBar);
		progressWidget.GetOnComplete().Insert(OnProgressBarComplete);
		progressWidget.InitProgressBar(title, totalTime);
		return progressBar;
	}
	void ReconcileProgressWidgets()
	{
		foreach (Widget progressBar : m_aProgressBarWidgets)
        {
			Print("[RL_Hud] ReconcileProgressWidgets foreach loop");
			if(progressBar)
			{
				RL_ProgressBarWidget progressWidget = RL_ProgressBarWidget.Cast(progressBar.FindHandler(RL_ProgressBarWidget));
				if(progressWidget && !progressWidget.IsComplete())
					return;
			}
			OnProgressBarComplete(progressBar);

		}

	}

	void OnProgressBarComplete(Widget progressWidget)
	{
		Print("OnProgressBarComplete");
		Print(progressWidget);
		Print(m_aProgressBarWidgets);
		if (m_aProgressBarWidgets.Contains(progressWidget))
			m_aProgressBarWidgets.RemoveItem(progressWidget);
		delete progressWidget;
	}

	void AddAdminWarning(int duration = 30)
	{
		if (!m_adminWarningContainer)
			return;

		Widget adminWarningWidget = GetGame().GetWorkspace().CreateWidgets(m_adminWarningLayout, m_adminWarningContainer);
		if (!adminWarningWidget)
			return;

		m_activeAdminWarnings.Insert(adminWarningWidget);
		GetGame().GetCallqueue().CallLater(DeleteAdminWarning, duration * 1000, false);
	}

	void DeleteAdminWarning()
	{
		int count = m_activeAdminWarnings.Count();
		if (count < 1)
			return;
		
		m_adminWarningContainer.RemoveChild(m_activeAdminWarnings[0]);
		m_activeAdminWarnings.Remove(0);
	}

	void ShowAdminWarning()
	{
		AddAdminWarning();
	}

	static RL_Hud GetCurrentInstance()
	{
		return s_pThisHud;
	}

	void ~RL_Hud()
	{
		if (m_character)
			m_character.GetOnAccountUpdated().Remove(OnAccountUpdated);
	}
}
