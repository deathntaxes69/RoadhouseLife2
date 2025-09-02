class RL_CallbackAddGarage : RL_RestCallback
{
	override void OnSuccess(string data, int dataSize)
	{
            RL_GarageItem castedResults = new RL_GarageItem();
		    castedResults.ExpandFromRAW(data);
            bool success = castedResults.GetVehicleId();
            ScriptModule scriptModule = GetGame().GetScriptModule();
            scriptModule.Call(m_callbackThis, m_callbackFunction, true, null, success, castedResults.GetVehicleId(), m_callbackContext);
			
	}
}
