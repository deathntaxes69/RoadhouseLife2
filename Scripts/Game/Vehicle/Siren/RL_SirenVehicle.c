modded class Vehicle
{
    [RplProp()]
    protected bool m_checkedForSiren = false;
    
    protected int m_iCurrentSirenSignal = 0;
    protected int m_iCurrentHornSignal = 0;
    
    protected string m_sirenSignalName = "POLICE_SIREN_SIGNAL";
    protected int m_sirenSignalMax = 6;
    protected string m_hornSignalName = "POLICE_HORN_SIGNAL";
    protected int m_hornSignalMax = 12;



    bool IsSirenOn()
    {
        return (m_iCurrentSirenSignal > 0);
    }
    void ToggleSirenSound(string characterid = "")
    {
        Print("ToggleSirenSound by " + characterid);
        if(IsSirenOn())
        {
            m_iCurrentSirenSignal = 0;
        } else {
            m_iCurrentSirenSignal = 1;
        }
        Rpc(RpcDo_SetSignalValueStr, m_sirenSignalName, m_iCurrentSirenSignal, false);
    }
    void CycleSirenSound()
    {
        m_iCurrentSirenSignal = m_iCurrentSirenSignal+1;
        if(m_iCurrentSirenSignal > m_sirenSignalMax)
            m_iCurrentSirenSignal = 1;
        Rpc(RpcDo_SetSignalValueStr, m_sirenSignalName, m_iCurrentSirenSignal, true);
    }
    void CycleHornSound()
    {
        m_iCurrentHornSignal = m_iCurrentHornSignal+1;
        if(m_iCurrentHornSignal > m_hornSignalMax)
            m_iCurrentHornSignal = 0;
        Rpc(RpcDo_SetSignalValueStr, m_hornSignalName, m_iCurrentHornSignal, false);
    }
    float GetSignalValueStr(string signalName)
    {
        SignalsManagerComponent signalComponent = SignalsManagerComponent.Cast(this.FindComponent(SignalsManagerComponent));
        int signalIndex = signalComponent.FindSignal(signalName);
        if(!signalIndex || signalIndex == -1)
        {
            Print(string.Format("No signal index found for %1", signalName));
            return -1;
        }

        return signalComponent.GetSignalValue(signalIndex);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_SetSignalValueStr(string signalName, int signalValue, bool shouldResetFirst)
    {
        Print(string.Format("Setting %1 to %2", signalName, signalValue));
        SignalsManagerComponent signalComponent = SignalsManagerComponent.Cast(this.FindComponent(SignalsManagerComponent));
        int signalIndex = signalComponent.FindSignal(signalName);
        if(!signalIndex || signalIndex == -1)
        {
            Print(string.Format("No signal index found for %1", signalName));
            return;
        }
        if(shouldResetFirst)
        {
            signalComponent.SetSignalValue(signalIndex, 0);
            GetGame().GetCallqueue().CallLater(RpcDo_SetSignalValueStr, 100, false, signalName, signalValue, false);
            return;
        }

        signalComponent.SetSignalValue(signalIndex, signalValue);
        
    }
}