modded class Vehicle
{
    RL_VehicleManagerComponent GetManagerComp()
    {
        RL_VehicleManagerComponent comp = RL_VehicleManagerComponent.Cast(this.FindComponent(RL_VehicleManagerComponent));
        return comp;
    }
}

[EntityEditorProps(category: "GameScripted/Components", description: "")]
class RL_VehicleManagerComponentClass: ScriptComponentClass
{
    [Attribute("", UIWidgets.Auto, "Car Alarm Sound Configuration")]
    ref SCR_AudioSourceConfiguration m_CarAlarmAudioSourceConfiguration;
    
    [Attribute("", UIWidgets.Coords, "Car Alarm Sound Offset Position")]
    vector m_vCarAlarmSoundOffset;
};

class RL_VehicleManagerComponent: ScriptComponent
{
    [RplProp()]
    int m_sGarageId;
    [RplProp()]
    string m_sOwnerId = "";
    [RplProp()]
    string m_sOwnerName = "";
    [RplProp()]
    bool m_bLocked = true;
	[RplProp()]
	string m_sLockPickingCharacterId = "";
    [RplProp()]
    int m_iVehiclePrice = 0;
    [RplProp()]
    bool m_bPoliceSearched = false;

    protected ref RL_GarageDbHelper m_garageHelper;
    private AudioHandle m_CarAlarmAudioHandle = AudioHandle.Invalid;
    
    int GetGarageId()
	{
        return m_sGarageId;
	}
    string GetOwnerId()
	{
        return m_sOwnerId;
	}
    string GetOwnerName()
	{
        return m_sOwnerName;
	}
    bool IsOwner(IEntity player)
	{
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(player);
        return (character && m_sOwnerId && character.GetCharacterId() == m_sOwnerId);
    
	}
    
    void SetPoliceSearched(bool newvalue)
    {
        m_bPoliceSearched = newvalue;
        Replication.BumpMe();
    }

    bool HavePoliceSearched()
    {
        return m_bPoliceSearched;
    }

    bool HasVehicleAccess(IEntity player)
    {
        if (Replication.IsServer())
            return IsOwner(player);
            
        if (IsOwner(player))
            return true;

        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(player);
        if (!character)
            return false;
        
        auto keysComp = RL_VehicleKeysComponent.Cast(character.FindComponent(RL_VehicleKeysComponent));
        if (m_sGarageId > 0 && keysComp && keysComp.HasVehicleKeyById(m_sGarageId))
            return true;
            
        return false;
    }
    bool IsLocked()
	{
        return !m_sOwnerId.IsEmpty() && m_bLocked;
	}
    bool IsLocked(IEntity player)
	{
        if (Replication.IsServer())
            return !m_sOwnerId.IsEmpty() && m_bLocked;

        if (HasVehicleAccess(player))
            return false;

        return !m_sOwnerId.IsEmpty() && m_bLocked;
	}
    void SetLockedState(bool state)
    {
        m_bLocked = state;
        SCR_BaseLockComponent lockComp = SCR_BaseLockComponent.Cast(GetOwner().FindComponent(SCR_BaseLockComponent));
        if (lockComp) {
            lockComp.UpdateStorageLock(state);
        }
        
        Replication.BumpMe();
    }
	void SetLockPickingCharacter(string characterId){
		m_sLockPickingCharacterId = characterId;
		Replication.BumpMe();
	}
	

	
	string GetLockPickingCharacterId()
	{
		return m_sLockPickingCharacterId;
	}
    int GetVehiclePrice()
    {
        return m_iVehiclePrice;
    }
    void SetProps(int garageId, string ownerId, string ownerName, int vehiclePrice = 0)
	{
        Rpc(RpcAsk_SetProps, garageId, ownerId, ownerName, vehiclePrice);
	}
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SetProps(int garageId, string ownerId, string ownerName, int vehiclePrice)
	{
        Print("[RL_VehicleManagerComponent] RpcAsk_SetProps");
        m_sGarageId = garageId;
        m_sOwnerId = ownerId;
        m_sOwnerName = ownerName;
        m_iVehiclePrice = vehiclePrice;
		Replication.BumpMe();
	}
    
    //------------------------------------------------------------------------------------------------
    void PlayCarAlarmRpc()
    {
        Rpc(RpcDo_PlayCarAlarm);
    }
    
    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_PlayCarAlarm()
    {
        Print("RpcDo_PlayCarAlarm Broadcast");
        PlayCarAlarmSound();
    }
    
    //------------------------------------------------------------------------------------------------
    void PlayCarAlarmSound()
    {
        RL_VehicleManagerComponentClass settings = RL_VehicleManagerComponentClass.Cast(GetComponentData(GetOwner()));
        if (!settings || !settings.m_CarAlarmAudioSourceConfiguration || !settings.m_CarAlarmAudioSourceConfiguration.IsValid())
            return;
        
        SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
        if (!soundManagerEntity)
            return;
        
        SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(GetOwner(), settings.m_CarAlarmAudioSourceConfiguration);
        if (!audioSource)
            return;
        
        vector mat[4];
        GetOwner().GetTransform(mat);
        mat[3] = GetOwner().CoordToParent(settings.m_vCarAlarmSoundOffset);
        soundManagerEntity.PlayAudioSource(audioSource, mat);
        m_CarAlarmAudioHandle = audioSource.m_AudioHandle;

        GetGame().GetCallqueue().CallLater(StopCarAlarmSound, 30000, false);
    }
    
    //------------------------------------------------------------------------------------------------
    void StopCarAlarmSound()
    {
        if (m_CarAlarmAudioHandle != AudioHandle.Invalid)
        {
            AudioSystem.TerminateSoundFadeOut(m_CarAlarmAudioHandle, false, 1000);
            m_CarAlarmAudioHandle = AudioHandle.Invalid;
        }
    }
    
    //------------------------------------------------------------------------------------------------
    bool IsCarAlarmSoundPlaying()
    {
        return AudioSystem.IsSoundPlayed(m_CarAlarmAudioHandle);
    }
    
	override void OnPostInit(IEntity owner)
	{		
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
		SCR_BaseLockComponent lockComp = SCR_BaseLockComponent.Cast(owner.FindComponent(SCR_BaseLockComponent));
		if (lockComp) {
			lockComp.UpdateStorageLock(m_bLocked);
		}
	}
    override event protected void OnDelete(IEntity owner)
	{
        StopCarAlarmSound();
        if(!m_sGarageId)
            return;

		if (Replication.IsServer()) {
			if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
			m_garageHelper.UpdateGarageState(m_sGarageId, 0);
		}
	}
}
