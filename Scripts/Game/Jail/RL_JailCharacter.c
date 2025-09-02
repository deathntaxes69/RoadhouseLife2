modded class SCR_ChimeraCharacter
{

    protected ref RL_RespawnUI m_respawnMenu;

    void TeleportToJail()
    {
        vector spawnPoint = RL_JailManagerComponent.GetFreeSpawnPoint();
        Rpc(RpcDo_TeleportToJail, spawnPoint);
        
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    private void RpcDo_TeleportToJail(vector spawnPoint)
    {
        RL_Utils.TeleportPlayer(this, spawnPoint);
    }
    void ReleaseFromJail()
    {
        Print("ReleaseFromJail");
        RL_JailReleasePoint spawnPoint = RL_JailReleasePoint.GetRandomSpawnPoint();
        if(!spawnPoint)
        {
            Print("ReleaseFromJail found no release point");
            return;
        }
        vector spawnPointPos;
        vector yawPitchRoll;
        spawnPoint.GetPosYPR(spawnPointPos, yawPitchRoll);

        Rpc(RpcDo_ReleaseFromJail, spawnPointPos);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    private void RpcDo_ReleaseFromJail(vector spawnPointPos)
    {
        Print("RpcDo_ReleaseFromJail");
        RL_Utils.TeleportPlayer(this, spawnPointPos);

    }
}