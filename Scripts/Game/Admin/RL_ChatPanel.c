modded class SCR_ChatPanel
{
	protected static ref array<string> s_aBlockedWords = {
		"BanWorld",
	};
	
	protected static ref array<string> s_aWordReplacements = {
		"admin", "god",
		"vdm", "reckless driving",
		"metagame", "psychic information",
		"failrp", "acting unusual",
		"rdm", "murder",
		"combat log", "vanished",
		"exploiting", "acting weird",
		"respawn", "catch a train",
		"npc", "citizen",
		"dupe", "magic",
		"cheat", "acting sus",
		"roleplay", "act normal",
		"erp", "sus",
		"noob", "new citizen"
	};
	
	//------------------------------------------------------------------------------------------------
	protected bool ContainsBlockedWord(string message)
	{
		if (message.IsEmpty() || !s_aBlockedWords)
			return false;
			
		message.ToLower();
		foreach (string blockedWord : s_aBlockedWords)
		{
			if (message.Contains(blockedWord))
				return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected string ApplyWordReplacements(string message)
	{
		if (message.IsEmpty() || !s_aWordReplacements)
			return message;
		
		message.ToLower();
		for (int i = 0; i < s_aWordReplacements.Count() - 1; i += 2)
		{
			message.Replace(s_aWordReplacements[i], s_aWordReplacements[i + 1]);
		}
		
		return message;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsMessageBlocked(string message)
	{
		if (message.IsEmpty())
			return true;
		
		if (ContainsBlockedWord(message))
			return true;
			
		string filteredMessage;
		if (SCR_ProfaneFilter.ReplaceProfanities(message, filteredMessage))
			return true;
	
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void SendMessage()
	{
		if (!m_Widgets.m_MessageEditBox)
		{
			super.SendMessage();
			return;
		}
		
		string message = m_Widgets.m_MessageEditBox.GetText();

		if (IsMessageBlocked(message))
		{
			SCR_ChatPanelManager mgr = SCR_ChatPanelManager.GetInstance();
			if (mgr)
				mgr.ShowHelpMessage("Message blocked");
			
			m_Widgets.m_MessageEditBox.SetText("");
			return;
		}
		
		message = ApplyWordReplacements(message);
		m_Widgets.m_MessageEditBox.SetText(message);
		
		super.SendMessage();
	}
}