modded class RL_RandomEventManager
{
	const string CHEMICAL_TRUCK_PREFAB = "{83FAFBE1A74E5C9C}Prefabs/Vehicles/Wheeled/HualHogan/HaulHoganDuPont.et";
	
	protected ref array<RL_TruckEventDropoffLocationComponent> m_aTruckDropoffLocations = {};
	
	//-----------------------------------------------------------------------------------------------
	void InitializeTruckDropoffLocations()
	{
		m_aTruckDropoffLocations = RL_TruckEventDropoffLocationComponent.GetAllDropoffLocations();
		PrintFormat("[RL_EventChemicalTruck] Found %1 truck dropoff locations", m_aTruckDropoffLocations.Count());
	}
	
	//-----------------------------------------------------------------------------------------------
	void EventChemicalTruck()
	{
		SpawnChemicalTruckEvent("chemicalTruck", CHEMICAL_TRUCK_PREFAB, "chemical truck");
	}
	
	//-----------------------------------------------------------------------------------------------
	void SpawnChemicalTruckEvent(string spawnPointType, string prefabPath, string eventName)
	{
		if (Replication.IsServer())
		{
			ref array<RL_RandomEventSpawnPoint> spawnPoints = GetAvailableSpawnPoints(spawnPointType);
			
			if (!spawnPoints || spawnPoints.Count() == 0)
			{
				PrintFormat("[RL_EventChemicalTruck] No available %1 spawn points found", spawnPointType);
				return;
			}
			
			RL_TruckEventDropoffLocationComponent.ClearAllActiveDropoffs();
			if (m_aTruckDropoffLocations.Count() == 0)
				InitializeTruckDropoffLocations();
			
			if (m_aTruckDropoffLocations.Count() == 0)
			{
				PrintFormat("[RL_EventChemicalTruck] No truck dropoff locations found");
				return;
			}
			
			RL_TruckEventDropoffLocationComponent selectedDropoff = m_aTruckDropoffLocations.GetRandomElement();
			if (!selectedDropoff)
			{
				PrintFormat("[RL_EventChemicalTruck] Failed to select dropoff location");
				return;
			}
			
			selectedDropoff.SetAsActiveDropoff(true);
			RL_RandomEventSpawnPoint spawnPoint = spawnPoints.GetRandomElement();
			if (!spawnPoint)
				return;
			
			IEntity spawnedEntity = spawnPoint.SpawnEntity(prefabPath, 120);
			if (spawnedEntity)
			{
				PrintFormat("[RL_EventChemicalTruck] Spawned %1 at spawn point %2", eventName, spawnPoint.GetSpawnId());
				Vehicle truck = Vehicle.Cast(spawnedEntity);
				if (truck)
				{
					RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(truck.FindComponent(RL_VehicleManagerComponent));
					if (vehicleManagerComponent)
					{
						//set propsm didnt work i think due to being called close to setlocked
						vehicleManagerComponent.m_sGarageId = -1;
						vehicleManagerComponent.m_sOwnerId = "SYSTEM";
						vehicleManagerComponent.m_sOwnerName = "Chemical Truck Event";
						vehicleManagerComponent.SetLockedState(true);
					}
					
					RL_TruckEventComponent truckEventComponent = RL_TruckEventComponent.Cast(truck.FindComponent(RL_TruckEventComponent));
					if (truckEventComponent)
						truckEventComponent.SetDropoffLocation(selectedDropoff.GetWorldPosition());
				}
				
				string markerText = "Chemical Truck";
				RL_MapUtils.CreateMarkerServer(spawnPoint.GetWorldPosition(), markerText, "TRUCK", Color.Yellow, 7200000, RL_MARKER_TYPE.CivOnly);
				RL_Utils.NotifyAllCivs("A Dulect chemical truck has been spotted in the area", "TRUCK");
				RollPoliceDetectionChemicalTruck(spawnPoint.GetWorldPosition());
			}
			else
			{
				PrintFormat("[RL_EventChemicalTruck] Failed to spawn %1", eventName);
				selectedDropoff.SetAsActiveDropoff(false);
			}
				}
	}
	
	//-----------------------------------------------------------------------------------------------
	void RollPoliceDetectionChemicalTruck(vector position)
	{
		int randomChance = Math.RandomInt(1, 3);
		if (randomChance == 1)
		{
			string messageTitle = "INFORMANT REPORT";
			string message = "An informant has reported a plot drop off a stolen chemical truck soon";
			string markerMessage = "Stolen Chemical Truck Dropoff";
			RL_Utils.NotifyAllPolice(message, messageTitle);
			RL_MapUtils.CreateMarkerFromPrefabServer(position, "{CFDEC4E7A4407DCF}Prefabs/World/Locations/Common/RL_MapMarker_PoliceEmergency.et", markerMessage, 1800000);
		}
	}
}