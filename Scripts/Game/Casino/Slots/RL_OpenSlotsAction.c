[BaseContainerProps]
class RL_OpenSlotsAction : ScriptedUserAction
{
	[Attribute("50", UIWidgets.SpinBox, "Minimum bet amount", "50 10000 50")]
	protected int m_minBet;
	
	[Attribute("1000", UIWidgets.SpinBox, "Maximum bet amount", "100 50000 100")]
	protected int m_maxBet;
	
	[Attribute("50", UIWidgets.SpinBox, "Bet increment amount", "10 1000 10")]
	protected int m_betIncrement;
	
	[Attribute("1", UIWidgets.Slider, "APPLE payout multiplier", "1 20 1", category: "Symbol Multipliers")]
	protected int m_appleMultiplier;
	
	[Attribute("3", UIWidgets.Slider, "GOLD_BARS payout multiplier", "1 20 1", category: "Symbol Multipliers")]
	protected int m_goldBarsMultiplier;
	
	[Attribute("1", UIWidgets.Slider, "PEACH payout multiplier", "1 20 1", category: "Symbol Multipliers")]
	protected int m_peachMultiplier;
	
	[Attribute("4", UIWidgets.Slider, "RUBY payout multiplier", "1 20 1", category: "Symbol Multipliers")]
	protected int m_rubyMultiplier;
	
	[Attribute("1", UIWidgets.Slider, "BOMB payout multiplier", "1 20 1", category: "Symbol Multipliers")]
	protected int m_bombMultiplier;
	
	[Attribute("10", UIWidgets.Slider, "DIAMOND payout multiplier", "1 50 1", category: "Symbol Multipliers")]
	protected int m_diamondMultiplier;
	
	[Attribute("5", UIWidgets.Slider, "CAR payout multiplier", "1 20 1", category: "Symbol Multipliers")]
	protected int m_carMultiplier;
	
	[Attribute("5", UIWidgets.Slider, "APPLE appearances on reel", "0 15 1", category: "Reel 1")]
	protected int m_reel1AppleFreq;
	[Attribute("2", UIWidgets.Slider, "GOLD_BARS appearances on reel", "0 15 1", category: "Reel 1")]
	protected int m_reel1GoldBarsFreq;
	[Attribute("5", UIWidgets.Slider, "PEACH appearances on reel", "0 15 1", category: "Reel 1")]
	protected int m_reel1PeachFreq;
	[Attribute("2", UIWidgets.Slider, "RUBY appearances on reel", "0 15 1", category: "Reel 1")]
	protected int m_reel1RubyFreq;
	[Attribute("4", UIWidgets.Slider, "BOMB appearances on reel", "0 15 1", category: "Reel 1")]
	protected int m_reel1BombFreq;
	[Attribute("1", UIWidgets.Slider, "DIAMOND appearances on reel", "0 5 1", category: "Reel 1")]
	protected int m_reel1DiamondFreq;
	[Attribute("1", UIWidgets.Slider, "CAR appearances on reel", "0 15 1", category: "Reel 1")]
	protected int m_reel1CarFreq;
	
	[Attribute("3", UIWidgets.Slider, "APPLE appearances on reel", "0 15 1", category: "Reel 2")]
	protected int m_reel2AppleFreq;
	[Attribute("3", UIWidgets.Slider, "GOLD_BARS appearances on reel", "0 15 1", category: "Reel 2")]
	protected int m_reel2GoldBarsFreq;
	[Attribute("4", UIWidgets.Slider, "PEACH appearances on reel", "0 15 1", category: "Reel 2")]
	protected int m_reel2PeachFreq;
	[Attribute("1", UIWidgets.Slider, "RUBY appearances on reel", "0 15 1", category: "Reel 2")]
	protected int m_reel2RubyFreq;
	[Attribute("5", UIWidgets.Slider, "BOMB appearances on reel", "0 15 1", category: "Reel 2")]
	protected int m_reel2BombFreq;
	[Attribute("1", UIWidgets.Slider, "DIAMOND appearances on reel", "0 5 1", category: "Reel 2")]
	protected int m_reel2DiamondFreq;
	[Attribute("3", UIWidgets.Slider, "CAR appearances on reel", "0 15 1", category: "Reel 2")]
	protected int m_reel2CarFreq;
	
	[Attribute("4", UIWidgets.Slider, "APPLE appearances on reel", "0 15 1", category: "Reel 3")]
	protected int m_reel3AppleFreq;
	[Attribute("1", UIWidgets.Slider, "GOLD_BARS appearances on reel", "0 15 1", category: "Reel 3")]
	protected int m_reel3GoldBarsFreq;
	[Attribute("3", UIWidgets.Slider, "PEACH appearances on reel", "0 15 1", category: "Reel 3")]
	protected int m_reel3PeachFreq;
	[Attribute("3", UIWidgets.Slider, "RUBY appearances on reel", "0 15 1", category: "Reel 3")]
	protected int m_reel3RubyFreq;
	[Attribute("3", UIWidgets.Slider, "BOMB appearances on reel", "0 15 1", category: "Reel 3")]
	protected int m_reel3BombFreq;
	[Attribute("1", UIWidgets.Slider, "DIAMOND appearances on reel", "0 5 1", category: "Reel 3")]
	protected int m_reel3DiamondFreq;
	[Attribute("5", UIWidgets.Slider, "CAR appearances on reel", "0 15 1", category: "Reel 3")]
	protected int m_reel3CarFreq;
	
