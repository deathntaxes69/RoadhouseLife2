[EntityEditorProps(category: "GameScripted/Housing", description: "House component for buyable properties")]
class RL_HouseComponentClass : ScriptComponentClass
{
	[Attribute(category: "Price", "50000", "Buy price of the house")]
	int m_iBuyPrice;
	
	[Attribute(category: "Price", "35000", "Sell price of the house")]
	int m_iSellPrice;
	
	[Attribute(category: "Furniture", "6", "Maximum number of furniture items that can be placed")]
	int m_iMaxFurniture;
	
	[Attribute(category: "Upgrade Spawn Points", desc: "Upgrade spawn point locations")]
	ref array<ref RL_HouseUpgradeSpawnPoint> m_UpgradeSpawnPoints;
}

class RL_HouseComponent : ScriptComponent
{
	[RplProp(onRplName: "OnOwnerCidReplicated")]
	protected int m_iOwnerCid = -1;
	
	[RplProp()]
	protected string m_sHouseId = "";
	
	[RplProp()]
	protected bool m_bIsLocked = false;
	
	[RplProp()]
	protected ref array<int> m_aOwnedUpgradeTypes;
	
	protected ref array<ref RL_BaseUpgradeData> m_aUpgrades;
	protected ref array<ref RL_DynamicUpgradeData> m_aDynamicUpgrades;
	
	protected vector m_vHouseBoundsMin;
	protected vector m_vHouseBoundsMax;
	
	protected ref array<IEntity> m_aSpawnedUpgrades = {};
	protected RplId m_RegisteredSellingSignId;
	protected RplId m_RegisteredSpawnPointId;
	protected RplId m_RegisteredStorageBoxId;
	protected ref array<RplId> m_aRegisteredDoorIds;
	
