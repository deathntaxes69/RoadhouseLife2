class RL_PropertyManagerComponentClass: ScriptComponentClass
{

};
class RL_PropertyManagerComponent: ScriptComponent
{
    [RplProp()]
    int m_iPropertyId;
    [RplProp()]
    string m_sOwnerId = "";
    [RplProp()]
    string m_sOwnerName = "";
    [RplProp()]
    bool m_bLocked = true;
    
    //protected ref RL_PropertyDbHelper m_propertyHelper;
	
    int GetPropertyId()
	{
        return m_iPropertyId;
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
    bool IsLocked()
	{
        return !m_sOwnerId.IsEmpty() && m_bLocked;
	}
    bool IsLocked(IEntity player)
	{
        bool isOwner = this.IsOwner(player);
        if(isOwner && isOwner)
            return false;

        return !m_sOwnerId.IsEmpty() && m_bLocked;
	}
    void SetLockedState(bool state)
    {
        m_bLocked = state;
        Replication.BumpMe();
    }
    void SetProps(int propertyId, string ownerId, string ownerName)
	{
        Rpc(RpcAsk_SetProps, propertyId, ownerId, ownerName);
	}
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SetProps(int propertyId, string ownerId, string ownerName)
	{
        Print("RpcAsk_SetProps");
        Print(propertyId);
        Print(ownerId);
        Print(ownerName);
        m_iPropertyId = propertyId;
        m_sOwnerId = ownerId;
        m_sOwnerName = ownerName;
		Replication.BumpMe();
	}
    
	override void OnPostInit(IEntity owner)
	{		
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
	
};
