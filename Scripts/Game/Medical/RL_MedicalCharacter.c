modded class SCR_ChimeraCharacter
{
    void Heal(float newHealth = 1, float newBlood = 1)
    {
        Rpc(RpcAsk_Heal, newHealth, newBlood);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_Heal(float newHealth, float newBlood)
    {
        Print("RpcAsk_Heal");
        SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(this.FindComponent(SCR_CharacterDamageManagerComponent));
        RL_MedicalUtils.Heal(damageMgr, newHealth, newBlood);
    }
    void AskOwnerToHeal(float newHealth = 1, float newBlood = 1, bool revive = false, bool localonly = false)
    {
        Rpc(RpcDo_Heal, newHealth, newBlood, revive, localonly);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_Heal(float newHealth, float newBlood, bool revive, bool localonly)
    {
        Print("RpcDo_Heal");
        if(revive)
        {
            auto dmc = SCR_CharacterDamageManagerComponent.Cast(this.FindComponent(SCR_CharacterDamageManagerComponent));
            dmc.SetRespawned(true);
        }
        SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(this.FindComponent(SCR_CharacterDamageManagerComponent));
        RL_MedicalUtils.Heal(damageMgr, newHealth, newBlood, false, localonly);
    }
}