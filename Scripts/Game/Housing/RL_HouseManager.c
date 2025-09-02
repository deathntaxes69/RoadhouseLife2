class RL_HouseManager
{
	protected static ref RL_HouseManager s_Instance;
	protected ref RL_HousingDbHelper m_housingHelper;
	protected ref array<ref RL_HouseData> m_aHouses;
	protected ref array<string> m_aRegisteredUniqueIds;
	protected ref map<string, RL_HouseComponent> m_mHouseComponents;
	protected ref map<string, ref RL_HousePrefabBounds> m_mBoundsByUniqueId;
	protected static const bool m_bDebugEnabled = true;
	
	//------------------------------------------------------------------------------------------------
	static RL_HouseManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new RL_HouseManager();
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void RL_HouseManager()
	{
		m_aHouses = new array<ref RL_HouseData>();
		m_aRegisteredUniqueIds = new array<string>();
		m_mHouseComponents = new map<string, RL_HouseComponent>();
		m_mBoundsByUniqueId = new map<string, ref RL_HousePrefabBounds>();
	}
	
	//------------------------------------------------------------------------------------------------
	static void Initialize()
	{
		if (!Replication.IsServer())
			return;
		
		RL_HouseManager instance = GetInstance();
		instance.m_housingHelper = new RL_HousingDbHelper();
		
		Print(string.Format("[HOUSES MY DUDE] Registered houses count: %1", instance.m_aHouses.Count()));
		
		if (instance.m_aHouses.Count() > 0)
		{
			Print("[HOUSES MY DUDE] Syncing houses to database");
			instance.m_housingHelper.SyncHousesToDatabase(instance.m_aHouses, instance, "OnHousesSynced");
		}
		else
		{
			Print("[HOUSES MY DUDE] No houses to sync LOL YOUR SHIT IS BROKEN");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void RegisterHouse(IEntity houseEntity, RL_HouseComponent houseComponent)
	{
		if (!houseEntity || !houseComponent)
			return;
		
		RL_HouseManager instance = GetInstance();
		
		string uniqueId = houseComponent.GetHouseId();
		if (uniqueId.IsEmpty())
			return;
		
		if (instance.m_aRegisteredUniqueIds.Contains(uniqueId))
			return;
			
		int buyPrice = houseComponent.GetBuyPrice();
		int sellPrice = houseComponent.GetSellPrice();
		string prefab = houseEntity.GetPrefabData().GetPrefabName();
		
		RL_HouseData houseData = new RL_HouseData();
		houseData.SetUniqueId(uniqueId);
		houseData.SetPrefab(prefab);
		houseData.SetBuyPrice(buyPrice);
		houseData.SetSellPrice(sellPrice);
		
		instance.m_aHouses.Insert(houseData);
		instance.m_aRegisteredUniqueIds.Insert(uniqueId);
		instance.m_mHouseComponents.Set(uniqueId, houseComponent);

		vector boundsMin, boundsMax;
		if (houseComponent.GetHouseBounds(boundsMin, boundsMax))
		{
			RL_HousePrefabBounds uniqueBounds = new RL_HousePrefabBounds();
			uniqueBounds.SetBounds(boundsMin, boundsMax);
			instance.m_mBoundsByUniqueId.Set(uniqueId, uniqueBounds);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	array<string> GetRegisteredUniqueIds()
	{
		return m_aRegisteredUniqueIds;
	}
	
	//------------------------------------------------------------------------------------------------
	map<string, RL_HouseComponent> GetHouseComponents()
	{
		return m_mHouseComponents;
	}

	//------------------------------------------------------------------------------------------------
	static bool GetHouseBoundsByUniqueId(string uniqueId, out vector boundsMin, out vector boundsMax)
	{
		RL_HouseManager instance = GetInstance();
		if (!instance)
			return false;

		RL_HousePrefabBounds cached = instance.m_mBoundsByUniqueId.Get(uniqueId);
		if (cached)
			return cached.GetBounds(boundsMin, boundsMax);

		RL_HouseComponent comp = instance.m_mHouseComponents.Get(uniqueId);
		if (comp)
		{
			vector minV, maxV;
			if (comp.GetHouseBounds(minV, maxV))
			{
				RL_HousePrefabBounds newBounds = new RL_HousePrefabBounds();
				newBounds.SetBounds(minV, maxV);
				instance.m_mBoundsByUniqueId.Set(uniqueId, newBounds);
				boundsMin = minV;
				boundsMax = maxV;
				return true;
			}
		}

		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnHousesSynced(bool success, string results)
	{
		
		if (!success)
			return;
		
		RL_HouseSyncResponse response = new RL_HouseSyncResponse();
		response.ExpandFromRAW(results);
		
		if (!response.GetSuccess())
			return;
		
		array<ref RL_OwnedHouseData> ownedHouses = response.GetOwnedHouses();
		if (!ownedHouses)
			return;
		
		foreach (RL_OwnedHouseData ownedHouse : ownedHouses)
		{
			Print("[RL_HouseManager] OnHousesSynced foreach loop");
			if (!ownedHouse)
				continue;
			
			string uniqueId = ownedHouse.GetUniqueId();
			int ownerCid = ownedHouse.GetOwnerCid();
			
			if (uniqueId.IsEmpty() || ownerCid <= 0)
				continue;
				
			RL_HouseComponent houseComponent = m_mHouseComponents.Get(uniqueId);
			if (houseComponent)
			{
				houseComponent.SetOwnerCid(ownerCid);
				
				array<ref RL_BaseUpgradeJsonData> upgradesJson = ownedHouse.GetUpgrades();
				if (upgradesJson && upgradesJson.Count() > 0)
				{
					array<ref RL_BaseUpgradeData> upgrades = new array<ref RL_BaseUpgradeData>();
					
					foreach (RL_BaseUpgradeJsonData upgradeJson : upgradesJson)
					{
						Print("[RL_HouseManager] OnHousesSynced foreach loop 2");
						if (!upgradeJson)
							continue;
						
						RL_BaseUpgradeData upgrade = new RL_BaseUpgradeData();
						upgrade.SetData(
							upgradeJson.GetUpgradeType(),
							upgradeJson.GetUpgradeName(),
							upgradeJson.GetResourceNamePrefab(),
							upgradeJson.GetPurchasedAt()
						);
						
						upgrades.Insert(upgrade);
					}
					
					houseComponent.SetUpgrades(upgrades);
				}
				
				array<ref RL_DynamicUpgradeJsonData> dynamicUpgradesJson = ownedHouse.GetDynamicUpgrades();
				int upgradeCount = 0;
				if (dynamicUpgradesJson)
					upgradeCount = dynamicUpgradesJson.Count();
				
				if (dynamicUpgradesJson && dynamicUpgradesJson.Count() > 0)
				{
					array<ref RL_DynamicUpgradeData> dynamicUpgrades = new array<ref RL_DynamicUpgradeData>();
					
					foreach (RL_DynamicUpgradeJsonData dynamicUpgradeJson : dynamicUpgradesJson)
					{
						Print("[RL_HouseManager] OnHousesSynced foreach loop 3");
						if (!dynamicUpgradeJson)
							continue;
						
						RL_DynamicUpgradeData dynamicUpgrade = new RL_DynamicUpgradeData();
						dynamicUpgrade.SetData(
							dynamicUpgradeJson.GetUpgradeUid(),
							dynamicUpgradeJson.GetPrefabPath(),
							dynamicUpgradeJson.GetPosition(),
							dynamicUpgradeJson.GetRotation(),
							dynamicUpgradeJson.GetPlacedAt()
						);
						dynamicUpgrades.Insert(dynamicUpgrade);
					}
					
					houseComponent.SetDynamicUpgrades(dynamicUpgrades);
					SpawnDynamicUpgrades(houseComponent, dynamicUpgrades, ownerCid);
				}
			}
		}
		
		// Suffocation, no breathing. we update it everywhere now idk
		array<string> ownedHouseIds = new array<string>();
		foreach (RL_OwnedHouseData ownedHouse : ownedHouses)
		{
			Print("[RL_HouseManager] OnHousesSynced foreach loop 4");
			if (ownedHouse)
				ownedHouseIds.Insert(ownedHouse.GetUniqueId());
		}
		
		foreach (string uniqueId, RL_HouseComponent houseComponent : m_mHouseComponents)
		{
			Print("[RL_HouseManager] OnHousesSynced foreach loop 5");
			if (houseComponent && !ownedHouseIds.Contains(uniqueId))
			{
				houseComponent.SetOwnerCid(-1);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void SpawnDynamicUpgrades(RL_HouseComponent houseComponent, array<ref RL_DynamicUpgradeData> dynamicUpgrades, int ownerCid)
	{
		if (!Replication.IsServer() || !houseComponent || !dynamicUpgrades)
		{
			bool isServer = Replication.IsServer();
			bool hasComponent = (houseComponent != null);
			bool hasUpgrades = (dynamicUpgrades != null);
			return;
		}
		
		IEntity houseEntity = houseComponent.GetOwner();
		if (!houseEntity)
			return;
		
		foreach (RL_DynamicUpgradeData dynamicUpgrade : dynamicUpgrades)
		{
			Print("[RL_HouseManager] SpawnDynamicUpgrades foreach loop");
			if (!dynamicUpgrade)
				continue;
			
			string prefabPath = dynamicUpgrade.GetPrefabPath();
			vector position = dynamicUpgrade.GetPosition();
			vector rotation = dynamicUpgrade.GetRotation();
			string upgradeUid = dynamicUpgrade.GetUpgradeUid();
			
			Resource prefabResource = Resource.Load(prefabPath);
			if (!prefabResource)
				continue;
			
			EntitySpawnParams spawnParams = new EntitySpawnParams();
			vector transform[4];
			Math3D.MatrixIdentity4(transform);
			Math3D.AnglesToMatrix(rotation, transform);
			transform[3] = position;
			spawnParams.Transform = transform;
			spawnParams.TransformMode = ETransformMode.WORLD;
			
			IEntity spawnedUpgrade = GetGame().SpawnEntityPrefab(prefabResource, GetGame().GetWorld(), spawnParams);
			if (spawnedUpgrade)
			{
				if (spawnedUpgrade.GetPhysics())
					spawnedUpgrade.GetPhysics().SetActive(true);
				RL_PickupComponent pickupComponent = RL_PickupComponent.Cast(spawnedUpgrade.FindComponent(RL_PickupComponent));
				if (pickupComponent)
				{
					pickupComponent.SetUpgradeUid(upgradeUid);
					pickupComponent.SetOwnerUid(ownerCid.ToString());
				}

			}
		}
	}
} 