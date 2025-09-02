class RL_DroppedCashComponentClass: ScriptComponentClass
{
};

class RL_DroppedCashComponent: ScriptComponent
{

	[RplProp()]
    string m_sOwnerId = "";
    [RplProp()]
    int m_iCashValue = 0;

    int GetCashValue()
	{
        return m_iCashValue;
	}
    void SetProps(string ownerId, int cashValue)
	{
        Print("-------- SetProps");
        m_sOwnerId = ownerId;
        m_iCashValue = cashValue;
        Replication.BumpMe();
	}
	
};