	//------------------------------------------------------------------------------------------------
	int GetBuyPrice()
	{
		RL_HouseComponentClass componentData = RL_HouseComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_iBuyPrice;
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetSellPrice()
	{
		RL_HouseComponentClass componentData = RL_HouseComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_iSellPrice;
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMaxFurniture()
	{
		RL_HouseComponentClass componentData = RL_HouseComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_iMaxFurniture;
		return 2;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_HouseUpgradeSpawnPoint> GetUpgradeSpawnPoints()
	{
		RL_HouseComponentClass componentData = RL_HouseComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_UpgradeSpawnPoints;
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerCid()
	{
		return m_iOwnerCid;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetOwnerCid(int ownerCid)
	{
		m_iOwnerCid = ownerCid;
		
		if (ownerCid != -1)
		{
			SetLocked(true);
		}
		else
		{
			SetLocked(false);
			if (Replication.IsServer())
			{
				RL_BaseUpgradeHelper.ClearSpawnedUpgrades(m_aSpawnedUpgrades);
				DeleteHouseStorageBoxes();
				if (m_aUpgrades)
					m_aUpgrades.Clear();
				
				if (m_aDynamicUpgrades)
					m_aDynamicUpgrades.Clear();
				
				if (m_aOwnedUpgradeTypes)
					m_aOwnedUpgradeTypes.Clear();
			}
		}
		
		UpdateSellingSignVisibility();
		
		if (Replication.IsServer())
		{
			Rpc(RpcDo_UpdateSellingSignVisibility);
			UpdateSpawnPointRestriction();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_UpdateSellingSignVisibility()
	{
		Print("RpcDo_UpdateSellingSignVisibility");
		UpdateSellingSignVisibility();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnOwnerCidReplicated()
	{
		UpdateSellingSignVisibility();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSellingSignVisibility()
	{
		if (m_RegisteredSellingSignId == Replication.INVALID_ID)
			return;
		
		IEntity signEntity = EPF_NetworkUtils.FindEntityByRplId(m_RegisteredSellingSignId);
		if (!signEntity)
			return;
		
		RL_HouseSellSignComponent signComponent = RL_HouseSellSignComponent.Cast(signEntity.FindComponent(RL_HouseSellSignComponent));
		if (signComponent)
		{
			signComponent.UpdateVisibility(IsOwned());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterSellingSign(RplId signId)
	{
		m_RegisteredSellingSignId = signId;
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterSellingSign(RplId signId)
	{
		if (m_RegisteredSellingSignId == signId)
		{
			m_RegisteredSellingSignId = Replication.INVALID_ID;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterSpawnPoint(RplId spawnId)
	{
		m_RegisteredSpawnPointId = spawnId;
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterSpawnPoint(RplId spawnId)
	{
		Print("[RL_HouseComponent] UnregisterSpawnPoint called with ID: " + spawnId);
		if (m_RegisteredSpawnPointId == spawnId)
		{
			m_RegisteredSpawnPointId = Replication.INVALID_ID;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterStorageBox(IEntity storageEntity, RplId storageBoxId)
	{
		if(!m_aSpawnedUpgrades)
			m_aSpawnedUpgrades = {};
		if(!m_aSpawnedUpgrades.Contains(storageEntity))
		{
			Print("[RL_HouseComponent] RegisterStorageBox adding to spawned upgrades");
			m_aSpawnedUpgrades.Insert(storageEntity);
		}
		Print("[RL_HouseComponent] RegisterStorageBox called with ID: " + storageBoxId);
		if(m_RegisteredStorageBoxId != Replication.INVALID_ID)
		{
			Print("[RL_HouseComponent] Deleting previous storage box. This should not happen?");
			IEntity oldStorage = EPF_NetworkUtils.FindEntityByRplId(m_RegisteredStorageBoxId);
			string persistenceId = EPF_PersistenceComponent.GetPersistentId(oldStorage);
			Print("Old storage persistence ID: " + persistenceId);
			if(oldStorage)
				SCR_EntityHelper.DeleteEntityAndChildren(oldStorage);
		}
		m_RegisteredStorageBoxId = storageBoxId;
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterStorageBox(RplId storageBoxId)
	{
		if (m_RegisteredStorageBoxId == storageBoxId)
		{
			m_RegisteredStorageBoxId = Replication.INVALID_ID;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterDoor(RplId doorId)
	{
		if (!m_aRegisteredDoorIds)
			m_aRegisteredDoorIds = new array<RplId>();
		
		if (m_aRegisteredDoorIds.Find(doorId) == -1)
		{
			m_aRegisteredDoorIds.Insert(doorId);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterDoor(RplId doorId)
	{
		if (!m_aRegisteredDoorIds)
			return;
		
		int index = m_aRegisteredDoorIds.Find(doorId);
		if (index != -1)
		{
			m_aRegisteredDoorIds.RemoveOrdered(index);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (GetGame().InPlayMode())
		{
			GetGame().GetCallqueue().CallLater(UpdateSellingSignVisibility, 1000, false);
			UpdateSpawnPointRestriction();
		}

	}
	
	//------------------------------------------------------------------------------------------------
	bool IsOwned()
	{
		return m_iOwnerCid != -1;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsLocked()
	{
		return m_bIsLocked;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetLocked(bool locked)
	{
		m_bIsLocked = locked;
		
		if (locked)
		{
			CloseAllDoors();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void ToggleLock()
	{
		SetLocked(!m_bIsLocked);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CloseAllDoors()
	{
		if (!m_aRegisteredDoorIds)
			return;
		
		foreach (RplId doorId : m_aRegisteredDoorIds)
		{
			IEntity doorEntity = EPF_NetworkUtils.FindEntityByRplId(doorId);
			if (!doorEntity)
				continue;
			
			RL_HouseDoorComponent doorComponent = RL_HouseDoorComponent.Cast(doorEntity.FindComponent(RL_HouseDoorComponent));
			if (doorComponent)
			{
				doorComponent.CloseDoor();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPlayerOwner(int characterId)
	{
		return IsOwned() && m_iOwnerCid == characterId;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetHouseId()
	{
		return m_sHouseId;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpgrades(array<ref RL_BaseUpgradeData> upgrades)
	{
		m_aUpgrades = upgrades;
		RL_BaseUpgradeHelper.UpdateOwnedUpgradeTypes(m_aUpgrades, m_aOwnedUpgradeTypes);
		
		if (Replication.IsServer())
		{
			SpawnUpgrades();
			UpdateSpawnPointRestriction();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void AddUpgrade(RL_BaseUpgradeData upgrade)
	{
		if (!upgrade)
		{
			Print("AddUpgrade called with null upgrade!", LogLevel.ERROR);
			return;
		}
		
		if (!m_aUpgrades)
			m_aUpgrades = new array<ref RL_BaseUpgradeData>();
		
		m_aUpgrades.Insert(upgrade);
		RL_BaseUpgradeHelper.UpdateOwnedUpgradeTypes(m_aUpgrades, m_aOwnedUpgradeTypes);
		
		if (Replication.IsServer())
		{
			SpawnUpgrades();
			UpdateSpawnPointRestriction();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SpawnUpgrades()
	{
		array<ref RL_HouseUpgradeSpawnPoint> spawnPoints = GetUpgradeSpawnPoints();
		RL_BaseUpgradeHelper.SpawnUpgrades(GetOwner(), m_aUpgrades, spawnPoints, m_aSpawnedUpgrades, "house");
	}

	//------------------------------------------------------------------------------------------------
	array<ref RL_BaseUpgradeData> GetUpgrades()
	{
		return m_aUpgrades;
	}
	
	//------------------------------------------------------------------------------------------------
	array<int> GetOwnedUpgradeTypes()
	{
		return m_aOwnedUpgradeTypes;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDynamicUpgrades(array<ref RL_DynamicUpgradeData> dynamicUpgrades)
	{
		m_aDynamicUpgrades = dynamicUpgrades;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_DynamicUpgradeData> GetDynamicUpgrades()
	{
		return m_aDynamicUpgrades;
	}
	
	//------------------------------------------------------------------------------------------------
	void AddDynamicUpgrade(RL_DynamicUpgradeData dynamicUpgrade)
	{
		if (!dynamicUpgrade)
			return;
		
		if (!m_aDynamicUpgrades)
			m_aDynamicUpgrades = new array<ref RL_DynamicUpgradeData>();
		
		m_aDynamicUpgrades.Insert(dynamicUpgrade);
	}
	
	//------------------------------------------------------------------------------------------------
	bool RemoveDynamicUpgrade(string upgradeUid)
	{
		if (!m_aDynamicUpgrades || upgradeUid.IsEmpty())
			return false;
		
		for (int i = m_aDynamicUpgrades.Count() - 1; i >= 0; i--)
		{
			Print("[RL_HouseComponent] RemoveDynamicUpgrade for");
			RL_DynamicUpgradeData upgrade = m_aDynamicUpgrades[i];
			if (upgrade && upgrade.GetUpgradeUid() == upgradeUid)
			{
				m_aDynamicUpgrades.RemoveOrdered(i);
				return true;
			}
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetCurrentFurnitureCount()
	{
		if (!m_aDynamicUpgrades)
			return 0;
		return m_aDynamicUpgrades.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsFurnitureAtLimit()
	{
		return GetCurrentFurnitureCount() >= GetMaxFurniture();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsWithinHouse(IEntity entity)
	{
		if (!entity)
			return false;
		
		// maybe change in future to check if the entities bounding box goes outside of the house instead
		vector entityPos = entity.GetOrigin();
		
		return (entityPos[0] >= m_vHouseBoundsMin[0] && entityPos[0] <= m_vHouseBoundsMax[0] &&
				entityPos[1] >= m_vHouseBoundsMin[1] && entityPos[1] <= m_vHouseBoundsMax[1] &&
				entityPos[2] >= m_vHouseBoundsMin[2] && entityPos[2] <= m_vHouseBoundsMax[2]);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSpawnPointRestriction()
	{
		if (!Replication.IsServer())
			return;
		
		if (m_RegisteredSpawnPointId == Replication.INVALID_ID)
			return;
		
		IEntity spawnEntity = EPF_NetworkUtils.FindEntityByRplId(m_RegisteredSpawnPointId);
		if (!spawnEntity)
			return;
		
		RL_HouseSpawnComponent spawnComponent = RL_HouseSpawnComponent.Cast(spawnEntity.FindComponent(RL_HouseSpawnComponent));
		if (spawnComponent)
		{
			bool hasPlayerSpawnUpgrade = HasUpgradeType(HouseUpgrades.PLAYER_SPAWN);
			bool isOwned = IsOwned();
			spawnComponent.UpdateSpawnRestriction(isOwned, m_iOwnerCid, hasPlayerSpawnUpgrade);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasUpgradeType(HouseUpgrades upgradeType)
	{
		return RL_BaseUpgradeHelper.HasUpgradeType(m_aOwnedUpgradeTypes, upgradeType);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteHouseStorageBoxes()
	{
		if (!Replication.IsServer())
			return;
		
		if (m_RegisteredStorageBoxId == Replication.INVALID_ID)
			return;
		
		IEntity storageBoxEntity = EPF_NetworkUtils.FindEntityByRplId(m_RegisteredStorageBoxId);
		if (!storageBoxEntity)
			return;
		
		RL_StorageBox_Entity storageBox = RL_StorageBox_Entity.Cast(storageBoxEntity);
		if (storageBox)
			SCR_EntityHelper.DeleteEntityAndChildren(storageBox);
	}

	//------------------------------------------------------------------------------------------------
	bool GetHouseBounds(out vector houseBoundsMin, out vector houseBoundsMax)
	{
		houseBoundsMin = m_vHouseBoundsMin;
		houseBoundsMax = m_vHouseBoundsMax;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void RL_HouseComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		string entityName = ent.GetName();
		
		if (!GetGame().InPlayMode()) 
			return;
		
		if (!Replication.IsServer())
			return;
		
		if (!m_aOwnedUpgradeTypes)
			m_aOwnedUpgradeTypes = new array<int>();
		
		if (!m_aRegisteredDoorIds)
			m_aRegisteredDoorIds = new array<RplId>();
		
		m_RegisteredSellingSignId = Replication.INVALID_ID;
		m_RegisteredSpawnPointId = Replication.INVALID_ID;
		m_RegisteredStorageBoxId = Replication.INVALID_ID;
		vector position = ent.GetOrigin();
		m_sHouseId = string.Format("%1_%2_%3", 
			Math.Round(position[0]), Math.Round(position[1]), Math.Round(position[2]));
		ent.GetWorldBounds(m_vHouseBoundsMin, m_vHouseBoundsMax);
		
		RL_HouseManager.RegisterHouse(ent, this);
		
		UpdateSellingSignVisibility();
		UpdateSpawnPointRestriction();
	}
} 