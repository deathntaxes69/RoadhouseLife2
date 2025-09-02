class RL_CallbackFetchLicenses : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
		if (!jsonCtx.ImportFromString(data))
		{
			OnError(0);
			return;
		}

		string uid;
		string licenses;

		jsonCtx.ReadValue("uid", uid);
		jsonCtx.ReadValue("licenses", licenses);

		if (m_callbackThis && m_callbackFunction)
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, m_callbackContext, true, uid, licenses);
	}
}

class RL_CallbackModifyLicense : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
		SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
		if (!jsonCtx.ImportFromString(data))
		{
			OnError(0);
			return;
		}

		string uid;
		string licenses;
		int bank;

		jsonCtx.ReadValue("id", uid);
		jsonCtx.ReadValue("licenses", licenses);
		jsonCtx.ReadValue("bank", bank);

		if (m_callbackThis && m_callbackFunction)
			GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, m_callbackContext, true, uid, licenses, bank);
	}
}

class RL_CallbackModifyRecipe : RL_RestCallback
{
    override void OnSuccess(string data, int dataSize)
    {
        SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
        if (!jsonCtx.ImportFromString(data))
        {
            OnError(0);
            return;
        }

        string uid;
        string recipes;

        jsonCtx.ReadValue("id", uid);
        jsonCtx.ReadValue("recipes", recipes);

        if (m_callbackThis && m_callbackFunction)
            GetGame().GetScriptModule().Call(m_callbackThis, m_callbackFunction, true, m_callbackContext, true, uid, recipes);
    }
}
