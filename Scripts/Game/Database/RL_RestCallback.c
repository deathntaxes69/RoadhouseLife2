class RL_RestCallback : RestCallback
{
	protected Managed m_callbackThis;
	protected string m_callbackFunction;
	protected ref Managed m_callbackContext;
	protected int m_shopId;

	void SetCallback(Managed callbackThis, string callbackFunction, Managed context = null)
	{
		m_callbackThis = callbackThis;
		m_callbackFunction = callbackFunction;
		m_callbackContext = context;
	}
	void SetShopId(int shopId)
	{
		m_shopId = shopId;
	}

	override void OnError(int errorCode)
	{
		if (m_callbackThis && m_callbackFunction)
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, false, "");
	}

	override void OnTimeout()
	{
		if (m_callbackThis && m_callbackFunction)
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, false, "");
	}

	override void OnSuccess(string data, int dataSize)
	{
		if (m_callbackThis && m_callbackFunction)
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, null, true, data);

	}

}
