class RL_DeliveryJobDbHelper : RL_DbHelper
{
    protected ref RL_RestCallback m_callbackInsertDelivery;

    void InsertDelivery(string uid, int cid, string startingPoint, string endPoint, int timeTaken, float distance, int cashEarned, Managed callbackTarget, string callbackMethod)
    {
        if (!m_restContext)
        {
            Print("RL_DeliveryJobDbHelper: Rest context not initialized", LogLevel.ERROR);
            return;
        }

        if (!m_callbackInsertDelivery) m_callbackInsertDelivery = new RL_RestCallback();
        m_callbackInsertDelivery.SetCallback(callbackTarget, callbackMethod);

        m_restContext.SetHeaders(m_baseHeaders);
        string requestURL = "delivery/complete";
        
        SCR_JsonSaveContext jsonCtx = new SCR_JsonSaveContext();
        jsonCtx.WriteValue("uid", uid);
        jsonCtx.WriteValue("cid", cid);
        jsonCtx.WriteValue("starting_point", startingPoint);
        jsonCtx.WriteValue("end_point", endPoint);
        jsonCtx.WriteValue("time_taken", timeTaken);
        jsonCtx.WriteValue("distance", distance);
        jsonCtx.WriteValue("cash_earned", cashEarned);

        if(RL_DBHelperCheckIfNotServer()) return;
        m_restContext.POST(m_callbackInsertDelivery, requestURL, jsonCtx.ExportToString());
    }
} 