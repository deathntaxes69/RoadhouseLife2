modded class SCR_ChimeraCharacter
{ 
    [RplProp()]
    protected bool m_bIsEscorted = false;   

    protected RL_EscortVehicle m_escortVehicle;
    
    void SetEscortVehicle(RL_EscortVehicle escortVehicle)
    {
        m_escortVehicle = escortVehicle;
    }
    RL_EscortVehicle GetEscortVehicle()
    {
        return m_escortVehicle;
    }
    bool GetEscortState()
    {
        return m_bIsEscorted; 
    } 
    void SetEscortedState(bool state)
    { 
        m_bIsEscorted = state;
        Replication.BumpMe();

        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
        if(!state && character && character.IsRestrained())
        {
            character.RestrainAgain();
        }
    }
    void GetOutEscort()
    { 
        Rpc(RpcDo_GetOutEscort);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    private void RpcDo_GetOutEscort()
    {
        SCR_CompartmentAccessComponent compAccessComponent = SCR_CompartmentAccessComponent.Cast(this.FindComponent(SCR_CompartmentAccessComponent));
        compAccessComponent.GetOutVehicle(EGetOutType.TELEPORT, -1, false, false);
        
    }
    void ForceStopEscortAction()
    {
		Print("ForceStopEscortAction");
		if (GetEscortVehicle())
		{
			GetEscortVehicle().StopEscort();
			SetEscortVehicle(null);
		}
        SetEscortedState(false);
	}
}    