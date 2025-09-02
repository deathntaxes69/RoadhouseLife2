enum RL_LAP_STATUS
{
	WaitingToStart,
	InProgress,
	Completed,
	Failed
}

class RL_LapTimer
{
	protected string m_sCharacterId;
	protected string m_sTrackName;
	protected int m_iTotalCheckpoints;
	protected int m_iCurrentCheckpoint;
	protected float m_fLapStartTime;
	protected float m_fLastCheckpointTime;
	protected RL_LAP_STATUS m_eStatus;
	protected ref array<float> m_aCheckpointTimes;
	protected string m_sVehicleName;
	protected bool m_bFirstAsFinish;
	
	void RL_LapTimer(string characterId, string trackName, int totalCheckpoints)
	{
		m_sCharacterId = characterId;
		m_sTrackName = trackName;
		m_iTotalCheckpoints = totalCheckpoints;
		m_iCurrentCheckpoint = 0;
		m_fLapStartTime = 0;
		m_fLastCheckpointTime = 0;
		m_eStatus = RL_LAP_STATUS.WaitingToStart;
		m_aCheckpointTimes = new array<float>;
		m_sVehicleName = "";
		m_bFirstAsFinish = false;
	}
	
	string GetCharacterId() { return m_sCharacterId; }
	string GetTrackName() { return m_sTrackName; }
	int GetTotalCheckpoints() { return m_iTotalCheckpoints; }
	int GetCurrentCheckpoint() { return m_iCurrentCheckpoint; }
	RL_LAP_STATUS GetStatus() { return m_eStatus; }
	string GetVehicleName() { return m_sVehicleName; }
	
	void SetVehicleName(string vehicleName) { m_sVehicleName = vehicleName; }
	
	bool ProcessCheckpoint(int checkpointNumber, bool firstAsFinish = false)
	{
		if (m_eStatus == RL_LAP_STATUS.WaitingToStart)
		{
			m_bFirstAsFinish = firstAsFinish;
		}
		
		float currentTime = GetGame().GetWorld().GetWorldTime() / 1000.0;
		
		// Starting a new lap, must hit checkpoint 1 first
		if (m_eStatus == RL_LAP_STATUS.WaitingToStart)
		{
			if (checkpointNumber != 1)
				return false;
				
			StartLap(currentTime);
			m_fLastCheckpointTime = currentTime;
			return true;
		}
		
		// In progress, check its the right checkpoint
		if (m_eStatus == RL_LAP_STATUS.InProgress)
		{
			int expectedNext = m_iCurrentCheckpoint + 1;
			m_fLastCheckpointTime = currentTime;
			
			// If first checkpoint is also finish and we passed all others
			if (firstAsFinish && checkpointNumber == 1 && m_iCurrentCheckpoint == m_iTotalCheckpoints)
			{
				// Complete the lap by hitting checkpoint 1 again
				m_aCheckpointTimes.Insert(currentTime - m_fLapStartTime);
				CompleteLap();
				return true;
			}
			
			if (expectedNext > m_iTotalCheckpoints)
			{
				// This shouldn't happen if the checkpoints are setup right
				FailLap();
				return false;
			}
			
			if (checkpointNumber != expectedNext)
			{
				// Skipped or wrong checkpoint, fail the lap
				FailLap();
				return false;
			}
			
			// Correct checkpoint
			m_iCurrentCheckpoint = checkpointNumber;
			m_aCheckpointTimes.Insert(currentTime - m_fLapStartTime);
			
			// Check if all are completed (only if not using first as finish)
			if (m_iCurrentCheckpoint == m_iTotalCheckpoints && !m_bFirstAsFinish)
			{
				CompleteLap();
			}
			
			return true;
		}
		
		return false;
	}
	
	void StartLap(float startTime)
	{
		m_fLapStartTime = startTime;
		m_eStatus = RL_LAP_STATUS.InProgress;
		m_iCurrentCheckpoint = 1;
		m_aCheckpointTimes.Clear();
		m_aCheckpointTimes.Insert(0); // Checkpoint 1 time is always 0
	}
	
