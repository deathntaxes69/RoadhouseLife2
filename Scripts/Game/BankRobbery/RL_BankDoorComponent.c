[EntityEditorProps(category: "GameScripted/Components", description: "")]
class SCR_BankDoorComponentClass : ScriptComponentClass
{
	[Attribute("", UIWidgets.EditBox, "Bank Name (displayed on map marker)")]
	string m_sBankName;
	
	[Attribute("500", UIWidgets.EditBox, "Drill Time (Seconds)")]
	int m_drillTime;
	
	[Attribute("100", UIWidgets.EditBox, "Succesful mini game disarm alarm delay (Seconds)")]
	int m_disarmAlarmDelay;
	
	[Attribute("", UIWidgets.Auto, "Alarm Sound Configuration")]
	ref SCR_AudioSourceConfiguration m_AlarmAudioSourceConfiguration;
	
	[Attribute("", UIWidgets.Coords, "Sound Offset Position")]
	vector m_vAlarmSoundOffset;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Required Drill Item", params: "et")]
	ResourceName m_DrillItemPrefab;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Required Explosive Item", params: "et")]
	ResourceName m_ExplosiveItemPrefab;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Explosion Effect Prefab", params: "et")]
	ResourceName m_ExplosionEffectPrefab;
	
	[Attribute("10", UIWidgets.EditBox, "Explosion Countdown Time (Seconds)")]
	int m_explosionCountdownTime;
	
	[Attribute("", UIWidgets.Auto, "Drill Spawn Location")]
	ref PointInfo m_DrillSpawnLocation;
}

enum RL_BankOperationType
{
	NONE,
	DRILL,
	EXPLOSION
}

class SCR_BankDoorComponent : ScriptComponent
{	
	protected static const int UPDATE_INTERVAL_MS = 10000;
	protected static const int DEFAULT_DRILL_TIME = 30;
	protected static const int DEFAULT_COUNTDOWN_TIME = 10;
	protected static const int DEFAULT_ALARM_DELAY = 100;
	protected static const string DEFAULT_BANK_NAME = "Bank";
	protected static const string DRILL_PREFAB_PATH = "{C71D7124EF929731}Prefabs/BankRobbery/VaultDrillStatic.et";
	protected static const string ROBBERY_MARKER_PATH = "{DF3550E63EBBE99E}Prefabs/World/Locations/Common/RL_MapMarker_Robbery.et";
	protected static const int MARKER_DURATION = 1200000;
	
	[RplProp()]
	bool m_alarmDisabled = false;
	
	[RplProp()]
	bool m_isOpen = false;
	
	[RplProp()]
	bool m_alarmTriggered = false;
	
	[RplProp()]
	protected int m_currentOperation = RL_BankOperationType.NONE;
	
	[RplProp()]
	protected float m_operationStartTime = 0;
	
	[RplProp()]
	protected int m_operationDuration = 0;
	
	[RplProp()]
	protected int m_operationTimeRemaining = 0;
	
	[RplProp()]
	protected float m_alarmScheduledTime = 0;

