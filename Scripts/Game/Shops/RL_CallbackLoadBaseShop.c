class RL_CallbackLoadBaseShop : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		ScriptModule scriptModule = GetGame().GetScriptModule();
		scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, true, data, m_shopId);
	}
}