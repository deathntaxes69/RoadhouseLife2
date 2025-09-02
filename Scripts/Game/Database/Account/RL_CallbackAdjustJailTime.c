class RL_CallbackAdjustJailTime : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		bool success = true;
		RL_JailData castedResults = new RL_JailData();
		castedResults.ExpandFromRAW(data);

		ScriptModule scriptModule = GetGame().GetScriptModule();
		scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, success, castedResults.jailTime, m_callbackContext);

		string characterId = castedResults.id.ToString();
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(characterId);
		if (!character)
			return;
		scriptModule.Call(character, "AdjustJailTimeCallback", true, null, castedResults.jailTime);
	}

}

class RL_JailData : JsonApiStruct
{
	int id = 0;
	int jailTime = 0;

	void RL_JailData()
	{
		RegV("id");
		RegV("jailTime");
	}

}
