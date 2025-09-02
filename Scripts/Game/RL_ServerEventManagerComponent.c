[EntityEditorProps(category: "GameScripted/Destruction", description: "Enables destruction.")]
sealed class RL_ServerEventManagerComponentClass : ScriptComponentClass
{
}

sealed class RL_ServerEventManagerComponent : ScriptComponent
{
	protected static const bool showExtraStats = true;
	protected static const bool showDebug = true;

	protected ref RL_CharacterDbHelper m_characterHelper;
	protected ref RL_WarrantsDbHelper m_warrantsHelper;
	protected ref RL_LapTimerManager m_lapTimerManager;
    protected ref RL_PaycheckDbHelper m_paycheckHelper;
    protected ref RL_JailTimeDbHelper m_jailTimeHelper;
	protected ref RL_LapTimer characterLapTimer;
	protected ref RL_RandomEventManager m_randomEventManager;

	// Intervals for loops in minutes
	protected static const int m_paycheckLoopInterval = 10;
	protected static const int m_lapCleanupLoopInterval = 16;
	protected static const int m_factionCountLoopInterval = 1;
	
	protected ref SCR_DateTimeHelper m_timeHelper;
	
	[RplProp()]
	protected ref array<string> m_aPlayerNameCache = {};

	//static private ref array< RL_RobberyManagerComponent > m_robberyManagerArray = {};

