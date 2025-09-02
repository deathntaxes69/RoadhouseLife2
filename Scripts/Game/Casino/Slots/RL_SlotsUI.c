class RL_SlotSymbol
{
	string name;
	int multiplier;
	
	void RL_SlotSymbol(string symbolName, int symbolMultiplier)
	{
		name = symbolName;
		multiplier = symbolMultiplier;
	}
}

class RL_SlotReel
{
	ref array<int> symbolIndices = {};
	
	void RL_SlotReel()
	{
		symbolIndices = new array<int>();
	}
	
	void AddSymbol(int symbolIndex, int frequency = 1)
	{
		for (int i = 0; i < frequency; i++)
		{
			Print("[RL_SlotReel] AddSymbol for");
			symbolIndices.Insert(symbolIndex);
		}
	}
	
	int GetRandomSymbol()
	{
		if (symbolIndices.Count() == 0)
			return 0;
		
		return symbolIndices[Math.RandomInt(0, symbolIndices.Count())];
	}
}

class RL_SlotsUI : ChimeraMenuBase
{
	protected Widget m_wRoot;
	
	protected ImageWidget m_wSlotOne;
	protected ImageWidget m_wSlotTwo;
	protected ImageWidget m_wSlotThree;
	protected TextWidget m_wSpinAmount;
	protected TextWidget m_wStatus;
	protected TextWidget m_wCash;
	
	protected SCR_InputButtonComponent m_SpinButtonComponent;
	protected SCR_InputButtonComponent m_SpinDecreaseComponent;
	protected SCR_InputButtonComponent m_SpinIncreaseComponent;
	
	protected int m_minBet = 100;
	protected int m_maxBet = 1000;
	protected int m_betIncrement = 100;
	
	protected int m_currentBet = 100;
	protected bool m_isSpinning = false;
	protected int m_spinDuration = 3500;
	protected int m_cachedCash = 0;
	
	protected ref array<ref RL_SlotSymbol> m_slotSymbols = {};
	protected ref array<ref RL_SlotReel> m_slotReels = {};
	protected const ResourceName m_slotImageSet = "{0A09C46D79C6D698}UI/Imagesets/MapMarkers/mapMarkerIcons.imageset";
	
	protected ref array<int> m_currentSlots = {0, 0, 0};
	protected ref array<int> m_targetSlots = {0, 0, 0};
	
	protected int m_sessionStartTime;
	protected int m_sessionSpent = 0;
	protected int m_sessionNetProfit = 0;
	protected int m_sessionBiggestWin = 0;
	protected int m_sessionTotalSpins = 0;
	protected int m_sessionTotalWins = 0;
	protected int m_sessionTotalLosses = 0;
	protected int m_sessionTotalBets = 0;
	
	override void OnMenuOpen() 
	{
        m_wRoot = GetRootWidget();
		
		m_wSlotOne = ImageWidget.Cast(m_wRoot.FindAnyWidget("slotOne"));
		m_wSlotTwo = ImageWidget.Cast(m_wRoot.FindAnyWidget("slotTwo"));
		m_wSlotThree = ImageWidget.Cast(m_wRoot.FindAnyWidget("slotThree"));
		m_wSpinAmount = TextWidget.Cast(m_wRoot.FindAnyWidget("spinAmount"));
		m_wStatus = TextWidget.Cast(m_wRoot.FindAnyWidget("status"));
		m_wCash = TextWidget.Cast(m_wRoot.FindAnyWidget("cash"));
		
		m_SpinButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent("spinButton", m_wRoot);
		m_SpinDecreaseComponent = SCR_InputButtonComponent.GetInputButtonComponent("spinDecrease", m_wRoot);
		m_SpinIncreaseComponent = SCR_InputButtonComponent.GetInputButtonComponent("spinIncrease", m_wRoot);

		if (m_SpinButtonComponent)
			m_SpinButtonComponent.m_OnActivated.Insert(OnSpinButtonClicked);
		if (m_SpinDecreaseComponent)
			m_SpinDecreaseComponent.m_OnActivated.Insert(OnSpinDecreaseClicked);
		if (m_SpinIncreaseComponent)
			m_SpinIncreaseComponent.m_OnActivated.Insert(OnSpinIncreaseClicked);
		
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);

