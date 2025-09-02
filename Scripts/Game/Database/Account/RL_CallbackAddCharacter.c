class RL_CallbackAddCharacter : RL_RestCallback
{
	RL_CharacterAccount m_character;

	void SetCharacter(RL_CharacterAccount character)
	{
		m_character = character;
	}

	override void OnSuccess(string data, int dataSize)
	{
		ScriptModule scriptModule = GetGame().GetScriptModule();
		scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, m_character, (data == "Name in use"));
	}
}
