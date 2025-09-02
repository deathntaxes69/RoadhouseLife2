class RL_RandomEventManager
{
    protected static ref RL_RandomEventManager s_Instance;
    protected ref RL_RandomEventConfig m_EventConfig;
    protected ref array<ref RL_RandomEventGroup> m_EventGroups;
    protected ref map<string, int> m_GroupTimers;
    protected static const string m_ConfigPath = "{807445C4860A003B}Configs/RandomEvents/RL_RandomEvents.conf";
    protected ref map<string, ref array<RL_RandomEventSpawnPoint>> m_SpawnPoints;
    
    // We could change this system to setup all events at server runtime like kevin suggested but this *feels* more random

    //------------------------------------------------------------------------------------------------
    static RL_RandomEventManager GetInstance()
    {
        if (!s_Instance)
            s_Instance = new RL_RandomEventManager();
        return s_Instance;
    }
    
    //------------------------------------------------------------------------------------------------
    void RL_RandomEventManager()
    {
        m_GroupTimers = new map<string, int>();
        m_SpawnPoints = new map<string, ref array<RL_RandomEventSpawnPoint>>();
        LoadEventConfig();
        InitializeTimers();
		
		GetGame().GetCallqueue().CallLater(CallEvent, 20000, false, "InitializeCartelShipmentSpawns");
    }
    
    //------------------------------------------------------------------------------------------------
    protected void LoadEventConfig()
    {
        PrintFormat("[RL_RandomEventManager] Loading event config from %1", m_ConfigPath);
            
        Resource configResource = BaseContainerTools.LoadContainer(m_ConfigPath);
        if (!configResource)
        {
            PrintFormat("[RL_RandomEventManager] Could not load config file");
            return;
        }
        
        m_EventConfig = RL_RandomEventConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(configResource.GetResource().ToBaseContainer()));
        if (!m_EventConfig)
        {
            PrintFormat("[RL_RandomEventManager] Could not create config instance");
            return;
        }
        
        m_EventGroups = m_EventConfig.eventGroups;

        PrintFormat("[RL_RandomEventManager] Loaded %1 event groups", m_EventGroups.Count());
    }
    
    //------------------------------------------------------------------------------------------------
    protected void InitializeTimers()
    {
        if (!m_EventGroups)
            return;
        Print("[RL_RandomEventManager] InitializeTimers");

        foreach (RL_RandomEventGroup group : m_EventGroups)
        {
            Print("[RL_RandomEventManager] InitializeTimers foreach");
            if (!group || !group.enabled)
                continue;
                
            m_GroupTimers.Set(group.groupId, 0);
            GetGame().GetCallqueue().CallLater(CheckEventGroup, group.frequencyMinutes * 60000, true, group.groupId);
            PrintFormat("[RL_RandomEventManager] Initialized timer for group %1 (%2) at frequency %3 minutes", group.name, group.groupId, group.frequencyMinutes);
        }
    }
    
    //------------------------------------------------------------------------------------------------
    protected void CheckEventGroup(string groupId)
    {
        if (!m_EventGroups)
            return;
            
        RL_RandomEventGroup group = FindEventGroup(groupId);
        if (!group || !group.enabled)
            return;
        
        PrintFormat("[RL_RandomEventManager] Checking event group %1 (%2)", group.name, groupId);
            
        float randomChance = Math.RandomFloat01();
        if (randomChance > group.executionChance)
        {
            PrintFormat("[RL_RandomEventManager] Group %1 skipped, chance %2 > %3", group.name, randomChance, group.executionChance);
            return;
        }
        
        array<ref RL_RandomEventDefinition> enabledEvents = new array<ref RL_RandomEventDefinition>();
        int totalWeight = 0;
        
        foreach (RL_RandomEventDefinition eventDef : group.events)
        {
            Print("[RL_RandomEventManager] CheckEventGroup foreach");
            if (eventDef && eventDef.enabled)
            {
                enabledEvents.Insert(eventDef);
                totalWeight += eventDef.weight;
            }
        }
        
        if (enabledEvents.Count() == 0)
            return;
        
        RL_RandomEventDefinition selectedEvent = SelectWeightedRandomEvent(enabledEvents, totalWeight);
        if (selectedEvent)
        {
            ExecuteEvent(selectedEvent);
        }
    }
    
    //------------------------------------------------------------------------------------------------
    protected RL_RandomEventDefinition SelectWeightedRandomEvent(array<ref RL_RandomEventDefinition> events, int totalWeight)
    {
        if (totalWeight <= 0)
            return null;
        Print("[RL_RandomEventManager] RL_RandomEventDefinition");

        int randomValue = Math.RandomInt(0, totalWeight);
        int currentWeight = 0;
        
        foreach (RL_RandomEventDefinition eventDef : events)
        {
            Print("[RL_RandomEventManager] SelectWeightedRandomEvent foreach");
            currentWeight += eventDef.weight;
            if (randomValue < currentWeight)
            {
                return eventDef;
            }
        }
        
        return events[events.Count() - 1];
    }
    
    //------------------------------------------------------------------------------------------------
    protected void ExecuteEvent(RL_RandomEventDefinition eventDef)
    {
        PrintFormat("[RL_RandomEventManager] Running event %1 (%2) (%3)", eventDef.name, eventDef.eventId, eventDef.methodName);
            
        if (eventDef.methodName != "")
        {
            CallEvent(eventDef.methodName);
        }
    }
    
    //------------------------------------------------------------------------------------------------
    protected void CallEvent(string methodName)
    {
        Print("[RL_RandomEventManager] CallEvent");
		ScriptModule scriptModule = GetGame().GetScriptModule();
        scriptModule.Call(this, methodName, true, null);
    }
    
    //------------------------------------------------------------------------------------------------
    protected RL_RandomEventGroup FindEventGroup(string groupId)
    {
        if (!m_EventGroups)
            return null;
            
        foreach (RL_RandomEventGroup group : m_EventGroups)
        {
            Print("[RL_RandomEventManager] FindEventGroup foreach");
            if (group && group.groupId == groupId)
                return group;
        }
        
        return null;
    }

    //------------------------------------------------------------------------------------------------
    // Force trigger
    void TriggerEvent(string eventId)
    {
        if (!m_EventGroups)
            return;

        Print("[RL_RandomEventManager] TriggerEvent");
            
        foreach (RL_RandomEventGroup group : m_EventGroups)
        {
            Print("[RL_RandomEventManager] TriggerEvent foreach");
            if (!group || !group.enabled)
                continue;
                
            foreach (RL_RandomEventDefinition eventDef : group.events)
            {
                Print("[RL_RandomEventManager] TriggerEvent foreach 2");
                if (eventDef && eventDef.eventId == eventId && eventDef.enabled)
                {
                    ExecuteEvent(eventDef);
                    return;
                }
            }
        }
        
        PrintFormat("[RL_RandomEventManager] Event not found %1, eventId");
    }
    
    //------------------------------------------------------------------------------------------------
    void RegisterSpawnPoint(RL_RandomEventSpawnPoint spawnPoint)
    {
        if (!spawnPoint)
            return;
            
        if (!m_SpawnPoints)
        {
            PrintFormat("[RL_RandomEventManager] m_SpawnPoints is null, initializing");
            m_SpawnPoints = new map<string, ref array<RL_RandomEventSpawnPoint>>();
        }
            
        string spawnId = spawnPoint.GetSpawnId();
        if (spawnId.IsEmpty())
            return;
        
        if (!m_SpawnPoints.Contains(spawnId))
        {
            m_SpawnPoints.Set(spawnId, new array<RL_RandomEventSpawnPoint>());
        }
        
        ref array<RL_RandomEventSpawnPoint> spawnArray = m_SpawnPoints.Get(spawnId);
        spawnArray.Insert(spawnPoint);
        
        PrintFormat("[RL_RandomEventManager] Registered spawn point %1 (total: %2)", spawnId, spawnArray.Count());
    }
    
    //------------------------------------------------------------------------------------------------
    void UnregisterSpawnPoint(RL_RandomEventSpawnPoint spawnPoint)
    {
        if (!spawnPoint)
            return;
            
        string spawnId = spawnPoint.GetSpawnId();
        if (spawnId.IsEmpty())
            return;
            
        if (!m_SpawnPoints.Contains(spawnId))
            return;
            
        ref array<RL_RandomEventSpawnPoint> spawnArray = m_SpawnPoints.Get(spawnId);
        int index = spawnArray.Find(spawnPoint);
        if (index != -1)
        {
            spawnArray.Remove(index);
            PrintFormat("[RL_RandomEventManager] Unregistered spawn point %1", spawnId);
            
            if (spawnArray.Count() == 0)
            {
                m_SpawnPoints.Remove(spawnId);
            }
        }
    }
    
    //------------------------------------------------------------------------------------------------
    ref array<RL_RandomEventSpawnPoint> GetSpawnPoints(string spawnId)
    {
        if (!m_SpawnPoints.Contains(spawnId))
            return null;
            
        return m_SpawnPoints.Get(spawnId);
    }
    
    //------------------------------------------------------------------------------------------------
    ref array<RL_RandomEventSpawnPoint> GetAvailableSpawnPoints(string spawnId)
    {
        if (!m_SpawnPoints.Contains(spawnId))
            return null;
            
        ref array<RL_RandomEventSpawnPoint> allSpawnPoints = m_SpawnPoints.Get(spawnId);
        ref array<RL_RandomEventSpawnPoint> availableSpawnPoints = new array<RL_RandomEventSpawnPoint>();
        
        foreach (RL_RandomEventSpawnPoint spawnPoint : allSpawnPoints)
        {
            Print("[RL_RandomEventManager] GetAvailableSpawnPoints foreach");
            if (spawnPoint && !spawnPoint.IsOccupied())
            {
                availableSpawnPoints.Insert(spawnPoint);
            }
        }
        
        return availableSpawnPoints;
    }
} 