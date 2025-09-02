class RL_CallbackAddDynamicUpgrade : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		PrintFormat("[CallbackAddDynamicUpgrade] OnSuccess called with data: %1", data);
		
		if (m_callbackThis && m_callbackFunction)
		{
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, true, data);
		}
	}

	override void OnError(int errorCode)
	{
		PrintFormat("[CallbackAddDynamicUpgrade] OnError called with code: %1", errorCode);
		
		if (m_callbackThis && m_callbackFunction)
		{
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, false, "");
		}
	}

	override void OnTimeout()
	{
		PrintFormat("[CallbackAddDynamicUpgrade] OnTimeout called");
		
		if (m_callbackThis && m_callbackFunction)
		{
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, false, "");
		}
	}
}