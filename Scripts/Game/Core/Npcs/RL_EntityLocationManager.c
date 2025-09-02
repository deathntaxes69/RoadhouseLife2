class RL_EntityLocationManager
{
	private static ref RL_EntityLocationManager s_instance;
	private ref map<IEntity, ELocationEntityType> m_mEntities = new map<IEntity, ELocationEntityType>();
	
	//------------------------------------------------------------------------------------------------
	static RL_EntityLocationManager GetInstance()
	{
		if (!s_instance)
			s_instance = new RL_EntityLocationManager();
		return s_instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterEntity(IEntity entity, ELocationEntityType entityType)
	{
		if (!entity)
			return;
			
		m_mEntities.Set(entity, entityType);
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterEntity(IEntity entity)
	{
		if (!entity)
			return;
			
		m_mEntities.Remove(entity);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDistanceToEntityType(vector fromPos, ELocationEntityType entityType)
	{
		IEntity nearestEntity = null;
		float nearestDistance = float.MAX;
		
		foreach (IEntity entity, ELocationEntityType type : m_mEntities)
		{
			Print("[RL_EntityLocationManager] GetDistanceToEntityType foreach loop");
			if (!entity)
				continue;
				
			if (type != entityType)
				continue;
				
			float distance = vector.Distance(fromPos, entity.GetOrigin());
			
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestEntity = entity;
			}
		}
		
		if (nearestEntity)
			return nearestDistance;
		else
			return -1;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetNearestEntityOfType(vector fromPos, ELocationEntityType entityType)
	{
		IEntity nearestEntity = null;
		float nearestDistance = float.MAX;
		
		foreach (IEntity entity, ELocationEntityType type : m_mEntities)
		{
			Print("[RL_EntityLocationManager] GetNearestEntityOfType foreach loop");
			if (!entity)
				continue;
				
			if (type != entityType)
				continue;
				
			float distance = vector.Distance(fromPos, entity.GetOrigin());
			
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestEntity = entity;
			}
		}
		
		return nearestEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	array<IEntity> GetEntitiesOfType(ELocationEntityType entityType)
	{
		array<IEntity> entities = {};
		
		foreach (IEntity entity, ELocationEntityType type : m_mEntities)
		{
			Print("[RL_EntityLocationManager] GetEntitiesOfType foreach loop");
			if (!entity)
				continue;
				
			if (type == entityType)
				entities.Insert(entity);
		}
		
		return entities;
	}
} 