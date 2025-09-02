class RL_VehicleSystemComponentClass : ScriptComponentClass
{
	[Attribute(category: "Spawn Settings", defvalue: "0", desc: "If true, disables collision checking when spawning vehicles")]
	bool m_bDisableCollisionCheck;
	
	[Attribute(category: "Spawn Settings", defvalue: "0", desc: "If true, spawns vehicle at nearest road instead of spawn positions")]
	bool m_bSpawnAtNearestRoad;
	
	[Attribute(category: "Spawn Settings", defvalue: "0", desc: "If true, uses house specific spawning logic")]
	bool m_bIsHouse;
}
class RL_VehicleSystemComponent: ScriptComponent
{
    [Attribute(category: "Spawn Positions")]
	protected ref PointInfo m_SpawnPosition;
	
	[Attribute(category: "Spawn Positions", desc: "PointInfo Locations")]
	protected ref array<ref PointInfo> m_AdditionalSpawnPositions;

	[Attribute(category: "Blocked Prefabs", desc: "List of prefab names that cannot be spawned at this location")]
	protected ref array<string> m_BlockedPrefabs;

	protected PointInfo m_SpawnPosFound;
	
    protected RL_GarageUI m_garageMenu;
	
	protected ref RL_GarageDbHelper m_garageHelper;
	protected ref RL_CharacterDbHelper m_characterHelper;
	protected ref array<Managed> m_purchaseContexts = {};

	bool GetDisableCollisionCheck()
	{
		RL_VehicleSystemComponentClass componentData = RL_VehicleSystemComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_bDisableCollisionCheck;
		return false;
	}

	bool GetSpawnAtNearestRoad()
	{
		RL_VehicleSystemComponentClass componentData = RL_VehicleSystemComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_bSpawnAtNearestRoad;
		return false;
	}

	bool GetIsHouse()
	{
		RL_VehicleSystemComponentClass componentData = RL_VehicleSystemComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_bIsHouse;
		return false;
	}

	EntitySpawnParams GetVehicleSpawnTransform(PointInfo m_SpawnPos)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		if (m_SpawnPos) {
			m_SpawnPos.Init(this.GetOwner());
			m_SpawnPos.GetModelTransform(params.Transform);
			params.Transform[0] = this.GetOwner().VectorToParent(params.Transform[0]);
			params.Transform[1] = this.GetOwner().VectorToParent(params.Transform[1]);
			params.Transform[2] = this.GetOwner().VectorToParent(params.Transform[2]);
			params.Transform[3] = this.GetOwner().CoordToParent(params.Transform[3]);
		} else {
			this.GetOwner().GetTransform(params.Transform);
		}

