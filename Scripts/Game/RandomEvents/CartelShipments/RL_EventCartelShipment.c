modded class RL_RandomEventManager
{
	
	const string HELI_CRASH_PREFAB = "{B93D3EE058C9011B}Prefabs/RandomEvents/CartelShipments/CartelHeliCrash.et";
	const string SHIPMENT_PREFAB = "{5C72091CB5B5AF13}Prefabs/RandomEvents/CartelShipments/CartelShipmentCrate.et";
	const string COCAINE_PREFAB = "{58E47FAA721D5EEC}Prefabs/Supplies/Cocaine_Brick.et";
	const string HEROIN_PREFAB = "{D0EAC6344C6CCAC8}Prefabs/Supplies/Heroin_Brick.et";
	const string FENTANYL_PREFAB = "{51F5F5E5E6A0C0EB}Prefabs/Supplies/Fentanyl_Bottle.et";
	const string WEED_SEED_PREFAB = "{6A7E0BA984422860}Prefabs/DrugPlanting/Weed/Weed_Seed_Bag.et";
	const string MAC10_RECIPE_PREFAB = "{0D50CFAA2E8A9938}Prefabs/Crafting/Recipes/RL_Mac10Recipe.et";
	const string SPECTRE_DRUM_MAG_RECIPE_PREFAB = "{0B2C1CE0FEB8B007}Prefabs/Crafting/Recipes/RL_SpectreDrumMagRecipe.et";
	
	//-----------------------------------------------------------------------------------------------
	void EventCartelShipment()
	{
		SpawnCartelEvent("cartelShipment", SHIPMENT_PREFAB, "cartel shipment");
	}
	
	//-----------------------------------------------------------------------------------------------
	void EventCartelHeliCrash()
	{
		SpawnCartelEvent("cartelHeli", HELI_CRASH_PREFAB, "cartel heli crash");
	}
	
	//-----------------------------------------------------------------------------------------------
	void SpawnCartelEvent(string spawnPointType, string prefabPath, string eventName)
	{
		if (Replication.IsServer())
		{
			ref array<RL_RandomEventSpawnPoint> spawnPoints = GetAvailableSpawnPoints(spawnPointType);
			
			if (!spawnPoints || spawnPoints.Count() == 0)
			{
				PrintFormat("[RL_EventCartelShipment] No available %1 spawn points found", spawnPointType);
				return;
			}
			
			RL_RandomEventSpawnPoint spawnPoint = spawnPoints.GetRandomElement();
			if (!spawnPoint)
				return;
			
			IEntity spawnedEntity = spawnPoint.SpawnEntity(prefabPath, 30);
			if (spawnedEntity)
			{
				PrintFormat("[RL_EventCartelShipment] Spawned %1 at spawn point %2", eventName, spawnPoint.GetSpawnId());
				FillCartelShipmentCrate(spawnedEntity);
				
				string markerText = "Cartel Shipment";
				if (spawnPointType == "cartelHeli")
					markerText = "Cartel Heli Crash";
				
				RL_MapUtils.CreateMarkerServer(spawnPoint.GetWorldPosition(), markerText, "DRUGS", Color.Red, 1800000, RL_MARKER_TYPE.CivOnly);
				RollPoliceDetection(spawnPointType, spawnPoint.GetWorldPosition());
				
				string messageTitle = "CARTEL ACTIVITY";
				string message = "A cartel shipment has arrived in Lakeside.";
				if(spawnPointType == "cartelHeli")
				{
					message = "There are reports of a cartel trafficking helicopter crash.";
				}
				RL_Utils.NotifyAllCivs(message, messageTitle);
			}
			else
			{
				PrintFormat("[RL_EventCartelShipment] Failed to spawn %1", eventName);
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	void FillCartelShipmentCrate(IEntity crateEntity)
	{
		if (!crateEntity)
			return;
		
		SCR_UniversalInventoryStorageComponent storageComponent = SCR_UniversalInventoryStorageComponent.Cast(crateEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
		if (!storageComponent)
			return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(crateEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return;
		
		TrySpawnItems(crateEntity, COCAINE_PREFAB, 50, 50, 1);
		TrySpawnItems(crateEntity, FENTANYL_PREFAB, 3, 8, 1);
		TrySpawnItems(crateEntity, MAC10_RECIPE_PREFAB, 1, 1, 20);
		TrySpawnItems(crateEntity, SPECTRE_DRUM_MAG_RECIPE_PREFAB, 1, 1, 30);
		TrySpawnWeedSeed(crateEntity);
		storageComponent.SetStorageLock(true);
	}
	
	//-----------------------------------------------------------------------------------------------
	void TrySpawnItems(IEntity crateEntity, string prefabPath, int minCount, int maxCount, int chanceToSpawn)
	{
		if (Math.RandomInt(1, chanceToSpawn) != 1)
			return;
		
		SCR_UniversalInventoryStorageComponent storageComponent = SCR_UniversalInventoryStorageComponent.Cast(crateEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
		if (!storageComponent)
			return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(crateEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return;
		
		int itemCount = Math.RandomInt(minCount, maxCount);
		
		Resource itemResource = Resource.Load(prefabPath);
		if (!itemResource)
			return;
		
		for (int i = 0; i < itemCount; i++)
		{
			IEntity item = GetGame().SpawnEntityPrefab(itemResource);
			if (!item)
				continue;
			
			if (!inventoryManager.TryInsertItemInStorage(item, storageComponent))
			{
				SCR_EntityHelper.DeleteEntityAndChildren(item);
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	void TrySpawnWeedSeed(IEntity crateEntity)
	{
		if (Math.RandomInt(1, 5) != 1)
			return;
		
		SCR_UniversalInventoryStorageComponent storageComponent = SCR_UniversalInventoryStorageComponent.Cast(crateEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
		if (!storageComponent)
			return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(crateEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return;
		
		Resource weedSeedResource = Resource.Load(WEED_SEED_PREFAB);
		if (!weedSeedResource)
			return;
		
		IEntity weedSeedItem = GetGame().SpawnEntityPrefab(weedSeedResource);
		if (!weedSeedItem)
			return;
		
		RL_WeedSeedComponent weedSeedComp = RL_WeedSeedComponent.Cast(weedSeedItem.FindComponent(RL_WeedSeedComponent));
		if (weedSeedComp)
		{
			weedSeedComp.SetIndoorOnly(true);
			weedSeedComp.SetYield(3);
			weedSeedComp.SetQuality(0.5);
			weedSeedComp.SetGrowSpeedInterval(400);
			weedSeedComp.ChangeStrainName("WiFi 41");
		}
		
		if (!inventoryManager.TryInsertItemInStorage(weedSeedItem, storageComponent))
		{
			SCR_EntityHelper.DeleteEntityAndChildren(weedSeedItem);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	void RollPoliceDetection(string spawnPointType, vector position)
	{
		int randomChance = Math.RandomInt(1, 5);
		if (randomChance == 1)
		{
			string messageTitle = "SUSPICIOUS ACTIVITY REPORTED";
			
			string message = "An informant has reported a cartel narco shipment.";
			string markerMessage = "Cartel Shipment Report";
			if(spawnPointType == "cartelHeli")
			{
				markerMessage = "Cartel Heli Crash Report";
				message = "Reports of a cartel trafficking helicopter crash.";
			}
			
			RL_Utils.NotifyAllPolice(message, messageTitle);
			RL_MapUtils.CreateMarkerFromPrefabServer(position, "{CFDEC4E7A4407DCF}Prefabs/World/Locations/Common/RL_MapMarker_PoliceEmergency.et", markerMessage, 1800000);
		}
	}
	
}