		UpdateBetDisplay();
		InitializeCashDisplay();
		UpdateStatusText("Ready");
		InitializeSessionTracking();
    }
	
	//------------------------------------------------------------------------------------------------
	void SetConfig(int minBet, int maxBet, int betIncrement)
	{
		m_minBet = minBet;
		m_maxBet = maxBet;
		m_betIncrement = betIncrement;
		
		m_currentBet = m_minBet;
		UpdateBetDisplay();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSymbolsAndReels(array<ref RL_SlotSymbol> symbols, array<ref RL_SlotReel> reels)
	{
		m_slotSymbols = symbols;
		m_slotReels = reels;
		InitializeSlots();
	}
	
	//------------------------------------------------------------------------------------------------
	void InitializeSlots()
	{
		for (int i = 0; i < 3; i++)
		{
			Print("[RL_SlotReel] InitializeSlots for");
			if (i < m_slotReels.Count())
				m_currentSlots[i] = m_slotReels[i].GetRandomSymbol();
			else
				m_currentSlots[i] = Math.RandomInt(0, m_slotSymbols.Count());
		}
		
		UpdateSlotImages();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSpinButtonClicked()
	{
		if (m_isSpinning)
			return;
		
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (!character)
		{
			UpdateStatusText("Error: Character not found");
			return;
		}
		
		if (character.GetCash() < m_currentBet)
		{
			UpdateStatusText("Not enough cash. Need $" + m_currentBet.ToString());
			return;
		}
		
		character.SpinSlotsSvr(character.GetCharacterId(), m_currentBet);
		m_isSpinning = true;
		UpdateStatusText("Processing bet...");
		SetButtonsEnabled(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSpinDecreaseClicked()
	{
		if (m_isSpinning)
			return;
		
		if (m_currentBet > m_minBet)
		{
			m_currentBet -= m_betIncrement;
			UpdateBetDisplay();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSpinIncreaseClicked()
	{
		if (m_isSpinning)
			return;
		
		if (m_currentBet < m_maxBet)
		{
			m_currentBet += m_betIncrement;
			UpdateBetDisplay();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateBetDisplay()
	{
		if (m_wSpinAmount)
			m_wSpinAmount.SetText("$" + m_currentBet.ToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateStatusText(string text)
	{
		if (m_wStatus)
			m_wStatus.SetText(text);
	}
	
	//------------------------------------------------------------------------------------------------
	void InitializeCashDisplay()
	{
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character)
		{
			m_cachedCash = character.GetCash();
			UpdateCashDisplay();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateCashDisplay()
	{
		if (m_wCash)
		{
			m_wCash.SetText("$" + m_cachedCash.ToString());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void StartSpinAfterValidation()
	{
		UpdateStatusText("Spinning...");
		for (int i = 0; i < 3; i++)
		{
			if (i < m_slotReels.Count())
				m_targetSlots[i] = m_slotReels[i].GetRandomSymbol();
			else
				m_targetSlots[i] = Math.RandomInt(0, m_slotSymbols.Count());
		}
		GetGame().GetCallqueue().CallLater(SpinAnimation, 100);
		GetGame().GetCallqueue().CallLater(StopSpinServerValidated, m_spinDuration);
	}
	
	//------------------------------------------------------------------------------------------------
	void SpinAnimation()
	{
		if (!m_isSpinning)
			return;
		
		for (int i = 0; i < 3; i++)
		{
			Print("[RL_SlotReel] SpinAnimation for");
			if (i < m_slotReels.Count())
				m_currentSlots[i] = m_slotReels[i].GetRandomSymbol();
			else
				m_currentSlots[i] = Math.RandomInt(0, m_slotSymbols.Count());
		}
		
		UpdateSlotImages();
		
		GetGame().GetCallqueue().CallLater(SpinAnimation, 100);
	}
	
	//------------------------------------------------------------------------------------------------
	void StopSpinServerValidated()
	{
		m_isSpinning = false;
		
		m_currentSlots = m_targetSlots;
		UpdateSlotImages();
		
		int winnings = CheckWinnings();
		
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character && winnings > 0)
		{
			character.SlotsWinSvr(character.GetCharacterId(), winnings, m_currentSlots);
		}
		else 
		{
			UpdateStatusText("Try again!");
			TrackSpinResult(false, 0);
			SetButtonsEnabled(true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	int CheckWinnings()
	{
		int slot1 = m_currentSlots[0];
		int slot2 = m_currentSlots[1];
		int slot3 = m_currentSlots[2];
		
		// Check for three of a kind (jackpot)
		if (slot1 == slot2 && slot2 == slot3)
		{
			if (slot1 < m_slotSymbols.Count())
			{
				int symbolMultiplier = m_slotSymbols[slot1].multiplier;
				return m_currentBet * symbolMultiplier * 3;
			}
		}
		
		// Check for two of a kind
		if (slot1 == slot2 || slot2 == slot3 || slot1 == slot3)
		{
			int matchingSymbol = -1;
			if (slot1 == slot2) matchingSymbol = slot1;
			else if (slot2 == slot3) matchingSymbol = slot2;
			else if (slot1 == slot3) matchingSymbol = slot1;
			
			if (matchingSymbol >= 0 && matchingSymbol < m_slotSymbols.Count())
			{
				int symbolMultiplier = m_slotSymbols[matchingSymbol].multiplier;
				return m_currentBet * symbolMultiplier;
			}
		}
		
		return 0; // No win
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateSlotImages()
	{
		SetSlotImage(m_wSlotOne, m_currentSlots[0]);
		SetSlotImage(m_wSlotTwo, m_currentSlots[1]);
		SetSlotImage(m_wSlotThree, m_currentSlots[2]);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSlotImage(ImageWidget slotWidget, int symbolIndex)
	{
		if (!slotWidget || symbolIndex < 0 || symbolIndex >= m_slotSymbols.Count())
			return;
		
		string symbolName = m_slotSymbols[symbolIndex].name;
		
		slotWidget.LoadImageFromSet(0, m_slotImageSet, symbolName);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetButtonsEnabled(bool enabled)
	{
		if (m_SpinButtonComponent)
			m_SpinButtonComponent.SetEnabled(enabled);
		if (m_SpinDecreaseComponent)
			m_SpinDecreaseComponent.SetEnabled(enabled);
		if (m_SpinIncreaseComponent)
			m_SpinIncreaseComponent.SetEnabled(enabled);
	}
	
    void CloseMenu()
	{
		UpdateSlotsStatsOnClose();
		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void InitializeSessionTracking()
	{
		m_sessionStartTime = System.GetTickCount();
		m_sessionSpent = 0;
		m_sessionNetProfit = 0;
		m_sessionBiggestWin = 0;
		m_sessionTotalSpins = 0;
		m_sessionTotalWins = 0;
		m_sessionTotalLosses = 0;
		m_sessionTotalBets = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	void TrackSpinStart()
	{
		m_sessionSpent += m_currentBet;
		m_sessionNetProfit -= m_currentBet;
		m_sessionTotalSpins++;
		m_sessionTotalBets += m_currentBet;
	}
	
	//------------------------------------------------------------------------------------------------
	void TrackSpinResult(bool isWin, int winnings)
	{
		if (isWin)
		{
			m_sessionTotalWins++;
			m_sessionNetProfit += winnings;
			
			if (winnings > m_sessionBiggestWin)
				m_sessionBiggestWin = winnings;
		}
		else
		{
			m_sessionTotalLosses++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateSlotsStatsOnClose()
	{
		if (m_sessionTotalSpins == 0)
			return;
		
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (!character)
		{
			Print("RL_SlotsUI: Character not found when updating slots stats", LogLevel.ERROR);
			return;
		}
		
		int sessionTimeSpent = (System.GetTickCount() - m_sessionStartTime) / 1000;
		int sessionAverageBet = m_sessionTotalBets / m_sessionTotalSpins;
		
		character.UpdateSlotsStats(
			m_sessionSpent,
			m_sessionNetProfit,
			m_sessionBiggestWin,
			m_sessionTotalSpins,
			m_sessionTotalWins,
			m_sessionTotalLosses,
			sessionTimeSpent,
			sessionAverageBet
		);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSpinValidated(int betAmount)
	{
		m_cachedCash -= betAmount;
		UpdateCashDisplay();
		
		TrackSpinStart();
		StartSpinAfterValidation();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSpinValidationFailed(string reason)
	{

		m_isSpinning = false;
		SetButtonsEnabled(true);
		UpdateStatusText(reason);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSpinWin(int winnings)
	{
		m_cachedCash += winnings;
		UpdateCashDisplay();
		UpdateStatusText("YOU WIN $" + winnings.ToString());
		
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character)
			character.PlayCommonSoundLocally("SOUND_SLOTSWIN");
		
		TrackSpinResult(true, winnings);
		SetButtonsEnabled(true);
	}
}