		return params;
	}

	EntitySpawnParams GetRoadSpawnTransform(vector position, float yawAngle)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		vector angles = Vector(0, yawAngle, 0);
		Math3D.AnglesToMatrix(angles, params.Transform);

		params.Transform[3] = position;
		
		return params;
	}

	EntitySpawnParams GetHouseSpawnTransform(PointInfo m_SpawnPos, float forwardOffset)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		// aaaaaaaaaaaaaaaaaaaaaa
		if (m_SpawnPos) {
			m_SpawnPos.Init(this.GetOwner());
			m_SpawnPos.GetModelTransform(params.Transform);
			params.Transform[0] = this.GetOwner().VectorToParent(params.Transform[0]);
			params.Transform[1] = this.GetOwner().VectorToParent(params.Transform[1]);
			params.Transform[2] = this.GetOwner().VectorToParent(params.Transform[2]);
			params.Transform[3] = this.GetOwner().CoordToParent(params.Transform[3]);
			vector tempRight = params.Transform[0];
			params.Transform[0] = params.Transform[2];
			params.Transform[2] = -tempRight;
			vector forwardVector = params.Transform[2];
		params.Transform[3] = params.Transform[3] + (forwardVector * forwardOffset);
		} else {
			this.GetOwner().GetTransform(params.Transform);
		}

		return params;
	}

	bool FindNearestRoadSpawn(string prefabName, out EntitySpawnParams spawnParams)
	{
		ChimeraAIWorld aiWorld = ChimeraAIWorld.Cast(GetGame().GetAIWorld());
		if (!aiWorld)
		{
			Print("[RL_VehicleSystemComponent] No AI World found for road network");
			return false;
		}

		RoadNetworkManager roadNetworkManager = aiWorld.GetRoadNetworkManager();
		if (!roadNetworkManager)
		{
			Print("[RL_VehicleSystemComponent] No Road Network Manager found");
			return false;
		}

		vector ownerPosition = GetOwner().GetOrigin();
		BaseRoad foundRoad;
		float distance;
		
		int result = roadNetworkManager.GetClosestRoad(ownerPosition, foundRoad, distance);
		if (result == 0 || !foundRoad)
		{
			Print("[RL_VehicleSystemComponent] No road found near position");
			return false;
		}

		vector roadPosition;
		if (!roadNetworkManager.GetReachableWaypointInRoad(ownerPosition, ownerPosition, 100.0, roadPosition))
		{
			Print("[RL_VehicleSystemComponent] No reachable waypoint found on road");
			return false;
		}

		array<float> rotations = {0, 45, 90, 135, 180, 225, 270, 315};
		
		foreach (float rotation : rotations)
		{
			Print("[RL_VehicleSystemComponent] FindNearestRoadSpawn foreach");
			EntitySpawnParams params = GetRoadSpawnTransform(roadPosition, rotation);
			
			if (RL_ShopUtils.ValidateSpawnLocation(prefabName, params, null))
			{
				spawnParams = params;
				return true;
			}
		}

		Print("[RL_VehicleSystemComponent] No clear spawn position found on road after trying all rotations");
		return false;
	}

	bool FindHouseSpawn(string prefabName, out EntitySpawnParams spawnParams)
	{
		array<float> forwardOffsets = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0};
		
		foreach (float offset : forwardOffsets)
		{
			Print("[RL_VehicleSystemComponent] FindHouseSpawn foreach");
			EntitySpawnParams params = GetHouseSpawnTransform(m_SpawnPosFound, offset);
			
			if (RL_ShopUtils.ValidateSpawnLocation(prefabName, params, null))
			{
				spawnParams = params;
				return true;
			}
		}
		
		Print("[RL_VehicleSystemComponent] No clear house spawn position found, trying nearest road");
		
		return FindNearestRoadSpawn(prefabName, spawnParams);
	}
	
	int lastCheckTime = -1;
	bool canSpawnCache = false;
	bool CanSpawn(string prefabName, array<IEntity> ignoreEntities = null)
	{
		if (GetDisableCollisionCheck())
		{
			m_SpawnPosFound = m_SpawnPosition;
			return true;
		}
		
		if (GetSpawnAtNearestRoad())
		{
			EntitySpawnParams roadSpawnParams;
			return FindNearestRoadSpawn(prefabName, roadSpawnParams);
		}
		
		if (GetIsHouse())
		{
			EntitySpawnParams houseSpawnParams;
			m_SpawnPosFound = m_SpawnPosition;
			return FindHouseSpawn(prefabName, houseSpawnParams);
		}
		
		int curTick = System.GetTickCount();
		if (curTick - lastCheckTime >= 1000)
		{
			array<IEntity> excludeEntities = {};
			
			if (ignoreEntities)
			{
				excludeEntities.InsertAll(ignoreEntities);
			}

			canSpawnCache = false;
			if (!m_AdditionalSpawnPositions.Contains(m_SpawnPosition))
				m_AdditionalSpawnPositions.InsertAt(m_SpawnPosition, 0);

			foreach (PointInfo m_SpawnPos : m_AdditionalSpawnPositions)
			{
				//todo can we optimize?
				//Print("[RL_VehicleSystemComponent] CanSpawn foreach");
				canSpawnCache = RL_ShopUtils.ValidateSpawnLocation(prefabName, GetVehicleSpawnTransform(m_SpawnPos), excludeEntities);
				m_SpawnPosFound = m_SpawnPos;
				if (canSpawnCache)
					break;
			}

			lastCheckTime = curTick;
		}
		
		return canSpawnCache;
	}
	IEntity Spawn(string prefabName, int garageId, string characterId, string characterName, int vehiclePrice = 0)
	{
		if (!Replication.IsServer()) return null;

		PrintFormat("[VehicleSystemComponent] Spawn prefabName=%1 characterId=%2", prefabName, characterId);
		
		if (m_BlockedPrefabs && m_BlockedPrefabs.Contains(prefabName))
		{
			SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(characterId);
			if (character)
				character.Notify("You cannot spawn that vehicle here.", "GARAGE");
			return null;
		}
		
		bool canSpawn = this.CanSpawn(prefabName, null);
		if (!canSpawn) 
			return null;

		EntitySpawnParams params;
		// why does this check again when its already done in CanSpawn????
		if (GetSpawnAtNearestRoad())
		{
			if (!FindNearestRoadSpawn(prefabName, params))
				return null;
		}
		else if (GetIsHouse())
		{
			if (!FindHouseSpawn(prefabName, params))
				return null;
		}
		else
		{
			params = GetVehicleSpawnTransform(m_SpawnPosFound);
			vector mat[4];
			m_SpawnPosFound.GetTransform(mat);
			mat[3] = params.Transform[3];
			params.Transform = mat;
		}
		Print("[VehicleSystemComponent] Spawn 2");
		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(prefabName), GetGame().GetWorld(), params);
		Print("[VehicleSystemComponent] Spawn 3");
		
		RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(entity.FindComponent(RL_VehicleManagerComponent));
		if(vehicleManagerComponent) {
			vehicleManagerComponent.SetProps(garageId, characterId, characterName, vehiclePrice);
		} else {
			Print("Spawned Vehicle has no RL_VehicleManagerComponent");
		}

		if (entity.GetPhysics())
			entity.GetPhysics().SetActive(true);
		Print("[VehicleSystemComponent] Spawn 4");

		return entity;
	}
}