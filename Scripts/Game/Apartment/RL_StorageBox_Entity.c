[EntityEditorProps(category: "GameScripted/Misc", description: "")]
class RL_StorageBox_EntityClass : GenericEntityClass
{
}

class RL_StorageBox_Entity : GenericEntity
{
	[RplProp()]
	protected string m_sOwnerPersistenceID;
	
	[RplProp()]
	protected string m_sStorageType;
	
	[RplProp()]
	protected bool m_bIsLoaded = false;
	
	protected RL_HouseComponent m_ParentHouseComponent;
	protected bool m_bRegistered = false;

	//------------------------------------------------------------------------------------------------
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
	}

	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		GetGame().GetCallqueue().CallLater(RegisterWithParentHouse, 1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetOwnerId()
	{
		return m_sOwnerPersistenceID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetOwnerId(string ownerId)
	{
		m_sOwnerPersistenceID = ownerId;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	string GetStorageType()
	{
		return m_sStorageType;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetStorageType(string storageType)
	{
		m_sStorageType = storageType;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsStorageLoaded()
	{
		return m_bIsLoaded;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetStorageLoaded(bool loaded)
	{
		m_bIsLoaded = loaded;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterWithParentHouse()
	{
		if (m_bRegistered)
			return;
		
		IEntity storageEntity = this;
		if (!storageEntity)
			return;
		
		IEntity parent = storageEntity.GetParent();
		if (!parent)
			return;

		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
		if (!houseComponent)
			return;
		
		RplId thisRplId = EPF_NetworkUtils.GetRplId(storageEntity);
		if (thisRplId != Replication.INVALID_ID)
		{
			houseComponent.RegisterStorageBox(storageEntity, thisRplId);
			m_ParentHouseComponent = houseComponent;
			m_bRegistered = true;
		}
	}

	//------------------------------------------------------------------------------------------------
	void RL_StorageBox_Entity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_StorageBox_Entity()
	{
		if (m_bRegistered && m_ParentHouseComponent)
		{
			RplId thisRplId = EPF_NetworkUtils.GetRplId(this);
			if (thisRplId != Replication.INVALID_ID)
			{
				m_ParentHouseComponent.UnregisterStorageBox(thisRplId);
			}
		}
	}
}
