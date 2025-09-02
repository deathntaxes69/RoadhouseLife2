class RL_CallbackTransactMoney : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		bool success = true;
		RL_MoneyData castedResults = new RL_MoneyData();
		castedResults.ExpandFromRAW(data);
		ScriptModule scriptModule = GetGame().GetScriptModule();

		if (castedResults.id == 0 && castedResults.cash == 0 && castedResults.bank == 0)
			success = false;

		if (m_callbackThis && m_callbackFunction && m_callbackContext)
			scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, success, m_callbackContext);

		if (!success)
			return;
		string characterId = castedResults.id.ToString();

		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(characterId);
		if (!character)
			return;
		scriptModule.Call(character, "TransactMoneyCallback", true, null, castedResults.cash, castedResults.bank);
	}

}

class RL_MoneyData : JsonApiStruct
{
	int id = 0;
	int cash = 0;
	int bank = 0;

	void RL_MoneyData()
	{
		RegV("id");
		RegV("cash");
		RegV("bank");
	}

}