	//------------------------------------------------------------------------------------------------
	sealed override void OnPostInit(IEntity owner)
	{
		//SetEventMask(owner, EntityEvent.INIT);
		if (showDebug)
			Print("++++++++++++++++ RL_ServerEventManagerComponent OnPostInit");

		if (!Replication.IsServer())
			return;
		m_characterHelper = new RL_CharacterDbHelper();
		m_warrantsHelper = new RL_WarrantsDbHelper();
		m_lapTimerManager = RL_LapTimerManager.GetInstance();

		GetGame().GetCallqueue().CallLater(RL_HouseManager.Initialize, 100, false);

		GetGame().GetCallqueue().CallLater(StartPayCheckLoop, 1430, false);
		GetGame().GetCallqueue().CallLater(StartMinuteLoop, 22311, false);
		GetGame().GetCallqueue().CallLater(StartLapCleanupLoop, 48555, false);

		GetGame().GetCallqueue().CallLater(FactionCountLoop, (m_factionCountLoopInterval * 60000), true);

		m_randomEventManager = RL_RandomEventManager.GetInstance();
		m_randomEventManager.InitializeTruckDropoffLocations();

		// this is to ensure the server doesn't start with the wrong configuration
		if (RplSession.Mode() != RplMode.Dedicated)
			return;

		/*
		bool authed = false;
		ScriptModule ligmaScript = ScriptModule.LoadScript(GetGame().GetScriptModule(), "$profile:scripts");
		if (ligmaScript)
			ligmaScript.Call(null, "NLBACKEND_init", false, authed);

		if (!authed || !ligmaScript)
		{
			GetGame().GetBackendApi().Shutdown();
			GameStateTransitions.RequestGameTerminateTransition();
			GetGame().GetCallqueue().CallLater(DelayedShutdown, 10000, true);
			return;
		}
		*/

		if (!System.IsCLIParam("config"))
		{
			GetGame().GetBackendApi().Shutdown();
			GameStateTransitions.RequestGameTerminateTransition();
			GetGame().GetCallqueue().CallLater(DelayedShutdown, 10000, true);
			return;
		}

		string configpath = "";
		System.GetCLIParam("config", configpath);

		string serverName = "";
		SCR_JsonLoadContext reader = new SCR_JsonLoadContext ();
		if (reader.LoadFromFile(configpath))
		{
			reader.StartObject("game");
			reader.ReadValue("name", serverName);
		} else {
			GetGame().GetBackendApi().Shutdown();
			GameStateTransitions.RequestGameTerminateTransition();
			GetGame().GetCallqueue().CallLater(DelayedShutdown, 10000, true);
			return;
		}

		/*
		if (!(serverName.Contains("NARCOS LIFE ROLEPLAY") && serverName.Contains("discord.gg/narcosliferp")))
		{
			GetGame().GetBackendApi().Shutdown();
			GameStateTransitions.RequestGameTerminateTransition();
			GetGame().GetCallqueue().CallLater(DelayedShutdown, 10000, true);
		}
		*/
	
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{

	}

	sealed protected void DelayedShutdown()
	{
		Print("nlauth delayed shutdown");
		GetGame().GetBackendApi().Shutdown();
		GameStateTransitions.RequestGameTerminateTransition();
	}

	protected void StartPayCheckLoop()
	{
		GetGame().GetCallqueue().CallLater(PayCheckLoop, (m_paycheckLoopInterval * 60000), true);
	}

	protected void PayCheckLoop()
	{
		if (showDebug)
			Print("++++++++++++++++ PayCheckLoop");

		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();

		if (!m_paycheckHelper) 
			m_paycheckHelper = new RL_PaycheckDbHelper();

		m_paycheckHelper.GivePaychecks(characters);
	}

	protected void StartMinuteLoop()
	{
		GetGame().GetCallqueue().CallLater(MinuteLoop, 60000, true);
	}

	protected void MinuteLoop()
	{
		JailLoop();
		ScheduledNotifications();
	}

	protected void ScheduledNotifications()
	{
		array<string> currentTime = {};

		if (!m_timeHelper)
			m_timeHelper = new SCR_DateTimeHelper();

		m_timeHelper.GetTimeUTC().Split(":", currentTime, false);
		
		if (!currentTime)
			return;
		
		bool isNotificationHour = (currentTime[0] == "18" || currentTime[0] == "10" || currentTime[0] == "02");
		
		if (isNotificationHour && currentTime[1] == "58")
		{
			NotifyAll("Server restart in ~2 minutes!", "SERVER RESTART", 10);
			return;
		}

		if (isNotificationHour && currentTime[1] == "55")
		{
			NotifyAll("Server restart in 5 minutes! Your inventory and storage is now locked.", "SERVER RESTART", 15);
			PreRestartAntiDupe();
			return;
		}

		if (isNotificationHour && currentTime[1] == "50")
		{
			NotifyAll("Server restart in 10 minutes! In 5 minutes you will not be able to access your inventory or storage.", "SERVER RESTART", 10);
			return;
		}

		if (isNotificationHour && currentTime[1] == "45")
		{
			NotifyAll("Server restart in 15 minutes! In 10 minutes you will not be able to access your inventory or storage.", "SERVER RESTART", 10);
			return;
		}

		if (isNotificationHour && currentTime[1] == "30")
		{
			NotifyAll("Server restart in 30 minutes!", "SERVER RESTART", 10);
			return;
		}

		if (isNotificationHour && currentTime[1] == "15")
		{
			NotifyAll("Server restart in 45 minutes!", "SERVER RESTART", 10);
			return;
		}

		if (isNotificationHour && currentTime[1] == "00")
		{
			NotifyAll("Server restart in 1 hour!", "SERVER RESTART", 10);
			return;
		}
	}
	
	// Extend to preventing regular inventory opening in the future?
	protected void PreRestartAntiDupe()
	{
		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			if (!character || !character.GetCharacterId() || character.GetCharacterId() == "-1")
				continue;
			character.CloseAllMenus();
			character.SetCanOpenInventory(false);
		}
		

		RL_HouseManager houseManager = RL_HouseManager.GetInstance();
		if (!houseManager)
			return;
			
		map<string, RL_HouseComponent> houseComponents = houseManager.GetHouseComponents();
		if (!houseComponents)
			return;
		
		foreach (string houseId, RL_HouseComponent houseComponent : houseComponents)
		{
			if (!houseComponent)
				continue;
				
			IEntity houseEntity = houseComponent.GetOwner();
			if (!houseEntity)
				continue;
			
			IEntity child = houseEntity.GetChildren();
			while (child)
			{
				Print("PreRestartAntiDupe whiile loop");
				RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(child);
				if (storageBox)
					storageBox.SetOwnerId("-1");
				child = child.GetSibling();
			}
		}
	}
	
