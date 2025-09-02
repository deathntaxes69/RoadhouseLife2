modded class SCR_ChimeraCharacter
{
    protected int m_iProcessCount = 0;
    void SetProcessCount(int processCount)
    {
        Print(string.Format("SetProcessCount set %1", processCount));
        Rpc(RpcAsk_SetProcessCount, processCount);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetProcessCount(int processCount)
    {
        Print(string.Format("RpcAsk_SetProcessCount set %1", processCount));
        m_iProcessCount = processCount;
    }
    int GetProcessCount()
    {
        return m_iProcessCount;
    }
}