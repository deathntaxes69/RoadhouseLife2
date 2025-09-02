class RL_LapTimerManager
{
	protected static ref RL_LapTimerManager s_Instance;
	protected ref map<string, ref RL_LapTimer> m_mActiveTimers;
	protected ref array<RL_RaceCheckpointEntity> m_aCheckpoints;
	protected ref map<string, ref array<RL_RaceCheckpointEntity>> m_mTrackCheckpoints;
	protected ref RL_LapRecordsDbHelper m_lapRecordsHelper;
	
	protected float m_fPendingLapTime;
	protected string m_sPendingVehicleName;
	protected string m_sPendingTrackName;
	protected SCR_ChimeraCharacter m_pendingCharacter;
	
	static RL_LapTimerManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new RL_LapTimerManager();
		return s_Instance;
	}
	
	void RL_LapTimerManager()
	{
		m_mActiveTimers = new map<string, ref RL_LapTimer>;
		m_aCheckpoints = new array<RL_RaceCheckpointEntity>;
		m_mTrackCheckpoints = new map<string, ref array<RL_RaceCheckpointEntity>>;
		if (!m_lapRecordsHelper) m_lapRecordsHelper = new RL_LapRecordsDbHelper();
	}
	
	void RegisterCheckpoint(RL_RaceCheckpointEntity checkpoint)
	{
		if (!checkpoint || m_aCheckpoints.Contains(checkpoint))
			return;
		
		m_aCheckpoints.Insert(checkpoint);
		
		string trackName = checkpoint.GetTrackName();
		if (!m_mTrackCheckpoints.Contains(trackName))
			m_mTrackCheckpoints.Insert(trackName, new array<RL_RaceCheckpointEntity>);
		
		array<RL_RaceCheckpointEntity> trackCheckpoints = m_mTrackCheckpoints.Get(trackName);
		if (!trackCheckpoints.Contains(checkpoint))
			trackCheckpoints.Insert(checkpoint);
	}
	
	void UnregisterCheckpoint(RL_RaceCheckpointEntity checkpoint)
	{
		if (!checkpoint)
			return;
		
		int index = m_aCheckpoints.Find(checkpoint);
		if (index != -1)
			m_aCheckpoints.Remove(index);
		
		string trackName = checkpoint.GetTrackName();
		if (m_mTrackCheckpoints.Contains(trackName))
		{
			array<RL_RaceCheckpointEntity> trackCheckpoints = m_mTrackCheckpoints.Get(trackName);
			int trackIndex = trackCheckpoints.Find(checkpoint);
			if (trackIndex != -1)
				trackCheckpoints.Remove(trackIndex);
		}
	}
	
	void ProcessCheckpointHit(SCR_ChimeraCharacter character, IEntity vehicle, string vehicleName, string trackName, int checkpointNumber, int totalCheckpoints, bool firstAsFinish = false)
	{
			
		if (!character || !trackName)
			return;
		
		string characterId = character.GetCharacterId();
		if (!characterId)
		{
			return;
		}
		
		// Get or create lap timer for this character
		RL_LapTimer lapTimer = GetOrCreateLapTimer(characterId, trackName, totalCheckpoints);
		if (!lapTimer)
		{
			return;
		}
		
		// Set vehicle name for this lap
		lapTimer.SetVehicleName(vehicleName);
		
		// Process the checkpoint
		bool success = lapTimer.ProcessCheckpoint(checkpointNumber, firstAsFinish);
		
		if (!success && lapTimer.GetStatus() == RL_LAP_STATUS.WaitingToStart)
		{
			return;
		}
		
		string message = "";
		if (success)
		{
			if (lapTimer.GetStatus() == RL_LAP_STATUS.Completed)
			{
				
				SubmitLapTimeToDatabase(character, trackName, vehicleName, lapTimer.GetLapTime());
				
				// For first as finish tracks, start a new lap right away
				if (firstAsFinish && checkpointNumber == 1)
				{
					ResetLapTimer(characterId);
					RL_LapTimer newTimer = GetOrCreateLapTimer(characterId, trackName, totalCheckpoints);
					if (newTimer)
					{
						newTimer.SetVehicleName(vehicleName);
						newTimer.ProcessCheckpoint(1, firstAsFinish);
					}
				}
				else
				{
					ResetLapTimer(characterId);
				}
				
				return;
			}
			else
			{
				message = lapTimer.GetProgressString();
			}
		}
		else
		{
			message = "CHECKPOINT SKIPPED!\nLap failed - drive through checkpoint 1 to restart";
			ResetLapTimer(characterId);
		}
		
		if (!message.IsEmpty())
		{
			character.Notify(message, "LAP TIMER", 5);
		}
	}
	
	protected void SubmitLapTimeToDatabase(SCR_ChimeraCharacter character, string trackName, string vehicleName, float lapTime)
	{
		if (!character || !m_lapRecordsHelper)
		{
			return;
		}
		
		string uid = character.GetIdentityId();
		string cid = character.GetCharacterId();
		
		if (!uid || !cid)
		{
			return;
		}
		
		m_fPendingLapTime = lapTime;
		m_sPendingVehicleName = vehicleName;
		m_sPendingTrackName = trackName;
		m_pendingCharacter = character;
		
		m_lapRecordsHelper.SubmitLapTime(uid, cid.ToInt(), trackName, vehicleName, lapTime, this, "OnLapTimeSubmitted");
	}
	
	void OnLapTimeSubmitted(bool success, string response)
	{
		if (!m_pendingCharacter)
		{
			return;
		}
		
		if (!success)
		{
			
			RL_LapTimer tempTimer = new RL_LapTimer("", "", 1);
			string fallbackMessage = string.Format("LAP COMPLETED!\nTime: %1\nVehicle: %2\nTrack: %3", 
				tempTimer.FormatTime(m_fPendingLapTime), m_sPendingVehicleName, m_sPendingTrackName);
			m_pendingCharacter.Notify(fallbackMessage, "LAP TIMER", 5);
			return;
		}
		
		
		RL_LapTimer tempTimer = new RL_LapTimer("", "", 1);
		string formattedTime = tempTimer.FormatTime(m_fPendingLapTime);
		
		string completionMessage = string.Format("LAP COMPLETED!\nTime: %1\nVehicle: %2\nTrack: %3", 
			formattedTime, m_sPendingVehicleName, m_sPendingTrackName);
		
		SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
		if (!jsonCtx.ImportFromString(response))
		{
			m_pendingCharacter.Notify(completionMessage, "LAP TIMER", 5);
			return;
		}
		
		string status;
		if (!jsonCtx.ReadValue("status", status))
		{
			m_pendingCharacter.Notify(completionMessage, "LAP TIMER", 5);
			return;
		}
		
		string recordMessage = "";
		if (status == "record_updated")
		{
			float previousTime, improvement;
			if (jsonCtx.ReadValue("previous_time", previousTime) && 
				jsonCtx.ReadValue("improvement", improvement))
			{
				recordMessage = string.Format("\n\nNEW PERSONAL BEST!\nPrevious: %1\nImprovement: %2", 
					tempTimer.FormatTime(previousTime), tempTimer.FormatTime(improvement));
			}
			else
			{
				recordMessage = "\n\nNEW PERSONAL BEST!";
			}
		}
		else if (status == "new_record")
		{
			recordMessage = "\n\nFIRST LAP RECORD SET!";
		}
		
		string finalMessage = completionMessage + recordMessage;
		int duration;
		string title;
		
		if (recordMessage.IsEmpty())
		{
			duration = 5;
			title = "LAP TIMER";
		}
		else
		{
			duration = 8;
			title = "LAP RECORD";
		}

		m_pendingCharacter.Notify(finalMessage, title, duration);
	}
	
	RL_LapTimer GetOrCreateLapTimer(string characterId, string trackName, int totalCheckpoints)
	{
		
		if (m_mActiveTimers.Contains(characterId))
		{
			RL_LapTimer existingTimer = m_mActiveTimers.Get(characterId);
			
			if (existingTimer.GetTrackName() != trackName)
			{
				m_mActiveTimers.Remove(characterId);
				RL_LapTimer newTimer = new RL_LapTimer(characterId, trackName, totalCheckpoints);
				m_mActiveTimers.Insert(characterId, newTimer);
				return newTimer;
			}
			
			return existingTimer;
		}
		
		RL_LapTimer newTimer = new RL_LapTimer(characterId, trackName, totalCheckpoints);
		m_mActiveTimers.Insert(characterId, newTimer);
		return newTimer;
	}
	
	void ResetLapTimer(string characterId)
	{
		if (!m_mActiveTimers.Contains(characterId))
			return;
		
		RL_LapTimer timer = m_mActiveTimers.Get(characterId);
		if (!timer)
			return;
		
		RL_LapTimer newTimer = new RL_LapTimer(characterId, timer.GetTrackName(), timer.GetTotalCheckpoints());
		m_mActiveTimers.Set(characterId, newTimer);
	}
	
	RL_LapTimer GetActiveTimerForCharacter(string characterId)
	{
		if (!m_mActiveTimers.Contains(characterId))
			return null;
			
		return m_mActiveTimers.Get(characterId);
	}
	
	void RemoveCharacterTimer(string characterId)
	{
		if (m_mActiveTimers.Contains(characterId))
			m_mActiveTimers.Remove(characterId);
	}
	
	array<string> GetAvailableTracks()
	{
		array<string> trackNames = new array<string>;
		foreach (string trackName, array<RL_RaceCheckpointEntity> checkpoints : m_mTrackCheckpoints)
		{
			Print("[RL_LapTimerManager] GetAvailableTracks foreach");
			trackNames.Insert(trackName);
		}
		return trackNames;
	}
	
	bool IsTrackValid(string trackName)
	{
		if (!m_mTrackCheckpoints.Contains(trackName))
			return false;
		
		array<RL_RaceCheckpointEntity> checkpoints = m_mTrackCheckpoints.Get(trackName);
		if (!checkpoints || checkpoints.Count() == 0)
			return false;
		
		// Verify all checkpoint numbers are present
		int expectedTotal = checkpoints[0].GetTotalCheckpoints();
		array<bool> numbersFound = new array<bool>;
		numbersFound.Resize(expectedTotal);
		
		foreach (RL_RaceCheckpointEntity checkpoint : checkpoints)
		{
			Print("[RL_LapTimerManager] IsTrackValid foreach");
			int number = checkpoint.GetCheckpointNumber();
			if (number >= 1 && number <= expectedTotal)
				numbersFound[number - 1] = true;
		}
		
		// Check if all numbers are found
		for (int i = 0; i < expectedTotal; i++)
		{
			Print("[RL_LapTimerManager] IsTrackValid for");
			if (!numbersFound[i])
			{
				return false;
			}
		}
		
		return true;
	}
	
	int GetActiveTimerCount()
	{
		return m_mActiveTimers.Count();
	}
} 