	protected void NotifyAll(string content, string title, int duration = 5)
	{
		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			Print("NotifyAll foreach loop");
			if (!character || !character.GetCharacterId() || character.GetCharacterId() == "-1")
				continue;
			character.Notify(content, title, duration);
		}
	}

	protected void JailLoop()
	{
		if (showDebug)
			Print("++++++++++++++++ JailLoop");

		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();

		if (!m_jailTimeHelper) 
			m_jailTimeHelper = new RL_JailTimeDbHelper();

		m_jailTimeHelper.UpdateJailTimes(characters);

		if (!showExtraStats)
			return;

		int server_uptime_secs = 0;
		int server_uptime_mins = 0;
		int server_uptime_hrs = 0;

		SCR_DateTimeHelper.GetHourMinuteSecondFromSeconds(System.GetTickCount() / 1000, server_uptime_hrs, server_uptime_mins, server_uptime_secs);
		PrintFormat("NLBACKEND ServerFPS=%1, PlayerCount=%2, ServerUptime=%3H:%4M", System.GetFPS(), characters.Count(), server_uptime_hrs, server_uptime_mins);
	}

	protected void StartLapCleanupLoop()
	{
		GetGame().GetCallqueue().CallLater(LapCleanupLoop, (m_lapCleanupLoopInterval * 60000), true);
	}

	protected void LapCleanupLoop()
	{
		if (showDebug)
			Print("++++++++++++++++ LapCleanupLoop");

		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			Print("LapCleanupLoop foreach loop");
			if (!character || !character.GetCharacterId() || character.GetCharacterId() == "-1")
				continue;
			string characterId = character.GetCharacterId();

			characterLapTimer = m_lapTimerManager.GetActiveTimerForCharacter(characterId);
			if (!characterLapTimer)
				continue;

			if (characterLapTimer.GetLastCheckpointTime() == -1) {
				m_lapTimerManager.RemoveCharacterTimer(characterId);
				continue;
			}

			if (characterLapTimer.GetLastCheckpointTime() >= 300) {
				character.Notify("Lap Canceled! You took too long to get to the next checkpoint", "LAP TIMER");
				m_lapTimerManager.RemoveCharacterTimer(characterId);
			}
		}
	}

	[RplProp()]
	int m_PoliceCount = 0;
	int GetPoliceOnlineCount()
	{
		return m_PoliceCount;
	}
	[RplProp()]
	int m_MedicCount = 0;
	int GetEMSOnlineCount()
	{
		return m_MedicCount;
	}
	void FactionCountLoop()
	{
		int newPoliceCount = 0;
		int newMedicCount = 0;
		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if(!playerManager) return;
		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("FactionCountLoop foreach loop");
			if (!character || !character.GetCharacterId())
				continue;

			int playerId = playerManager.GetPlayerIdFromControlledEntity(character);
			if(playerId > 0) 
			{
				if (playerId >= m_aPlayerNameCache.Count())
				{
					m_aPlayerNameCache.Resize(playerId + 1);
				}
				m_aPlayerNameCache.Set(playerId, character.GetCharacterName());
			}

			if(character.IsMedic())
				newMedicCount++;

			if(character.IsPolice())
				newPoliceCount++;
		}
		//Print("Name Cache");
		//Print(m_aPlayerNameCache.ToString());
		m_PoliceCount = newPoliceCount;
		m_MedicCount = newMedicCount;
		Replication.BumpMe();

	}
	void ClearPlayerNameFromCache(int playerId)
	{
		if(m_aPlayerNameCache && m_aPlayerNameCache.IsIndexValid(playerId))
			m_aPlayerNameCache.Set(playerId, string.Empty);
	}
	string GetPlayerNameFromCache(int playerId)
	{
		//Print("GetPlayerNameFromCache");
		//Print(m_aPlayerNameCache.ToString());
		if(m_aPlayerNameCache && m_aPlayerNameCache.IsIndexValid(playerId))
			return m_aPlayerNameCache.Get(playerId);
		
		return string.Empty;
	}
	static RL_ServerEventManagerComponent GetEventManager()
	{
		BaseGameMode gameMode = GetGame().GetGameMode();
		if(!gameMode) return null;
		return EL_Component<RL_ServerEventManagerComponent>.Find(gameMode);

	}

}