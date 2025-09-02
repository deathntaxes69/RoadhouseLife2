class RL_BaseUpgradeData
{
	int m_iUpgradeType;
	string m_sUpgradeName;
	string m_sResourceNamePrefab;
	string m_sPurchasedAt;
	
	void RL_BaseUpgradeData()
	{
	}
	
	void SetData(int upgradeType, string upgradeName, string resourceNamePrefab, string purchasedAt)
	{
		m_iUpgradeType = upgradeType;
		m_sUpgradeName = upgradeName;
		m_sResourceNamePrefab = resourceNamePrefab;
		m_sPurchasedAt = purchasedAt;
	}
	
	int GetUpgradeType() { return m_iUpgradeType; }
	string GetUpgradeName() { return m_sUpgradeName; }
	string GetResourceNamePrefab() { return m_sResourceNamePrefab; }
	string GetPurchasedAt() { return m_sPurchasedAt; }
}

class RL_BaseUpgradeJsonData : JsonApiStruct
{
	protected int upgradeType;
	protected string upgradeName;
	protected string resourceNamePrefab;
	protected string purchasedAt;
	
	//------------------------------------------------------------------------------------------------
	void RL_BaseUpgradeJsonData()
	{
		RegV("upgradeType");
		RegV("upgradeName");
		RegV("resourceNamePrefab");
		RegV("purchasedAt");
	}
	
