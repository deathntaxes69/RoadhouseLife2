modded class SCR_ChimeraCharacter
{
    
    [RplProp()]
    protected bool m_bCallActive = false;

    
    protected bool m_bCallAccepted = false;
    protected bool m_bCallStarter = false;
    protected int m_iCallStartTime;
    protected int m_iCallAcceptedTime;
    protected int m_iCallParty;
    protected string m_sCallPartyName;
    protected int m_iCallFreq;
    protected int m_iOldFreq;

    protected SCR_AudioSource m_callSound;
    protected string m_outgoingSound = "SOUND_PHONEOUTGOINGCALL";
    protected string m_incomingSound = "SOUND_PHONERINGTONE";
    protected string m_endSound = "SOUND_PHONEENDCALL";


    bool GetPhoneStatus()
    {
        return m_bCallActive;
    }
    bool IsCallAccepted()
    {
        return m_bCallAccepted;
    }
    bool IsCallStarter()
    {
        return m_bCallStarter;
    }
    int GetCallAcceptedTime()
    {
        return m_iCallAcceptedTime;
    }
    int GetCallParty()
    {
        return m_iCallParty;
    }
    string GetCallPartyName()
    {
        return m_sCallPartyName;
    }
    void PlaceCall(int recipient, string recipientName)
    {
        m_bCallStarter = true;
        m_bCallAccepted = false;
        m_iCallParty = recipient;
        m_sCallPartyName = recipientName;
        m_iCallStartTime = System.GetUnixTime();
        Replication.BumpMe();
        Rpc(RpcAsk_PlaceCall, RL_Utils.GetPlayerId(), recipient);
        PlayCallSound(m_outgoingSound);
        GetGame().GetCallqueue().CallLater(MonitorCall, 1000, true);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_PlaceCall(int sender, int recipient)
    {
        Print("RpcAsk_PlaceCall");
        string senderName = RL_Utils.GetPlayerName(sender);

        IEntity recipientEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(recipient);
        if(!recipientEntity)
        {
            Rpc(RpcDo_CallFailed);
            return;
        }
        SCR_ChimeraCharacter recipientCharacter = SCR_ChimeraCharacter.Cast(recipientEntity);

        BaseRadioComponent radioComp = RL_RadioUtils.GetRadioComponent(recipientCharacter);
        if(!radioComp || recipientCharacter.GetPhoneStatus())
        {
            Rpc(RpcDo_CallFailed);
            return;
        }
        m_bCallActive = true;
        Replication.BumpMe();
        recipientCharacter.ReceiveCall(sender, senderName);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_CallFailed()
    {
        ResetCallVars();
        StopCallSound();
        RL_Utils.Notify( "Your call could not be completed as dialed.", "PHONE");
    }
    void ReceiveCall(int sender, string senderName)
    {
        m_bCallActive = true;
        Replication.BumpMe();
        Rpc(RpcDo_ReceiveCall, sender, senderName);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_ReceiveCall(int sender, string senderName)
    {
        m_bCallAccepted = false;
        m_iCallParty = sender;
        m_sCallPartyName = senderName;
        m_iCallStartTime = System.GetUnixTime();

        RL_Utils.Notify(string.Format("%1 is calling you.", senderName), "PHONE");

        //MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_PhoneMenu);
        PlayCallSound(m_incomingSound);
		
        GetGame().GetCallqueue().CallLater(MonitorCall, 1000, true);
    }
    void AcceptCall()
    {
        if(!m_iCallParty)
        {
            return;
        }
        Print("AcceptCall" + m_iCallParty);
        m_bCallAccepted = true;
        m_iCallAcceptedTime = System.GetUnixTime();

		m_iCallFreq = 1000000 + (m_iCallParty * 1000);
        Print("AcceptCall SetPhoneFrequency " + m_iCallFreq);
        RL_RadioUtils.SetChannelActive(1);
        //m_iOldFreq = 
		//RL_RadioUtils.SetPhoneFrequency(m_iCallFreq);
        Rpc(RpcAsk_AcceptCall, m_iCallParty, RL_Utils.GetPlayerId());
        StopCallSound();
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_AcceptCall(int sender, int recipient)
    {
        string recipientName = RL_Utils.GetPlayerName(recipient);

        int callFreq = 1000000 + (sender * 1000);
        Print("RpcAsk_AcceptCall Freq");
        Print(callFreq);
        RL_RadioUtils.SetPhoneFrequency(sender, callFreq);
        RL_RadioUtils.SetPhoneFrequency(recipient, callFreq);

        IEntity senderEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(sender);
        SCR_ChimeraCharacter senderCharacter = SCR_ChimeraCharacter.Cast(senderEntity);
        
        senderCharacter.CallAccepted(recipient, recipientName);
    }
    void CallAccepted(int recipient, string recipientName)
    {
        Rpc(RpcDo_CallAccepted, recipient, recipientName);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_CallAccepted(int recipient, string recipientName)
    {
        RL_Utils.Notify( string.Format("%1 accepted your call", recipientName), "PHONE");

        m_iCallParty = recipient;
        m_sCallPartyName = recipientName;
        m_bCallAccepted = true;
        m_iCallAcceptedTime = System.GetUnixTime();

		m_iCallFreq  = 1000000 + (RL_Utils.GetPlayerId() * 1000);
        Print("RpcDo_CallAccepted SetPhoneFrequency " + m_iCallFreq);

        //RL_RadioUtils.SetPhoneFrequency(m_iCallFreq);
        RL_RadioUtils.SetChannelActive(1);
        StopCallSound();
    }
    void DenyCall()
    {
        Print("DenyCall");
        if(!m_iCallParty)
        {
            return;
        }
        Rpc(RpcAsk_DenyCall, m_iCallParty, RL_Utils.GetPlayerId());
        StopCallSound();
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_DenyCall(int sender, int recipient)
    {
        Print("RpcAsk_DenyCall");
        string recipientName = RL_Utils.GetPlayerName(recipient);
        IEntity senderEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(sender);
        SCR_ChimeraCharacter senderCharacter = SCR_ChimeraCharacter.Cast(senderEntity);
        senderCharacter.CallDenied(recipient, recipientName);
    }
    void CallDenied(int recipient, string recipientName)
    {
        Rpc(RpcDo_CallDenied, recipient, recipientName);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_CallDenied(int recipient, string recipientName)
    {
        RL_Utils.Notify(string.Format("%1 denied your call", recipientName), "PHONE");
        ResetCallVars();
        PlayCallSound(m_endSound);
    }
    void ResetCallVars()
    {
        m_bCallStarter = false;
        m_bCallAccepted = false;
        m_iCallParty = null;
        m_sCallPartyName = "";
        m_iCallStartTime = null;
        m_iCallAcceptedTime = null;
        if(m_iCallFreq)
            RL_RadioUtils.SetChannelActive(0);
        m_iCallFreq = null;
        GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.RL_PhoneMenu);
    }
    void EndCall()
    {
        if(m_iCallParty)
        {
            Rpc(RpcAsk_EndCall, m_iCallParty);
            PlayCallSound(m_endSound);
        }
        ResetCallVars();
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_EndCall(int callParty)
    {
        Print("RpcAsk_EndCall");
        m_bCallActive = false;
        Replication.BumpMe();
        IEntity partyEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(callParty);
        if(!partyEntity) return;
        SCR_ChimeraCharacter partyCharacter = SCR_ChimeraCharacter.Cast(partyEntity);
        if(!partyCharacter) return;
        partyCharacter.CallEnded();
        
    }
    void CallEnded()
    {
        m_bCallActive = false;
        Replication.BumpMe();
        Rpc(RpcDo_CallEnded);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_CallEnded()
    {

        RL_Utils.Notify("Call ended", "PHONE");
        ResetCallVars();
        PlayCallSound(m_endSound);
    }
    void PlayCallSound(string soundName)
    {
        StopCallSound();
		
        m_callSound = this.PlayCommonSoundLocally(soundName);
    }
    void StopCallSound()
    {
		GetGame().GetSoundManagerEntity().TerminateAudioSource(m_callSound);
    }
    void MonitorCall()
    {
        int maxCallingTime = 10;
        int currentUnixTime = System.GetUnixTime();
        bool alive = GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE;
        if(
            !m_iCallStartTime || 
            (!IsCallAccepted() && ((currentUnixTime - m_iCallStartTime) > maxCallingTime)) || 
            !alive
        )
        {
            GetGame().GetCallqueue().Remove(MonitorCall);
            EndCall();
        }
    }

}