	protected bool m_timerActive = false;
	private AudioHandle m_AlarmAudioHandle = AudioHandle.Invalid;
	protected IEntity m_bankRobberyMarker;
	protected IEntity m_spawnedDrill;
	protected SCR_BankDoorComponentClass m_cachedSettings;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_BankDoorComponentClass GetSettings()
	{
		if (!m_cachedSettings)
			m_cachedSettings = SCR_BankDoorComponentClass.Cast(GetComponentData(GetOwner()));
		return m_cachedSettings;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float GetCurrentTime()
	{
		return GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float CalculateElapsedTime(float startTime)
	{
		return (GetCurrentTime() - startTime) / 1000.0;
	}
	
	//------------------------------------------------------------------------------------------------
	protected int CalculateTimeRemaining(float startTime, int duration)
	{
		float elapsedTime = CalculateElapsedTime(startTime);
		return Math.Max(0, duration - Math.Round(elapsedTime));
	}
		
	//------------------------------------------------------------------------------------------------
	protected string GetBankName()
	{
		SCR_BankDoorComponentClass settings = GetSettings();
		if (settings && !settings.m_sBankName.IsEmpty())
			return settings.m_sBankName;
		return DEFAULT_BANK_NAME;
	}
		
	//------------------------------------------------------------------------------------------------
	protected bool ConsumeRequiredItem(IEntity userEntity, ResourceName itemPrefab, string itemType)
	{
		if (!userEntity || itemPrefab == "")
			return true;
			
		int removedCount = RL_InventoryUtils.RemoveAmount(userEntity, itemPrefab, 1);
		if (removedCount < 1)
			return false;

		return true;
	}
		
	//------------------------------------------------------------------------------------------------
	protected void StartUpdateLoop()
	{
		if (!m_timerActive)
		{
			m_timerActive = true;
			GetGame().GetCallqueue().CallLater(UpdateProgress, UPDATE_INTERVAL_MS);
	}
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool RplSave(ScriptBitWriter writer)
	{
		bool result = super.RplSave(writer);
		writer.WriteBool(m_alarmDisabled);
		writer.WriteBool(m_isOpen);
		writer.WriteBool(m_alarmTriggered);
		writer.WriteInt(m_currentOperation);
		writer.WriteFloat(m_operationStartTime);
		writer.WriteInt(m_operationDuration);
		writer.WriteInt(m_operationTimeRemaining);
		writer.WriteFloat(m_alarmScheduledTime);
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool RplLoad(ScriptBitReader reader)
	{
		bool result = super.RplLoad(reader);
		reader.ReadBool(m_alarmDisabled);
		reader.ReadBool(m_isOpen);
		reader.ReadBool(m_alarmTriggered);
		reader.ReadInt(m_currentOperation);
		reader.ReadFloat(m_operationStartTime);
		reader.ReadInt(m_operationDuration);
		reader.ReadInt(m_operationTimeRemaining);
		reader.ReadFloat(m_alarmScheduledTime);
		UpdateLocalState();
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateLocalState()
	{
		if (m_alarmTriggered && !IsAlarmSoundPlaying())
		{
			PlayAlarmRpc();
		}
		UpdateOperationVisuals();
	}
		
	//------------------------------------------------------------------------------------------------
	protected void UpdateOperationVisuals()
	{
		if (!Replication.IsServer())
			return;
		
		if (m_currentOperation == RL_BankOperationType.DRILL)
		{
			SpawnDrill();
		}
		else
		{
			DeleteDrill();
		}
	}
		
	//------------------------------------------------------------------------------------------------
	protected void SpawnDrill()
	{
		if (!Replication.IsServer() || m_spawnedDrill)
			return;
		
		SCR_BankDoorComponentClass settings = GetSettings();
		if (!settings || !settings.m_DrillSpawnLocation)
			return;
		
		settings.m_DrillSpawnLocation.Init(GetOwner());
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		settings.m_DrillSpawnLocation.GetModelTransform(spawnParams.Transform);
		spawnParams.Transform[3] = GetOwner().CoordToParent(spawnParams.Transform[3]);
		Resource drillPrefab = Resource.Load(DRILL_PREFAB_PATH);

		if (!drillPrefab)
			return;
		
		m_spawnedDrill = GetGame().SpawnEntityPrefab(drillPrefab, GetGame().GetWorld(), spawnParams);
		if (m_spawnedDrill)
		{
			GetOwner().AddChild(m_spawnedDrill, -1, EAddChildFlags.NONE);
			
			if (m_spawnedDrill.GetPhysics())
				m_spawnedDrill.GetPhysics().SetActive(true);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	protected void DeleteDrill()
	{
		if (!Replication.IsServer())
			return;
		
		CleanupDrill();
	}
		
	//------------------------------------------------------------------------------------------------
	protected void CleanupDrill()
	{
		if (m_spawnedDrill)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(m_spawnedDrill);
			m_spawnedDrill = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateProgress()
	{
		if (!Replication.IsServer())
			return;
			
		float currentTime = GetCurrentTime();
		bool needsUpdate = false;
		
		if (m_currentOperation != RL_BankOperationType.NONE && !m_isOpen)
		{
			int newTimeRemaining = CalculateTimeRemaining(m_operationStartTime, m_operationDuration);
			
			if (newTimeRemaining != m_operationTimeRemaining)
			{
				m_operationTimeRemaining = newTimeRemaining;
				needsUpdate = true;
			}
			
			if (CalculateElapsedTime(m_operationStartTime) >= m_operationDuration)
			{
				OnOperationComplete();
				needsUpdate = true;
			}
		}
		
		if (m_alarmScheduledTime > 0 && !m_alarmTriggered)
		{
			if (currentTime >= m_alarmScheduledTime)
			{
				m_alarmScheduledTime = 0;
				TriggerAlarm();
				needsUpdate = true;
			}
		}
		
		if (needsUpdate)
		{
			Replication.BumpMe();
		}
		
		if (m_timerActive)
		{
			GetGame().GetCallqueue().CallLater(UpdateProgress, UPDATE_INTERVAL_MS);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void StartOperation(RL_BankOperationType operationType, IEntity userEntity = null)
	{
		if (!Replication.IsServer())
			return;
			
		if (m_currentOperation != RL_BankOperationType.NONE)
			return;
		
		SCR_BankDoorComponentClass settings = GetSettings();
		ResourceName requiredItemPrefab = "";
		int duration = DEFAULT_DRILL_TIME;
		
		if (operationType == RL_BankOperationType.DRILL)
		{
			if (settings)
			{
				requiredItemPrefab = settings.m_DrillItemPrefab;
				duration = settings.m_drillTime;
			}
		}
		else if (operationType == RL_BankOperationType.EXPLOSION)
		{
			duration = DEFAULT_COUNTDOWN_TIME;
			if (settings)
			{
				requiredItemPrefab = settings.m_ExplosiveItemPrefab;
				duration = settings.m_explosionCountdownTime;
			}
		}
		
		if (!ConsumeRequiredItem(userEntity, requiredItemPrefab, "required item"))
			return;
		
		m_currentOperation = operationType;
		m_operationStartTime = GetCurrentTime();
		m_operationDuration = duration;
		m_operationTimeRemaining = duration;
		
		StartUpdateLoop();
		UpdateOperationVisuals();
		
		if (m_alarmDisabled && operationType == RL_BankOperationType.DRILL)
		{
			int alarmDelay = DEFAULT_ALARM_DELAY;
			if (settings)
				alarmDelay = settings.m_disarmAlarmDelay;
			m_alarmScheduledTime = GetCurrentTime() + (alarmDelay * 1000);
		}
		else
		{
			TriggerAlarm();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnOperationComplete()
	{
		RL_BankOperationType completedOperation = m_currentOperation;
		
		if (completedOperation == RL_BankOperationType.EXPLOSION)
		{
			SCR_BankDoorComponentClass settings = GetSettings();
			if (settings && settings.m_ExplosionEffectPrefab != "")
			{
				vector position = GetOwner().GetOrigin();
				Resource explosionResource = Resource.Load(settings.m_ExplosionEffectPrefab);
				
				if (explosionResource)
				{
					EntitySpawnParams params = EntitySpawnParams();
					params.TransformMode = ETransformMode.WORLD;
					GetOwner().GetTransform(params.Transform);
					
					IEntity explosion = GetGame().SpawnEntityPrefab(explosionResource, GetOwner().GetWorld(), params);
				}
			}
		}
		
		m_currentOperation = RL_BankOperationType.NONE;
		m_isOpen = true;
		m_operationTimeRemaining = 0;
		
		UpdateOperationVisuals();
		CheckIfTimerStillNeeded();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CheckIfTimerStillNeeded()
	{
		bool stillNeeded = (m_currentOperation != RL_BankOperationType.NONE) || (m_alarmScheduledTime > 0 && !m_alarmTriggered);
		
		if (!stillNeeded && m_timerActive)
		{
			m_timerActive = false;
			GetGame().GetCallqueue().Remove(UpdateProgress);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void TriggerAlarm()
	{
		if (m_alarmTriggered)
			return;
			
		m_alarmTriggered = true;
		m_alarmScheduledTime = 0;
		
		PlayAlarmRpc();
		CreateBankRobberyMarker();
		AlertAllPolice();
		
		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_MajorCrimeManagerComponent majorCrimeManager = RL_MajorCrimeManagerComponent.Cast(gameMode.FindComponent(RL_MajorCrimeManagerComponent));
		if (majorCrimeManager)
			majorCrimeManager.SetMajorCrimeCommitted();
		
		CheckIfTimerStillNeeded();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void PlayAlarmRpc()
	{
		Rpc(RpcDo_PlayAlarm);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_PlayAlarm()
	{
		PlayAlarmSound();
	}
	
	//------------------------------------------------------------------------------------------------
	void PlayAlarmSound()
	{
		SCR_BankDoorComponentClass settings = GetSettings();
		if (!settings || !settings.m_AlarmAudioSourceConfiguration || !settings.m_AlarmAudioSourceConfiguration.IsValid())
			return;
		
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (!soundManagerEntity)
			return;
		
		SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(GetOwner(), settings.m_AlarmAudioSourceConfiguration);
		if (!audioSource)

			return;
		
		vector mat[4];
		GetOwner().GetTransform(mat);
		mat[3] = GetOwner().CoordToParent(settings.m_vAlarmSoundOffset);
		
		soundManagerEntity.PlayAudioSource(audioSource, mat);
		m_AlarmAudioHandle = audioSource.m_AudioHandle;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsAlarmSoundPlaying()
	{
		return AudioSystem.IsSoundPlayed(m_AlarmAudioHandle);
	}
	

	
	//------------------------------------------------------------------------------------------------
	void SetAlarmDisabled(bool value)
	{
		if (!Replication.IsServer())
			return;
			
		m_alarmDisabled = value;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOperationTimeRemaining()
	{
		if (m_currentOperation == RL_BankOperationType.NONE)
			return 0;
		
		if (!Replication.IsServer())
			return m_operationTimeRemaining;
		
		return CalculateTimeRemaining(m_operationStartTime, m_operationDuration);
	}
	
	//------------------------------------------------------------------------------------------------
	void Reset()
	{
		if (!Replication.IsServer())
			return;
	
		DeleteBankRobberyMarker();
		// Cancel any active timers
		m_timerActive = false;
		GetGame().GetCallqueue().Remove(UpdateProgress);
		
		// Reset all variables
		m_alarmDisabled = false;
		m_isOpen = false;
		m_alarmTriggered = false;
		m_currentOperation = RL_BankOperationType.NONE;
		m_operationStartTime = 0;
		m_operationDuration = 0;
		m_operationTimeRemaining = 0;
		m_alarmScheduledTime = 0;
		
		UpdateOperationVisuals();
		
		// Reset cash drawers, look for cash manager in parent hierarchy
		IEntity parentEntity = GetOwner().GetParent();
		while (parentEntity)
		{
			Print("[BankDoorComponent] Reset while loop");
			RL_BankCashManagerComponent cashManager = RL_BankCashManagerComponent.Cast(parentEntity.FindComponent(RL_BankCashManagerComponent));
			if (cashManager)
			{
				cashManager.ResetAllDrawers();
				break;
			}
			parentEntity = parentEntity.GetParent();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsDrilling()
	{
		return m_currentOperation == RL_BankOperationType.DRILL;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsOpen()
	{
		return m_isOpen;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsAlarmTriggered()
	{
		return m_alarmTriggered;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsBlownUp()
	{
		return m_currentOperation == RL_BankOperationType.EXPLOSION && m_isOpen;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsExplosionCountdown()
	{
		return m_currentOperation == RL_BankOperationType.EXPLOSION;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanDoorBeOpened()
	{
		bool canOpen = m_isOpen && m_currentOperation == RL_BankOperationType.NONE;
		return canOpen;
	}
	
	//------------------------------------------------------------------------------------------------
	void AlertAllPolice()
	{
		if (!Replication.IsServer())
			return;
		
		string bankName = GetBankName();
		RL_Utils.NotifyAllPolice(string.Format("BANK ALARM TRIGGERED at %1. Location marked on your map.", bankName), "DISPATCH");
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateBankRobberyMarker()
	{
		if (!Replication.IsServer())
			return;
		
		DeleteBankRobberyMarker();
		
		string bankName = GetBankName();
		string markerText = string.Format("BANK ROBBERY IN PROGRESS - %1", bankName);
		
		m_bankRobberyMarker = RL_MapUtils.CreateMarkerFromPrefabServer(
			GetOwner().GetOrigin(),
			ROBBERY_MARKER_PATH,
			markerText,
			MARKER_DURATION
		);
	}
	
	//------------------------------------------------------------------------------------------------
	void DeleteBankRobberyMarker()
	{
		if (m_bankRobberyMarker)
		{
			RL_MapUtils.DeleteMarkerEntity(m_bankRobberyMarker);
			m_bankRobberyMarker = null;
		}
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_BankDoorComponent()
	{
		DeleteBankRobberyMarker();
		CleanupDrill();
		if (m_timerActive)
		{
			GetGame().GetCallqueue().Remove(UpdateProgress);
		}
	}
}