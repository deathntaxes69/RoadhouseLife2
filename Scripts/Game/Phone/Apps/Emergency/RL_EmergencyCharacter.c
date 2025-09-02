modded class SCR_ChimeraCharacter
{
    protected IEntity m_policeEmergencyMarker;
    protected IEntity m_emsEmergencyMarker;
    protected const float m_fEmergencyMarkerDisplayTime = 300000; // 5 minutes
    
	//------------------------------------------------------------------------------------------------
    void SendEmergencyToPolice(string messageTitle, string message, string markerText)
    {
        Rpc(RpcAsk_SendEmergencyToPolice, messageTitle, message, markerText);
    }
    
	//------------------------------------------------------------------------------------------------
    void SendEmergencyToEMS(string messageTitle, string message, string markerText)
    {
        Rpc(RpcAsk_SendEmergencyToEMS, messageTitle, message, markerText);
    }
 	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SendEmergencyToPolice(string messageTitle, string message, string markerText)
	{
        if (!Replication.IsServer())
            return;
		Print("RpcAsk_SendEmergencyToPolice");

		RL_Utils.NotifyAllPolice(message, messageTitle);
		CreatePoliceEmergencyMarker(GetOrigin(), message, GetCharacterName(), markerText);
    }
    
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SendEmergencyToEMS(string messageTitle, string message, string markerText)
	{
        if (!Replication.IsServer())
            return;
		Print("RpcAsk_SendEmergencyToEMS");

		RL_Utils.NotifyAllEMS(message, messageTitle);
		CreateEmsEmergencyMarker(GetOrigin(), GetCharacterName());
    }
    
	//------------------------------------------------------------------------------------------------
    void CreatePoliceEmergencyMarker(vector position, string message, string playerName, string markerText)
    {
        if (!Replication.IsServer())
            return;
            
        DeletePoliceEmergencyMarker();
        
        m_policeEmergencyMarker = RL_MapUtils.CreateMarkerFromPrefabServer(
            position, 
            "{CFDEC4E7A4407DCF}Prefabs/World/Locations/Common/RL_MapMarker_PoliceEmergency.et", 
            markerText, 
            m_fEmergencyMarkerDisplayTime
        );
    }
    
 	//------------------------------------------------------------------------------------------------
    void CreateEmsEmergencyMarker(vector position, string markerText)
    {
        if (!Replication.IsServer())
            return;
            
        DeleteEmsEmergencyMarker();
        
        m_emsEmergencyMarker = RL_MapUtils.CreateMarkerFromPrefabServer(
            position, 
            "{CB22E1D6A837B2DF}Prefabs/World/Locations/Common/RL_MapMarker_EmsEmergency.et", 
            markerText, 
            m_fEmergencyMarkerDisplayTime
        );
    }
    
	//------------------------------------------------------------------------------------------------
    void DeletePoliceEmergencyMarker()
    {
        if (!Replication.IsServer())
            return;
            
        if (m_policeEmergencyMarker)
        {
            RL_MapUtils.CancelMarkerDeletion(m_policeEmergencyMarker);
            RL_MapUtils.DeleteMarkerEntity(m_policeEmergencyMarker);
            m_policeEmergencyMarker = null;
        }
    }
    
	//------------------------------------------------------------------------------------------------
    void DeleteEmsEmergencyMarker()
    {
        if (!Replication.IsServer())
            return;
            
        if (m_emsEmergencyMarker)
        {
            RL_MapUtils.CancelMarkerDeletion(m_emsEmergencyMarker);
            RL_MapUtils.DeleteMarkerEntity(m_emsEmergencyMarker);
            m_emsEmergencyMarker = null;
        }
    }
    
	//------------------------------------------------------------------------------------------------
    void ~SCR_ChimeraCharacter()
    {
        if (Replication.IsServer())
        {
            DeletePoliceEmergencyMarker();
            DeleteEmsEmergencyMarker();
        }
    }
} 