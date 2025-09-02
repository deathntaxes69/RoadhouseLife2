class RL_PaycheckDbHelper : RL_DbHelper
{
	protected ref RL_CallbackPaycheckLoop m_callbackPaycheckLoop;

	// Paycheck stuff
	protected static const int paycheckAmount = 350;
	protected static const int paycheckAmountEMS = 750;
	protected static const int paycheckAmountPolice = 800;

	protected static const int paycheckXpAmount = 30;
	protected static const int paycheckXpAmountEMS = 50;
	protected static const int paycheckXpAmountPolice = 50;

	protected static const float paycheckAmountRankBonus = (paycheckAmountPolice * 2) / 9;
	protected static const string notifyTitleText = "PAYCHECK";
	protected static const string notifyText = "You have received a paycheck for $";

	void GivePaychecks(array<SCR_ChimeraCharacter> characters)
	{
		if (!m_restContext) {
			Print("RL_PaycheckDbHelper: Rest context not initialized", LogLevel.ERROR);
			return;
		}

		if (!m_callbackPaycheckLoop) m_callbackPaycheckLoop = new RL_CallbackPaycheckLoop();
		m_callbackPaycheckLoop.SetCallback(null, "", null);

		m_restContext.SetHeaders(m_baseHeaders);
		string requestURL = "paycheckloop";

		array<int> characterIds = {};
		array<int> amounts = {};
		array<int> playTimes = {};
		array<int> expCatagories = {};
		array<int> exps = {};

		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("[RL_PaycheckDbHelper] GivePaychecks foreach loop");
			if (!character)
				continue;

			string characterId = character.GetCharacterId();
			if (!characterId || characterId == "-1")
				continue;

			characterIds.Insert(characterId.ToInt(0));
			playTimes.Insert(character.GetTotalPlayTime());

			int amount = paycheckAmount;
			int expAmount = paycheckXpAmount;
			int expCatagory = RL_SkillsCategory.CHARACTER;

			if (character.IsPolice())
			{
				amount = Math.Round(Math.Min(paycheckAmountPolice + (paycheckAmountRankBonus * (character.GetPoliceRank() - 1)), paycheckAmountPolice * 2));
				expCatagory = RL_SkillsCategory.POLICE;
				expAmount = paycheckXpAmountPolice;
			}
			else if (character.IsMedic())
			{
				amount = paycheckAmountEMS;
				expCatagory = RL_SkillsCategory.EMS;
				expAmount = paycheckXpAmountEMS;
			}

			amounts.Insert(amount);
			expCatagories.Insert(expCatagory);
			exps.Insert(expAmount);

			character.Notify(notifyText + amount, notifyTitleText);
		}

		if (characterIds.Count() < 1)
			return;

		if (characterIds.Count() != (amounts.Count() + playTimes.Count() + expCatagories.Count() + exps.Count()) / 4)
		{
			Print("RL_PaycheckDbHelper: Paycheck array count mismatch, aborting");
			return;
		}

		SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
		jsonCtx.WriteValue("ids", string.Format("%1|", SCR_StringHelper.Join("|", characterIds)));
		jsonCtx.WriteValue("am", string.Format("%1|", SCR_StringHelper.Join("|", amounts)));
		jsonCtx.WriteValue("pts", string.Format("%1|", SCR_StringHelper.Join("|", playTimes)));
		jsonCtx.WriteValue("xpc", string.Format("%1|", SCR_StringHelper.Join("|", expCatagories)));
		jsonCtx.WriteValue("xp", string.Format("%1|", SCR_StringHelper.Join("|", exps)));
		string outputJson = jsonCtx.ExportToString();

		//Print(outputJson);

		if(RL_DBHelperCheckIfNotServer()) return;
		m_restContext.POST(m_callbackPaycheckLoop, requestURL, outputJson);
	}
}

class RL_CallbackPaycheckLoop : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		PrintFormat("RL_PaycheckDbHelper: Paycheck success data=%1", data);
	}

	override void OnError(int errorCode)
	{
		Print(string.Format("RL_PaycheckDbHelper: Paycheck failed. Error code: %1", errorCode), LogLevel.ERROR);
	}
}
