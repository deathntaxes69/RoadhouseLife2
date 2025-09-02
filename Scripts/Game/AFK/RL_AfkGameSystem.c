class RL_AfkGameSystem : GameSystem
{
	protected int m_iPlayerCountBeforeKick = 0;
	protected int m_iKickTime = 1200;
	protected ref array<int> m_aWarningsArray = {300, 120, 60};
	protected ref array<bool> m_aWarningsTriggered = {}; // Track which warnings have been sent

	protected int m_iLastMovementTime;  // Changed back to int
	protected World m_wActiveWorld;
	protected ActionManager m_amInputListener;

	protected PlayerManager m_playerManager;

	//------------------------------------------------------------------------------------------------
	override void OnInit()
	{
		Print("AFK OnInit");
		m_iLastMovementTime = System.GetUnixTime();  // Now consistent with int type
		
		// Initialize warnings triggered array with same size as warnings array
		m_aWarningsTriggered.Clear();
		for (int i = 0; i < m_aWarningsArray.Count(); i++)
		{
			m_aWarningsTriggered.Insert(false);
		}
		
		GetGame().GetCallqueue().CallLater(DelayedInit, 5000);
	}

	//------------------------------------------------------------------------------------------------
	protected void DelayedInit()
	{
		m_playerManager = GetGame().GetPlayerManager();
		m_amInputListener = GetGame().GetInputManager();
		if(m_amInputListener)
			GetGame().GetInputManager().AddActionListener("RL_AfkMovement", EActionTrigger.VALUE, UpdateMovement);
	}

	protected int m_iUpdateInterval = 5;
	protected int m_iLastUpdateTime = 0;	
	
	override void OnUpdate(ESystemPoint point)
	{
		int currentTime = System.GetUnixTime();

		// throttle updates
		if ((currentTime - m_iLastUpdateTime) < m_iUpdateInterval)
			return;
		
		m_iLastUpdateTime = currentTime;

		int playerId = SCR_PlayerController.GetLocalPlayerId();

		if(SCR_Global.IsAdmin(playerId))
		{
			Enable(false);
			Print("RL_AfkGameSystem ignoring admin");
			return;
		}
		int timeSinceLastMovement = currentTime - m_iLastMovementTime;
		if (!m_playerManager)
		{
			Print("RL_AfkGameSystem no player manager");
			return;
		}
		//Print(string.Format("timeSinceLastMovement %1", timeSinceLastMovement));

		int activePlayers = m_playerManager.GetPlayerCount();

		// Check if it's time to kick
		if (m_iKickTime <= timeSinceLastMovement)
		{
			if (activePlayers > m_iPlayerCountBeforeKick)
			{
				GameStateTransitions.RequestGameplayEndTransition();
			}
		}
		else
		{
			// Check multiple warning thresholds
			CheckAndSendWarnings(timeSinceLastMovement, activePlayers);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void CheckAndSendWarnings(int timeSinceLastMovement, int activePlayers)
	{
		if (activePlayers <= m_iPlayerCountBeforeKick)
			return;

		// Check each warning threshold
		for (int i = 0; i < m_aWarningsArray.Count(); i++)
		{
			int warningThreshold = m_iKickTime - m_aWarningsArray[i];
			// If we've reached this warning threshold and haven't sent this warning yet
			if (timeSinceLastMovement >= warningThreshold && !m_aWarningsTriggered[i])
			{
				m_aWarningsTriggered[i] = true;
				int minutesRemaining = m_aWarningsArray[i] / 60;
				string warningMessage = string.Format("You will be kicked in %1 minute(s) for inactivity!", minutesRemaining.ToString());
				RL_Utils.Notify(warningMessage, "AFK KICK WARNING", 10);
				Print(string.Format("AFK Warning sent: %1 minutes remaining", minutesRemaining.ToString()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void ResetWarnings()
	{
		// Reset all warning flags when player moves
		for (int i = 0; i < m_aWarningsTriggered.Count(); i++)
		{
			m_aWarningsTriggered[i] = false;
		}
	}
	protected float m_fTimer = 0;
	protected float m_fCheckInterval = 6;
	protected int m_iUnderMapCount = 0;
	protected const int m_iUnderMapLimit = 2;
	protected const float m_iUnderMapThreshold = -1;
	protected int m_iTeleportTries = 0;
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateMovement(float inputValue)
	{

		if (!float.AlmostEqual(inputValue, 0.0, 0.001))
		{
			m_iLastMovementTime = System.GetUnixTime();
			ResetWarnings();
		}
		/*
		float timeSlice = GetWorld().GetFixedTimeSlice();

		m_fTimer += timeSlice;

		if (m_fTimer < m_fCheckInterval)
			return;

		m_fTimer = 0;

		PlayerController pc = GetGame().GetPlayerController();
		if(!pc) return;
		IEntity player = pc.GetControlledEntity();
		if(!player) return;
		vector origin = player.GetOrigin();

		if (origin[1] < m_iUnderMapThreshold)
		{
			m_iUnderMapCount++;

			PrintFormat("[UnderMapCheck] Player below threshold (%.2f). Count: %1/%2", origin[1], m_iUnderMapCount, m_iUnderMapLimit);

			if (m_iUnderMapCount >= m_iUnderMapLimit && m_iTeleportTries < 5)
			{
				Print("[UnderMapCheck] Player was under the map 5 times!");
				origin[1] = GetGame().GetWorld().GetSurfaceY(origin[0], origin[2]);
				RL_Utils.TeleportPlayer(RL_Utils.GetLocalCharacter(), origin);
				m_iUnderMapCount = 0;
				m_iTeleportTries = m_iTeleportTries + 1;
			}
		}
		else
		{
			//reset count if they are above ground
			m_iUnderMapCount = 0;
			m_iTeleportTries = 0;
		}*/

	}

	//------------------------------------------------------------------------------------------------
	override void OnStopped()
	{
		if (m_amInputListener)
			m_amInputListener.RemoveActionListener("RL_AfkMovement", EActionTrigger.VALUE, UpdateMovement);
	}
}