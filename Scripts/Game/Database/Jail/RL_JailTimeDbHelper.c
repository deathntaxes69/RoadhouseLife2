class RL_JailTimeDbHelper : RL_DbHelper
{
	protected ref RL_CallbackJailTimeLoop m_callbackJailTimeLoop;

	void UpdateJailTimes(array<SCR_ChimeraCharacter> characters)
	{
		if (!m_restContext) {
			Print("RL_JailTimeDbHelper: Rest context not initialized", LogLevel.ERROR);
			return;
		}

		if (!m_callbackJailTimeLoop) m_callbackJailTimeLoop = new RL_CallbackJailTimeLoop();
		m_callbackJailTimeLoop.SetCallback(null, "", null);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "jailtimeloop";

		array<int> characterIds = {};
		array<int> issueWarrantIds = {};

		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("[RL_JailTimeDbHelper] UpdateJailTimes foreach loop");
			if (!character)
				continue;

			string characterId = character.GetCharacterId();
			if (!characterId || characterId == "-1")
				continue;

			if (character.GetCharacterController().GetLifeState() == ECharacterLifeState.DEAD)
				continue;

			int jailTime = character.GetJailTime();
			if (!jailTime || jailTime <= 0)
				continue;

			if (RL_JailManagerComponent.IsOutsideJailBounds(character))
			{
				//m_characterHelper.AdjustJailTime(characterId, -jailTime);
				//m_warrantsHelper.IssueWarrant(characterId, m_jailEscapeCrimeId, "-1");

				character.SetHasWarrant(true);
				issueWarrantIds.Insert(characterId.ToInt(0));
				character.AdjustJailTimeCallback(0);
			} else {
				//m_characterHelper.AdjustJailTime(characterId, -1);

				characterIds.Insert(characterId.ToInt(0));
				character.AdjustJailTimeCallback(jailTime - 1);
				if (jailTime <= 1) character.ReleaseFromJail();
			}
		}

		int ccount = characterIds.Count();
		int wcount = issueWarrantIds.Count();

		if (ccount < 1 && wcount < 1)
			return;

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();

		jsonCtx.WriteValue("ids", string.Format("%1|", SCR_StringHelper.Join("|", characterIds)));
		jsonCtx.WriteValue("wids", string.Format("%1|", SCR_StringHelper.Join("|", issueWarrantIds)));

		string outputJson = jsonCtx.ExportToString();

		//Print(outputJson);

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackJailTimeLoop, requestURL, outputJson);
	}
}

class RL_CallbackJailTimeLoop : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		PrintFormat("RL_JailTimeDbHelper: Update jail time success data=%1", data);
	}

	override void OnError(int errorCode)
	{
		Print(string.Format("RL_JailTimeDbHelper: Update jail time failed. Error code: %1", errorCode), LogLevel.ERROR);
	}
}
