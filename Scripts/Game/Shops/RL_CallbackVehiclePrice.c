class RL_CallbackVehiclePrice : RL_RestCallback
{
    override void OnSuccess(string data, int dataSize)
    {
        bool success = true;
        int price = 0;
        if (data != "")
        {
            SCR_JsonLoadContext jsonCtx = new SCR_JsonLoadContext();
            if (jsonCtx.ImportFromString(data))
            {
                jsonCtx.ReadValue("price", price);
            }
        }

        ScriptModule scriptModule = GetGame().GetScriptModule();
        scriptModule.Call(m_callbackThis, m_callbackFunction, success, price);
    }
}

class RL_CallbackVehicleRetrievalPrice : RL_RestCallback
{
    override void OnSuccess(string data, int dataSize)
    {
        bool success = true;
        
        ScriptModule scriptModule = GetGame().GetScriptModule();
        scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, success, data);
    }
}