	protected IEntity m_OwnerEntity;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_OwnerEntity = pOwnerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		MenuBase menu = menuManager.FindMenuByPreset(ChimeraMenuPreset.RL_SlotsUI);
		if (!menu)
		{
        	GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_SlotsUI);
			menu = menuManager.FindMenuByPreset(ChimeraMenuPreset.RL_SlotsUI);
		}
		
		GetGame().GetCallqueue().CallLater(SetupMenuData, 50, false, pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetupMenuData(IEntity pOwnerEntity)
	{
		MenuBase menu = GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.RL_SlotsUI);
		if (!menu) return;
		
		RL_SlotsUI slotsUI = RL_SlotsUI.Cast(menu);
		if (slotsUI)
		{
			slotsUI.SetConfig(m_minBet, m_maxBet, m_betIncrement);
			
			array<ref RL_SlotSymbol> symbols = BuildSymbolsFromAttributes();
			array<ref RL_SlotReel> reels = BuildReelsFromAttributes();
			
			if (symbols.Count() > 0 && reels.Count() > 0)
			{
				slotsUI.SetSymbolsAndReels(symbols, reels);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_SlotSymbol> BuildSymbolsFromAttributes()
	{
		array<ref RL_SlotSymbol> symbols = {};
	
		symbols.Insert(new RL_SlotSymbol("APPLE", Math.Max(1, m_appleMultiplier)));
		symbols.Insert(new RL_SlotSymbol("GOLD_BARS", Math.Max(1, m_goldBarsMultiplier)));
		symbols.Insert(new RL_SlotSymbol("PEACH", Math.Max(1, m_peachMultiplier)));
		symbols.Insert(new RL_SlotSymbol("RUBY", Math.Max(1, m_rubyMultiplier)));
		symbols.Insert(new RL_SlotSymbol("BOMB", Math.Max(1, m_bombMultiplier)));
		symbols.Insert(new RL_SlotSymbol("DIAMOND", Math.Max(1, m_diamondMultiplier)));
		symbols.Insert(new RL_SlotSymbol("CAR", Math.Max(1, m_carMultiplier)));
		
		return symbols;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_SlotReel> BuildReelsFromAttributes()
	{
		array<ref RL_SlotReel> reels = {};
		
		RL_SlotReel reel1 = new RL_SlotReel();
		if (m_reel1AppleFreq > 0) reel1.AddSymbol(0, m_reel1AppleFreq);
		if (m_reel1GoldBarsFreq > 0) reel1.AddSymbol(1, m_reel1GoldBarsFreq);
		if (m_reel1PeachFreq > 0) reel1.AddSymbol(2, m_reel1PeachFreq);
		if (m_reel1RubyFreq > 0) reel1.AddSymbol(3, m_reel1RubyFreq);
		if (m_reel1BombFreq > 0) reel1.AddSymbol(4, m_reel1BombFreq);
		if (m_reel1DiamondFreq > 0) reel1.AddSymbol(5, m_reel1DiamondFreq);
		if (m_reel1CarFreq > 0) reel1.AddSymbol(6, m_reel1CarFreq);
		reels.Insert(reel1);
		
		RL_SlotReel reel2 = new RL_SlotReel();
		if (m_reel2AppleFreq > 0) reel2.AddSymbol(0, m_reel2AppleFreq);
		if (m_reel2GoldBarsFreq > 0) reel2.AddSymbol(1, m_reel2GoldBarsFreq);
		if (m_reel2PeachFreq > 0) reel2.AddSymbol(2, m_reel2PeachFreq);
		if (m_reel2RubyFreq > 0) reel2.AddSymbol(3, m_reel2RubyFreq);
		if (m_reel2BombFreq > 0) reel2.AddSymbol(4, m_reel2BombFreq);
		if (m_reel2DiamondFreq > 0) reel2.AddSymbol(5, m_reel2DiamondFreq);
		if (m_reel2CarFreq > 0) reel2.AddSymbol(6, m_reel2CarFreq);
		reels.Insert(reel2);
		
		RL_SlotReel reel3 = new RL_SlotReel();
		if (m_reel3AppleFreq > 0) reel3.AddSymbol(0, m_reel3AppleFreq);
		if (m_reel3GoldBarsFreq > 0) reel3.AddSymbol(1, m_reel3GoldBarsFreq);
		if (m_reel3PeachFreq > 0) reel3.AddSymbol(2, m_reel3PeachFreq);
		if (m_reel3RubyFreq > 0) reel3.AddSymbol(3, m_reel3RubyFreq);
		if (m_reel3BombFreq > 0) reel3.AddSymbol(4, m_reel3BombFreq);
		if (m_reel3DiamondFreq > 0) reel3.AddSymbol(5, m_reel3DiamondFreq);
		if (m_reel3CarFreq > 0) reel3.AddSymbol(6, m_reel3CarFreq);
		reels.Insert(reel3);
		
		return reels;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}