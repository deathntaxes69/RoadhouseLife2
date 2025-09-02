modded class SCR_ChimeraCharacter
{

    void AskSetFrequency(int channel, float hz)
    {
        Rpc(RpcAsk_SetFrequency, channel, hz);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_SetFrequency(int channel, float hz)
	{
        RL_RadioUtils.SetFrequency(this, channel, hz);
    }

}