class RL_BaseShopDbHelper : RL_DbHelper
{
    protected ref RL_CallbackLoadBaseShop m_callbackLoadShop;
    protected ref RL_CallbackVehiclePrice m_callbackVehiclePrice;
    protected ref RL_CallbackVehicleRetrievalPrice m_callbackVehicleRetrievalPrice;
    protected ref RL_CallbackFurniturePrice m_callbackFurniturePrice;
    
    void LoadVehicleShop(Managed callbackThis, string callbackFunction, int shopId)
    {
		if (!m_callbackLoadShop) m_callbackLoadShop = new RL_CallbackLoadBaseShop();
        m_callbackLoadShop.SetCallback(callbackThis, callbackFunction);
        m_callbackLoadShop.SetShopId(shopId);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "vehicleshop/" + shopId;

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackLoadShop, requestURL);

    }
    void LoadClothingShop(Managed callbackThis, string callbackFunction, int shopId)
    {
		if (!m_callbackLoadShop) m_callbackLoadShop = new RL_CallbackLoadBaseShop();
        m_callbackLoadShop.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "clothingshop/" + shopId;
	
        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackLoadShop, requestURL);

    }
    void LoadItemShop(Managed callbackThis, string callbackFunction, int shopId)
    {
		if (!m_callbackLoadShop) m_callbackLoadShop = new RL_CallbackLoadBaseShop();
        m_callbackLoadShop.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "itemshop/" + shopId;

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackLoadShop, requestURL);

    }

    void GetVehiclePrice(Managed callbackThis, string callbackFunction, string prefab)
    {
        if (!m_callbackVehiclePrice) m_callbackVehiclePrice = new RL_CallbackVehiclePrice();
        m_callbackVehiclePrice.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "vehicleprice";
        SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
        jsonData.WriteValue("prefab", prefab);
        string jsonBody = jsonData.ExportToString();

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackVehiclePrice, requestURL, jsonBody);
    }

    void GetVehicleRetrievalPrice(Managed callbackThis, string callbackFunction, string prefab)
    {
        if (!m_callbackVehicleRetrievalPrice) m_callbackVehicleRetrievalPrice = new RL_CallbackVehicleRetrievalPrice();
        m_callbackVehicleRetrievalPrice.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "vehicleprice";
        SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
        jsonData.WriteValue("prefab", prefab);
        string jsonBody = jsonData.ExportToString();

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackVehicleRetrievalPrice, requestURL, jsonBody);
    }

    void LoadFurnitureShop(Managed callbackThis, string callbackFunction, int shopId)
    {
		if (!m_callbackLoadShop) m_callbackLoadShop = new RL_CallbackLoadBaseShop();
        m_callbackLoadShop.SetCallback(callbackThis, callbackFunction);
        m_callbackLoadShop.SetShopId(shopId);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "furnitureshop/" + shopId;

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.GET(m_callbackLoadShop, requestURL);

    }

    void GetFurniturePrice(Managed callbackThis, string callbackFunction, string prefab)
    {
        if (!m_callbackFurniturePrice) m_callbackFurniturePrice = new RL_CallbackFurniturePrice();
        m_callbackFurniturePrice.SetCallback(callbackThis, callbackFunction);
        
        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "furnitureprice";
        SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
        jsonData.WriteValue("prefab", prefab);
        string jsonBody = jsonData.ExportToString();

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackFurniturePrice, requestURL, jsonBody);
    }

}