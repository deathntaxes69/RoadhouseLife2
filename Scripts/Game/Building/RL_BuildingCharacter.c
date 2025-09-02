modded class SCR_ChimeraCharacter
{
	protected static const float MAX_BUILDING_PLACEMENT_DISTANCE = 50.0;
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcRequestBuildingPlacement(vector position, vector angles, ResourceName prefabPath, bool isDynamicUpgrade, ResourceName gadgetPrefab)
	{
		if (!Replication.IsServer())
			return;
		Print("RpcRequestBuildingPlacement");
		
		if (!ValidateServerBuildingPlacement(position, angles, prefabPath))
			return;

		SpawnBuildingOnServer(position, angles, prefabPath, isDynamicUpgrade, gadgetPrefab);
	}
	
	//------------------------------------------------------------------------------------------------
	bool RequestBuildingPlacement(vector position, vector angles, ResourceName prefabPath, bool isDynamicUpgrade, ResourceName gadgetPrefab)
	{
		if (!RL_PlacementComponentClass.ValidateResourceName(prefabPath))
			return false;

		RplComponent rplComponent = GetRplComponent();
		if (!rplComponent || !rplComponent.IsOwner())
			return false;
		
		Rpc(RpcRequestBuildingPlacement, position, angles, prefabPath, isDynamicUpgrade, gadgetPrefab);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ValidateServerBuildingPlacement(vector position, vector angles, ResourceName prefabPath)
	{
		if (!RL_PlacementComponentClass.ValidateResourceName(prefabPath))
			return false;
		
		vector playerPos = GetOrigin();
		float distance = vector.Distance(position, playerPos);
		
		return distance <= MAX_BUILDING_PLACEMENT_DISTANCE;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ValidateDynamicUpgradeLimit()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return false;
		
		string ownedHouseId = character.GetOwnedHouseId();
		if (ownedHouseId.IsEmpty())
			return false;
		
		RL_HouseManager houseManager = RL_HouseManager.GetInstance();
		if (!houseManager)
			return false;
		
		map<string, RL_HouseComponent> houseComponents = houseManager.GetHouseComponents();
		RL_HouseComponent houseComponent = houseComponents.Get(ownedHouseId);
		
		if (!houseComponent)
			return false;
		
		int currentCount = houseComponent.GetCurrentFurnitureCount();
		int maxCount = houseComponent.GetMaxFurniture();
		PrintFormat("[BuildingCharacter] Current furniture count: %1/%2", currentCount, maxCount);
		
		if (houseComponent.IsFurnitureAtLimit())
		{
			character.Notify(string.Format("Furniture limit reached! (%1/%2)", currentCount, maxCount), "Building");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SpawnBuildingOnServer(vector position, vector angles, ResourceName prefabPath, bool isDynamicUpgrade, ResourceName gadgetPrefab)
	{
		if (isDynamicUpgrade && !ValidateDynamicUpgradeLimit())
			return;
			
		Resource prefabResource = Resource.Load(prefabPath);
		if (!prefabResource)
			return;
		
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		vector transform[4];
		Math3D.MatrixIdentity4(transform);
		Math3D.AnglesToMatrix(angles, transform);
		transform[3] = position;
		spawnParams.Transform = transform;
		spawnParams.TransformMode = ETransformMode.WORLD;
		
		IEntity spawnedBuilding = GetGame().SpawnEntityPrefab(prefabResource, GetGame().GetWorld(), spawnParams);
		if (spawnedBuilding)
		{
			if (spawnedBuilding.GetPhysics())
				spawnedBuilding.GetPhysics().SetActive(true);

			RL_PickupComponent pickupComponent = RL_PickupComponent.Cast(spawnedBuilding.FindComponent(RL_PickupComponent));
			if (pickupComponent)
			{
				SCR_ChimeraCharacter characterCast = SCR_ChimeraCharacter.Cast(this);
				if (characterCast)
				{
					string ownerUid = characterCast.GetCharacterId();
					pickupComponent.SetOwnerUid(ownerUid);
					if (isDynamicUpgrade)
					{
						string upgradeUid = RL_Utils.GenerateRandomUid();
						pickupComponent.SetUpgradeUid(upgradeUid);
						characterCast.AddDynamicUpgrade(upgradeUid, prefabPath, position, angles);
					}
				}
			}
			
			if (!gadgetPrefab.IsEmpty())
				int removedCount = RL_InventoryUtils.RemoveAmount(this, gadgetPrefab, 1);
		}
	}
}