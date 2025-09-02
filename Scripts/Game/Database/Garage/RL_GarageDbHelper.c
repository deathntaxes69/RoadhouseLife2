class RL_GarageDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackLoadGarage;
    protected ref RL_CallbackAddGarage m_callbackAddGarage;
    protected ref RL_CallbackUpdateGarageState m_callbackUpdateGarageState;
    protected ref RL_RestCallback m_callbackUpdateVehicleColors;

    void LoadGarage(Managed callbackThis, string callbackFunction, string uid)
    {
		if (!m_callbackLoadGarage) m_callbackLoadGarage = new RL_RestCallback();
        m_callbackLoadGarage.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "fetchgarage/" + uid;

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackLoadGarage, requestURL);

    }
    void AddToGarage(Managed callbackThis, string callbackFunction, Managed context, RL_GarageItem garageItem)
    {
        if (!m_callbackAddGarage) m_callbackAddGarage = new RL_CallbackAddGarage();
        m_callbackAddGarage.SetCallback(callbackThis, callbackFunction, context);

        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "insertGarage";

		Tuple5<int, string, string, string, int> typedContext = Tuple5<int, string, string, string, int>.Cast(context);

        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("cid", garageItem.GetCharacterId().ToInt());
        jsonCtx.WriteValue("prefab", garageItem.GetPrefab());
		jsonCtx.WriteValue("price", typedContext.param5);

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackAddGarage, requestURL, jsonCtx.ExportToString());
    }
    void UpdateGarageState(int garageId, int state)
    {
        if (!m_callbackUpdateGarageState) m_callbackUpdateGarageState = new RL_CallbackUpdateGarageState();
		m_callbackUpdateGarageState.SetCallback(null, "");

        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "updategaragestate/" + garageId + "/" + state.ToString();

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackUpdateGarageState, requestURL, "");
    }
    
    void UpdateVehicleColors(int garageId, string colorsJson, Managed callbackThis = null, string callbackFunction = "")
    {
        if (!m_callbackUpdateVehicleColors) m_callbackUpdateVehicleColors = new RL_RestCallback();
        m_callbackUpdateVehicleColors.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "updatevehiclecolors/" + garageId;
        
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackUpdateVehicleColors, requestURL, colorsJson);
    }

}