	void CompleteLap()
	{
		m_eStatus = RL_LAP_STATUS.Completed;
		float lapTime = GetLapTime();
	}
	
	void FailLap()
	{
		m_eStatus = RL_LAP_STATUS.Failed;
	}
	
	float GetLapTime()
	{
		if (m_aCheckpointTimes.Count() == 0)
			return 0;
			
		return m_aCheckpointTimes[m_aCheckpointTimes.Count() - 1];
	}

	float GetLastCheckpointTime()
	{
		if (m_fLastCheckpointTime == 0)
			return -1;

		float currentTime = GetGame().GetWorld().GetWorldTime() / 1000.0;
		return currentTime - m_fLastCheckpointTime;
	}
	
	float GetElapsedTime()
	{
		if (m_eStatus != RL_LAP_STATUS.InProgress)
			return 0;
			
		float currentTime = GetGame().GetWorld().GetWorldTime() / 1000.0;
		return currentTime - m_fLapStartTime;
	}
	
	string GetStatusString()
	{
		switch (m_eStatus)
		{
			case RL_LAP_STATUS.WaitingToStart: return "Waiting to Start";
			case RL_LAP_STATUS.InProgress: return string.Format("In Progress (%1/%2)", m_iCurrentCheckpoint, m_iTotalCheckpoints);
			case RL_LAP_STATUS.Completed: return string.Format("Completed - %1", FormatTime(GetLapTime()));
			case RL_LAP_STATUS.Failed: return "Failed";
		}
		return "Unknown";
	}
	
	string GetProgressString()
	{
		if (m_eStatus == RL_LAP_STATUS.WaitingToStart)
			return string.Format("Drive through checkpoint 1 to start lap on %1", m_sTrackName);
		else if (m_eStatus == RL_LAP_STATUS.InProgress)
		{
			int nextCheckpoint = m_iCurrentCheckpoint + 1;
			
			// Special case for first-as-finish: after hitting all checkpoints, return to checkpoint 1
			if (m_bFirstAsFinish && m_iCurrentCheckpoint == m_iTotalCheckpoints)
			{
				return string.Format("Return to start to finish! | Elapsed: %1s", FormatTime(GetElapsedTime()));
			}
			else if (nextCheckpoint > m_iTotalCheckpoints)
			{
				return string.Format("Final checkpoint reached! | Elapsed: %1s", FormatTime(GetElapsedTime()));
			}
			else
			{
				return string.Format("Next: Checkpoint %1 | Elapsed: %2s", nextCheckpoint, FormatTime(GetElapsedTime()));
			}
		}
		else if (m_eStatus == RL_LAP_STATUS.Completed)
			return string.Format("Lap completed in %1!", FormatTime(GetLapTime()));
		else
			return "Lap failed - drive through checkpoint 1 to restart";
	}
	
	string FormatTime(float timeSeconds)
	{
		// mm:ss.cc
		int minutes = Math.Floor(timeSeconds / 60);
		float remainingSeconds = timeSeconds - (minutes * 60);
		int seconds = Math.Floor(remainingSeconds);
		int centiseconds = Math.Round((remainingSeconds - seconds) * 100);
		
		// Handle centiseconds overflow
		if (centiseconds >= 100)
		{
			centiseconds = 0;
			seconds++;
			if (seconds >= 60)
			{
				seconds = 0;
				minutes++;
			}
		}
		
		string minutesStr = string.Format("%1", minutes);
		if (minutesStr.Length() < 2)
			minutesStr = "0" + minutesStr;
			
		string secondsStr = string.Format("%1", seconds);
		if (secondsStr.Length() < 2)
			secondsStr = "0" + secondsStr;
			
		string centisecondsStr = string.Format("%1", centiseconds);
		if (centisecondsStr.Length() < 2)
			centisecondsStr = "0" + centisecondsStr;
		
		return string.Format("%1:%2.%3", minutesStr, secondsStr, centisecondsStr);
	}
} 