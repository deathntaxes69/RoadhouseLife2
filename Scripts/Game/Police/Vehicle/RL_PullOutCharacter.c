modded class SCR_ChimeraCharacter
{ 
    void GetOutVehicle()
    { 
        Rpc(RpcDo_GetOutVehicle);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    private void RpcDo_GetOutVehicle()
    {
        // Don't pull out uncuffed police
        if(this.IsPolice() && !this.IsHandcuffed())
            return;

        SCR_CompartmentAccessComponent compAccessComponent = SCR_CompartmentAccessComponent.Cast(this.FindComponent(SCR_CompartmentAccessComponent));
        compAccessComponent.GetOutVehicle(EGetOutType.TELEPORT, -1, false, false);
        
    } 
}    