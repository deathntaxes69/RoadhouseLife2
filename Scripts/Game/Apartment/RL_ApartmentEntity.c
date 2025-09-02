[EntityEditorProps(category: "GameScripted/Misc", description: "")]
class RL_ApartmentEntityClass : SCR_DestructibleBuildingEntityClass
{
}

class RL_ApartmentEntity : SCR_DestructibleBuildingEntity
{
	[Attribute()]
	private ref PointInfo m_vPlayerSpawnPoint;

	[Attribute()]
	private ref PointInfo m_vStorageSpawnPoint;

	private vector m_vWorldStorageSpawnPoint;

	private string m_sPlayerPersistenceID;

	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_vPlayerSpawnPoint.Init(this);
		m_vStorageSpawnPoint.Init(this);
	}
	vector GetPlayerSpawnPoint()
	{
		vector mat[4];
		m_vPlayerSpawnPoint.GetModelTransform(mat);
		return this.CoordToParent(mat[3]);
	}
	vector GetStorageSpawnPoint()
	{
		vector mat[4];
		m_vStorageSpawnPoint.GetModelTransform(mat);
		m_vWorldStorageSpawnPoint = this.CoordToParent(mat[3]);
		return m_vWorldStorageSpawnPoint;
	}
	void RL_ApartmentEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
}
