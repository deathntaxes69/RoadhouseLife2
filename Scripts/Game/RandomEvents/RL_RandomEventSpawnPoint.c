[EntityEditorProps(category: "RandomEventSpawn", description: "RandomEventSpawn")]
class RL_RandomEventSpawnPointClass : SCR_PositionClass
{
	
}

class RL_RandomEventSpawnPoint : SCR_Position
{
	[Attribute("", desc: "Unique spawn point ID")]
	protected string m_sSpawnId;
	
	protected IEntity m_ownerEntity;
	protected bool m_bOccupied;
	protected IEntity m_spawnedEntity;
	protected int m_iCleanupTime;
	
	string GetSpawnId() { return m_sSpawnId; }
	bool IsOccupied() { return m_bOccupied; }
	IEntity GetSpawnedEntity() { return m_spawnedEntity; }
	
	//------------------------------------------------------------------------------------------------
	vector GetWorldPosition()
	{
		return GetOrigin();
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_ownerEntity = owner;
		m_bOccupied = false;
		m_spawnedEntity = null;
		m_iCleanupTime = 0;
		
		if (!GetGame().InPlayMode()) 
			return;
		
		RL_RandomEventManager manager = RL_RandomEventManager.GetInstance();
		if (manager)
			manager.RegisterSpawnPoint(this);
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity SpawnEntity(string prefabPath, int cleanupTimeMinutes = 0)
	{
		if (m_bOccupied)
		{
			PrintFormat("[RL_RandomEventSpawnPoint] Spawn point %1 is already occupied", m_sSpawnId);
			return null;
		}
		
		if (!Replication.IsServer())
		{
			return null;
		}
		
		Resource prefabResource = Resource.Load(prefabPath);
		if (!prefabResource)
		{
			PrintFormat("[RL_RandomEventSpawnPoint] Failed to load prefab: %1", prefabPath);
			return null;
		}
		
		vector spawnPos = GetOrigin();
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = spawnPos;
		
		IEntity spawnedEntity = GetGame().SpawnEntityPrefab(prefabResource, GetGame().GetWorld(), spawnParams);
		if (spawnedEntity)
		{
			m_spawnedEntity = spawnedEntity;
			m_bOccupied = true;
			m_iCleanupTime = cleanupTimeMinutes;
			
			PrintFormat("[RL_RandomEventSpawnPoint] Spawned entity at spawn point %1, position %2", m_sSpawnId, spawnPos.ToString());
			
			if (cleanupTimeMinutes > 0)
			{
				GetGame().GetCallqueue().CallLater(CleanupSpawnedEntity, cleanupTimeMinutes * 60000, false);
				PrintFormat("[RL_RandomEventSpawnPoint] Scheduled cleanup for spawn point %1 in %2 minutes", m_sSpawnId, cleanupTimeMinutes);
			}
			
			return spawnedEntity;
		}
		else
		{
			PrintFormat("[RL_RandomEventSpawnPoint] Failed to spawn entity at spawn point %1", m_sSpawnId);
			return null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void CleanupSpawnedEntity()
	{
		if (!m_spawnedEntity)
			return;
			
		PrintFormat("[RL_RandomEventSpawnPoint] Cleaning up spawned entity at spawn point %1", m_sSpawnId);
		
		SCR_EntityHelper.DeleteEntityAndChildren(m_spawnedEntity);
		m_spawnedEntity = null;
		m_bOccupied = false;
		m_iCleanupTime = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	void ForceCleanup()
	{
		if (m_iCleanupTime > 0)
		{
			GetGame().GetCallqueue().Remove(CleanupSpawnedEntity);
		}
		
		CleanupSpawnedEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_RandomEventSpawnPoint()
	{
		ForceCleanup();
		
		RL_RandomEventManager manager = RL_RandomEventManager.GetInstance();
		if (manager)
			manager.UnregisterSpawnPoint(this);
	}
}