	//------------------------------------------------------------------------------------------------
	int GetUpgradeType()
	{
		return upgradeType;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUpgradeName()
	{
		return upgradeName;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetResourceNamePrefab()
	{
		return resourceNamePrefab;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPurchasedAt()
	{
		return purchasedAt;
	}
}

// Shared upgrade purchase response class
class RL_BaseUpgradePurchaseResponse : JsonApiStruct
{
	protected bool success;
	protected string message;
	protected int newBank;
	protected string propertyId;
	protected int upgradeType;
	protected string upgradeName;
	protected int price;
	protected string resourceNamePrefab;
	
	//------------------------------------------------------------------------------------------------
	void RL_BaseUpgradePurchaseResponse()
	{
		RegV("success");
		RegV("message");
		RegV("newBank");
		RegV("propertyId");
		RegV("upgradeType");
		RegV("upgradeName");
		RegV("price");
		RegV("resourceNamePrefab");
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSuccess() { return success; }
	string GetMessage() { return message; }
	int GetNewBank() { return newBank; }
	string GetPropertyId() { return propertyId; }
	int GetUpgradeType() { return upgradeType; }
	string GetUpgradeName() { return upgradeName; }
	int GetPrice() { return price; }
	string GetResourceNamePrefab() { return resourceNamePrefab; }
}

class RL_BasePropertyPurchaseResponse : JsonApiStruct
{
	protected bool success;
	protected string message;
	protected int newBank;
	protected string propertyId;
	protected int price;
	
	//------------------------------------------------------------------------------------------------
	void RL_BasePropertyPurchaseResponse()
	{
		RegV("success");
		RegV("message");
		RegV("newBank");
		RegV("propertyId");
		RegV("price");
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSuccess() { return success; }
	string GetMessage() { return message; }
	int GetNewBank() { return newBank; }
	string GetPropertyId() { return propertyId; }
	int GetPrice() { return price; }
}

class RL_BaseUpgradeHelper
{
	//------------------------------------------------------------------------------------------------
	static void SpawnUpgrades(IEntity owner, array<ref RL_BaseUpgradeData> upgrades, 
		array<ref RL_HouseUpgradeSpawnPoint> spawnPoints, out array<IEntity> spawnedUpgrades,
		string storageType = "")
	{
		if (!Replication.IsServer() || !upgrades || !spawnPoints)
			return;
		
		ClearSpawnedUpgrades(spawnedUpgrades);
		
		foreach (RL_BaseUpgradeData upgrade : upgrades)
		{
			Print("[RL_BaseUpgradeHelper] SpawnUpgrades foreach loop");
			if (!upgrade)
				continue;
			
			RL_HouseUpgradeSpawnPoint spawnPoint = FindSpawnPointByUpgradeType(spawnPoints, upgrade.m_iUpgradeType);
			if (!spawnPoint)
				continue;
			
			if (upgrade.m_iUpgradeType == HouseUpgrades.STORAGE || upgrade.m_iUpgradeType == HouseUpgrades.STORAGE_APARTMENT)
			{
				SpawnStorageUpgradeStorage(owner, upgrade, spawnPoint, spawnedUpgrades, storageType);
			}
			else
			{
				SpawnUpgradeAtPoint(owner, upgrade, spawnPoint, spawnedUpgrades, storageType);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static RL_HouseUpgradeSpawnPoint FindSpawnPointByUpgradeType(array<ref RL_HouseUpgradeSpawnPoint> spawnPoints, int upgradeType)
	{
		if (!spawnPoints)
			return null;
		
		foreach (RL_HouseUpgradeSpawnPoint point : spawnPoints)
		{
			Print("[RL_BaseUpgradeHelper] FindSpawnPointByUpgradeType foreach loop");
			if (point && point.GetUpgradeType() == upgradeType)
			{
				return point;
			}
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	static void SpawnStorageUpgradeStorage(IEntity owner, RL_BaseUpgradeData upgrade, 
		RL_HouseUpgradeSpawnPoint spawnPoint, out array<IEntity> spawnedUpgrades, string storageType)
	{
		if (!upgrade || !spawnPoint || !owner || storageType.IsEmpty())
			return;

		string ownerPersistenceId = GetOwnerPersistenceId(owner);
		if (ownerPersistenceId.IsEmpty())
			return;
		
		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_StorageHelperComponent storageHelper = RL_StorageHelperComponent.Cast(gameMode.FindComponent(RL_StorageHelperComponent));
		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		
		if (!storageHelper || !persistenceManager)
			return;
		
		int playerId = ownerPersistenceId.ToInt();
		int ownerRplId = EPF_NetworkUtils.GetRplId(owner);
		if (ownerRplId == Replication.INVALID_ID)
			return;
		
		int upgradeType = upgrade.m_iUpgradeType;
		string upgradePrefab = upgrade.m_sResourceNamePrefab;
		
		storageHelper.LoadStorageFromDb(persistenceManager, ownerPersistenceId, playerId, storageType, ownerRplId, upgradeType, upgradePrefab);
	}
	
	//------------------------------------------------------------------------------------------------
	static void FinishStorageUpgradeLoadSimple(IEntity owner, int upgradeType, string upgradePrefab, string storageType, RL_StorageBox_Entity existingStorage)
	{
		if (!owner)
			return;
		
		string ownerPersistenceId = GetOwnerPersistenceId(owner);
		if (ownerPersistenceId.IsEmpty())
			return;
		
		RL_HouseUpgradeSpawnPoint spawnPoint = FindSpawnPointByType(owner, upgradeType);
		BaseGameMode gameMode = GetGame().GetGameMode();
		RL_StorageHelperComponent storageHelper = RL_StorageHelperComponent.Cast(gameMode.FindComponent(RL_StorageHelperComponent));
		
		if (!spawnPoint || !storageHelper)
			return;
	
		PointInfo pointInfo = spawnPoint.GetSpawnPoint();
		if (!pointInfo)
			return;
		
		pointInfo.Init(owner);
		
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		pointInfo.GetModelTransform(spawnParams.Transform);
		
		spawnParams.Transform[0] = owner.VectorToParent(spawnParams.Transform[0]);
		spawnParams.Transform[1] = owner.VectorToParent(spawnParams.Transform[1]);
		spawnParams.Transform[2] = owner.VectorToParent(spawnParams.Transform[2]);
		spawnParams.Transform[3] = owner.CoordToParent(spawnParams.Transform[3]);
		vector spawnPosition = spawnParams.Transform[3];
		
		vector forwardVec = spawnParams.Transform[2];
		vector angles = forwardVec.VectorToAngles();
		float yawRotation = angles[0];
		
		IEntity spawnedStorage = storageHelper.SpawnStorage(
			upgradePrefab,
			spawnPosition,
			existingStorage,
			ownerPersistenceId,
			storageType,
			owner,
			yawRotation
		);
		
		if (spawnedStorage && spawnedStorage.GetPhysics())
			spawnedStorage.GetPhysics().SetActive(true);

		if (!existingStorage && Replication.IsServer())
		{
			RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(spawnedStorage);
			if (storageBox)
			{
				storageBox.SetStorageLoaded(true);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static RL_HouseUpgradeSpawnPoint FindSpawnPointByType(IEntity owner, int upgradeType)
	{
		array<ref RL_HouseUpgradeSpawnPoint> spawnPoints = GetUpgradeSpawnPoints(owner);
		if (!spawnPoints)
			return null;
		
		return FindSpawnPointByUpgradeType(spawnPoints, upgradeType);
	}
	
	//------------------------------------------------------------------------------------------------
	static array<ref RL_HouseUpgradeSpawnPoint> GetUpgradeSpawnPoints(IEntity owner)
	{
		if (!owner)
			return null;
		
		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(owner.FindComponent(RL_HouseComponent));
		if (houseComponent)
		{
			return houseComponent.GetUpgradeSpawnPoints();
		}
		
		RL_ApartmentComponent apartmentComponent = RL_ApartmentComponent.Cast(owner.FindComponent(RL_ApartmentComponent));
		if (apartmentComponent)
		{
			return apartmentComponent.GetUpgradeSpawnPoints();
		}
		
		return null;
	}

	//------------------------------------------------------------------------------------------------
	static void SpawnUpgradeAtPoint(IEntity owner, RL_BaseUpgradeData upgrade, 
		RL_HouseUpgradeSpawnPoint spawnPoint, out array<IEntity> spawnedUpgrades,
		string storageType = "")
	{
		if (!upgrade || !spawnPoint || !owner)
			return;
		
		PointInfo pointInfo = spawnPoint.GetSpawnPoint();
		if (!pointInfo)
			return;
		
		pointInfo.Init(owner);

		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		pointInfo.GetModelTransform(spawnParams.Transform);
		spawnParams.Transform[0] = owner.VectorToParent(spawnParams.Transform[0]);
		spawnParams.Transform[1] = owner.VectorToParent(spawnParams.Transform[1]);
		spawnParams.Transform[2] = owner.VectorToParent(spawnParams.Transform[2]);
		spawnParams.Transform[3] = owner.CoordToParent(spawnParams.Transform[3]);
		
		Resource resource = Resource.Load(upgrade.m_sResourceNamePrefab);
		if (!resource)
			return;
		
		IEntity spawnedEntity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), spawnParams);
		if (spawnedEntity)
		{
			owner.AddChild(spawnedEntity, -1, EAddChildFlags.NONE);
			
			if (!spawnedUpgrades)
				spawnedUpgrades = new array<IEntity>();
			
			spawnedUpgrades.Insert(spawnedEntity);
			
			if (spawnedEntity.GetPhysics())
				spawnedEntity.GetPhysics().SetActive(true);
		}
	}

	//------------------------------------------------------------------------------------------------
	static void ClearSpawnedUpgrades(out array<IEntity> spawnedUpgrades)
	{
		if (!spawnedUpgrades)
			return;
		
		foreach (IEntity entity : spawnedUpgrades)
		{
			Print("[RL_BaseUpgradeHelper] ClearSpawnedUpgrades foreach loop");
			Print(entity);
			if (entity)
			{
				SCR_EntityHelper.DeleteEntityAndChildren(entity);
			}
		}
		
		spawnedUpgrades.Clear();
	}

	//------------------------------------------------------------------------------------------------
	static void UpdateOwnedUpgradeTypes(array<ref RL_BaseUpgradeData> upgrades, out array<int> ownedUpgradeTypes)
	{
		if (!ownedUpgradeTypes)
			ownedUpgradeTypes = new array<int>();
		
		ownedUpgradeTypes.Clear();
		
		if (upgrades)
		{
			foreach (RL_BaseUpgradeData upgrade : upgrades)
			{
				Print("[RL_BaseUpgradeHelper] UpdateOwnedUpgradeTypes foreach loop");
				if (upgrade)
				{
					ownedUpgradeTypes.Insert(upgrade.m_iUpgradeType);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static bool HasUpgradeType(array<int> ownedUpgradeTypes, int upgradeType)
	{
		if (!ownedUpgradeTypes)
			return false;
		
		return ownedUpgradeTypes.Contains(upgradeType);
	}

	//------------------------------------------------------------------------------------------------
	static string GetOwnerPersistenceId(IEntity owner)
	{
		if (!owner)
			return "";
		
		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(owner.FindComponent(RL_HouseComponent));
		if (houseComponent && houseComponent.IsOwned())
		{
			return houseComponent.GetOwnerCid().ToString();
		}
		
		RL_ApartmentComponent apartmentComponent = RL_ApartmentComponent.Cast(owner.FindComponent(RL_ApartmentComponent));
		if (apartmentComponent && apartmentComponent.IsOwned())
		{
			return apartmentComponent.GetOwnerCid().ToString();
		}
		
		return "";
	}
} 