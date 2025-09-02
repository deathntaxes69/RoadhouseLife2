class RL_ShopUtils
{
	// Configuration
	private static const bool ENABLE_DEBUG_MODE = false;
	
	// Cache management
	private static ref map<ResourceName, ref UIInfo> m_itemInfoCache = new map<ResourceName, ref UIInfo>();
	private static ref map<ResourceName, ref Managed> m_vehicleInfoCache = new map<ResourceName, ref Managed>();
	private static ref map<string, ref array<vector>> m_entityBoundsCache = new map<string, ref array<vector>>();
	
	// Debug visualization
	private static autoptr Shape m_debugBoundingShape;
	private static autoptr Shape m_debugCollisionShape;
	
	//------------------------------------------------------------------------------------------------
	// ENTITY AND PREFAB UTILITIES
	//------------------------------------------------------------------------------------------------
	
	//! Retrieves the root prefab resource name from an entity
	static ResourceName ExtractEntityPrefabName(IEntity targetEntity)
	{
		if (!targetEntity)
			return string.Empty;
			
		ResourceName foundPrefabName;
		BaseContainer currentPrefab = targetEntity.GetPrefabData().GetPrefab();
		
		while (foundPrefabName.IsEmpty() && currentPrefab)
		{
			Print("[ShopUtils] ExtractEntityPrefabName while loop");
			foundPrefabName = currentPrefab.GetResourceName();
			currentPrefab = currentPrefab.GetAncestor();
		}
		
		return foundPrefabName;
	}
	
	//! Generic method to extract component data from entity source
	private static BaseContainer ExtractComponentData(IEntitySource entitySource, typename componentType, string attributePath)
	{
		if (!entitySource)
			return null;
			
		int componentCount = entitySource.GetComponentCount();
		for (int componentIndex = 0; componentIndex < componentCount; componentIndex++)
		{
			Print("[ShopUtils] ExtractComponentData for loop");
			IEntityComponentSource componentSource = entitySource.GetComponent(componentIndex);
			if (componentSource.GetClassName().ToType().IsInherited(componentType))
			{
				BaseContainer attributesContainer = componentSource.GetObject("Attributes");
				if (attributesContainer)
				{
					BaseContainer targetContainer = attributesContainer.GetObject(attributePath);
					if (targetContainer)
						return targetContainer;
				}
			}
		}
		
		return null;
	}
	
	//! Generic method to extract UI info from component
	private static BaseContainer ExtractUIInfoFromComponent(IEntitySource entitySource, typename componentType, string uiInfoPath)
	{
		if (!entitySource)
			return null;
			
		int componentCount = entitySource.GetComponentCount();
		for (int componentIndex = 0; componentIndex < componentCount; componentIndex++)
		{
			//Print("[ShopUtils] ExtractUIInfoFromComponent for loop");
			IEntityComponentSource componentSource = entitySource.GetComponent(componentIndex);
			if (componentSource.GetClassName().ToType().IsInherited(componentType))
			{
				BaseContainer uiInfoContainer = componentSource.GetObject(uiInfoPath);
				if (uiInfoContainer)
					return uiInfoContainer;
			}
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	// UI INFO RETRIEVAL
	//------------------------------------------------------------------------------------------------
	
	//! Retrieves cached or loads item UI information
	static UIInfo GetItemDisplayInfo(ResourceName prefabResource)
	{
		UIInfo cachedInfo = m_itemInfoCache.Get(prefabResource);
		if (cachedInfo)
			return cachedInfo;
		
		Resource entityResource = Resource.Load(prefabResource);
		IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(entityResource);
		
		if (entitySource)
		{
			BaseContainer itemDisplayContainer = ExtractComponentData(entitySource, InventoryItemComponent, "ItemDisplayName");
			if (itemDisplayContainer)
			{
				cachedInfo = UIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(itemDisplayContainer));
			}
		}
		
		m_itemInfoCache.Set(prefabResource, cachedInfo);
		return cachedInfo;
	}
	
	//! Retrieves cached or loads vehicle UI information
	static Managed GetVehicleDisplayInfo(ResourceName prefabResource)
	{
		Managed cachedInfo = m_vehicleInfoCache.Get(prefabResource);
		if (cachedInfo)
			return cachedInfo;
		
		Resource entityResource = Resource.Load(prefabResource);
		IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(entityResource);
		
		if (entitySource)
		{
			BaseContainer vehicleUIContainer = ExtractUIInfoFromComponent(entitySource, SCR_EditableVehicleComponent, "m_UIInfo");
			if (vehicleUIContainer)
			{
				cachedInfo = Managed.Cast(BaseContainerTools.CreateInstanceFromContainer(vehicleUIContainer));
			}
		}
		
		m_vehicleInfoCache.Set(prefabResource, cachedInfo);
		return cachedInfo;
	}
	
	//! Gets display name for any prefab (vehicle or item)
	static string GetEntityDisplayName(ResourceName prefabResource)
	{
		// Try vehicle info first
		Managed vehicleDisplayInfo = GetVehicleDisplayInfo(prefabResource);
		
		UIInfo standardVehicleInfo = UIInfo.Cast(vehicleDisplayInfo);
		if (standardVehicleInfo)
			return standardVehicleInfo.GetName();
		
		SCR_EditableEntityUIInfo editableVehicleInfo = SCR_EditableEntityUIInfo.Cast(vehicleDisplayInfo);
		if (editableVehicleInfo)
			return editableVehicleInfo.GetName();
		
		// Fallback to item info
		UIInfo itemDisplayInfo = GetItemDisplayInfo(prefabResource);
		if (itemDisplayInfo)
			return itemDisplayInfo.GetName();
		
		// Return prefab name as last resort
		return prefabResource;
	}
	
	//------------------------------------------------------------------------------------------------
	// DRUG PRICE CALCULATION
	//------------------------------------------------------------------------------------------------
	
	static float CalculateQualityAdjustedPrice(float basePrice, IEntity itemEntity)
	{
		if (!itemEntity)
			return basePrice;
		
		RL_DrugComponent drugComponent = RL_DrugComponent.Cast(itemEntity.FindComponent(RL_DrugComponent));
		if (!drugComponent)
			return basePrice;
		
		float qualityMultiplier = 1.0 + drugComponent.GetQuality();
		return basePrice * qualityMultiplier;
	}

	static int CalculateTotalQualityAdjustedSellPrice(IEntity character, ResourceName prefab, int quantity, int baseSellPrice)
	{
		if (!character || quantity <= 0 || baseSellPrice <= 0)
			return 0;
		
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(
			character.FindComponent(InventoryStorageManagerComponent)
		);
		if (!inventoryManager)
			return baseSellPrice * quantity;
		
		array<IEntity> prefabItems = RL_InventoryUtils.FindItemsByPrefab(inventoryManager, prefab);
		if (!prefabItems || prefabItems.Count() == 0)
			return 0;
		
		int totalPrice = 0;
		int itemsToProcess = Math.Min(quantity, prefabItems.Count());
		
		for (int i = 0; i < itemsToProcess; i++)
		{
			Print("[ShopUtils] CalculateTotalQualityAdjustedSellPrice for loop");
			totalPrice += CalculateQualityAdjustedPrice(baseSellPrice, prefabItems[i]);
		}
		
		return totalPrice;
	}
	
	//------------------------------------------------------------------------------------------------
	// INVENTORY MANAGEMENT
	//------------------------------------------------------------------------------------------------
	
	//! Automatically equips item to appropriate slot
	static bool AutoEquipItemToInventory(SCR_InventoryStorageManagerComponent inventoryManager, IEntity itemEntity)
	{
		if (!inventoryManager || !itemEntity)
			return false;
		
		EStoragePurpose targetPurpose = DetermineItemStoragePurpose(itemEntity);
		
		// Try specific purpose first
		bool itemInserted = inventoryManager.TryInsertItem(itemEntity, targetPurpose, null);
		
		// Fallback to any available slot
		if (!itemInserted)
			itemInserted = inventoryManager.TryInsertItem(itemEntity, EStoragePurpose.PURPOSE_ANY, null);
		
		return itemInserted;
	}
	
	//! Determines appropriate storage purpose for an item
	private static EStoragePurpose DetermineItemStoragePurpose(IEntity itemEntity)
	{
		if (itemEntity.FindComponent(WeaponComponent))
			return EStoragePurpose.PURPOSE_WEAPON_PROXY;
		
		if (itemEntity.FindComponent(BaseLoadoutClothComponent))
			return EStoragePurpose.PURPOSE_LOADOUT_PROXY;
		
		if (itemEntity.FindComponent(SCR_GadgetComponent))
			return EStoragePurpose.PURPOSE_GADGET_PROXY;
		
		return EStoragePurpose.PURPOSE_ANY;
	}
	
	//------------------------------------------------------------------------------------------------
	// SPAWN VALIDATION
	//------------------------------------------------------------------------------------------------
	
	//! Validates if location is clear for spawning entity
	static bool ValidateSpawnLocation(ResourceName prefabResource, EntitySpawnParams spawnParameters, 
		array<IEntity> entitiesToIgnore = null, bool shouldRemoveObstacles = true, 
		BaseWorld targetWorld = null, vector positionOffset = vector.Zero)
	{
		if (!targetWorld)
			targetWorld = GetGame().GetWorld();

		if (!prefabResource || prefabResource.IsEmpty())
			return false;

		if (!m_entityBoundsCache)
			m_entityBoundsCache = new map<string, ref array<vector>>();

		vector transformMatrix[4];
		array<vector> entityBounds = GetOrCreateEntityBounds(prefabResource, spawnParameters, targetWorld);
		
		if (!entityBounds)
			return false;
		
		return PerformCollisionCheck(entityBounds, spawnParameters, entitiesToIgnore, positionOffset);
	}
	
	//! Gets or creates cached entity bounds
	private static array<vector> GetOrCreateEntityBounds(ResourceName prefabResource, EntitySpawnParams spawnParameters, BaseWorld targetWorld)
	{
		if (m_entityBoundsCache.Contains(prefabResource))
		{
			return m_entityBoundsCache.Get(prefabResource);
		}
		
		Resource prefabResourceObj = Resource.Load(prefabResource);
		if (!prefabResourceObj.IsValid())
			return null;

		SCR_PrefabPreviewEntity previewEntity = SCR_PrefabPreviewEntity.Cast(
			SCR_PrefabPreviewEntity.SpawnPreviewFromPrefab(
				prefabResourceObj, 
				"SCR_PrefabPreviewEntity", 
				targetWorld, 
				spawnParameters, 
				"{56EBF5038622AC95}Assets/Conflict/CanBuild.emat"
			)
		);
		
		if (!previewEntity)
			return null;

		vector boundsMin, boundsMax;
		vector tempTransform[4];
		previewEntity.GetTransform(tempTransform);
		previewEntity.GetPreviewBounds(boundsMin, boundsMax);
		
		array<vector> entityBounds = {boundsMin, boundsMax};
		m_entityBoundsCache.Insert(prefabResource, entityBounds);

		SCR_EntityHelper.DeleteEntityAndChildren(previewEntity);
		
		return entityBounds;
	}
	
	//! Performs actual collision detection
	private static bool PerformCollisionCheck(array<vector> entityBounds, EntitySpawnParams spawnParameters, 
		array<IEntity> entitiesToIgnore, vector positionOffset)
	{
		TraceOBB collisionTrace = new TraceOBB();
		Math3D.MatrixIdentity3(collisionTrace.Mat);
		
		vector transformMatrix[4] = spawnParameters.Transform;
		
		collisionTrace.Mat[0] = transformMatrix[0];
		collisionTrace.Mat[1] = transformMatrix[1];
		collisionTrace.Mat[2] = transformMatrix[2];
		collisionTrace.Start = transformMatrix[3] + positionOffset;
		collisionTrace.Flags = TraceFlags.ENTS;
		collisionTrace.ExcludeArray = entitiesToIgnore;
		collisionTrace.LayerMask = EPhysicsLayerPresets.VehicleSimple;
		collisionTrace.Mins = entityBounds[0];
		collisionTrace.Maxs = entityBounds[1];

		if (ENABLE_DEBUG_MODE)
		{
			CreateDebugVisualization(collisionTrace, transformMatrix);
		}

		GetGame().GetWorld().TracePosition(collisionTrace, null);

		if (collisionTrace.TraceEnt)
		{
			if (ENABLE_DEBUG_MODE)
			{
				CreateCollisionDebugVisualization(collisionTrace.TraceEnt);
			}
			return false;
		}

		return true;
	}
	
	//! Creates debug visualization for spawn checking
	private static void CreateDebugVisualization(TraceOBB collisionTrace, vector transformMatrix[4])
	{
		m_debugBoundingShape = Shape.Create(
			ShapeType.BBOX, 
			COLOR_BLUE_A, 
			ShapeFlags.VISIBLE | ShapeFlags.NOZBUFFER | ShapeFlags.WIREFRAME, 
			collisionTrace.Mins, 
			collisionTrace.Maxs
		);
		m_debugBoundingShape.SetMatrix(transformMatrix);
	}
	
	//! Creates debug visualization for collision points
	private static void CreateCollisionDebugVisualization(IEntity collidingEntity)
	{
		vector entityTransform[4];
		collidingEntity.GetTransform(entityTransform);
		m_debugCollisionShape = Shape.CreateSphere(
			COLOR_RED, 
			ShapeFlags.VISIBLE | ShapeFlags.NOZBUFFER | ShapeFlags.WIREFRAME, 
			collidingEntity.GetOrigin(), 
			0.1
		);
		m_debugCollisionShape.SetMatrix(entityTransform);
	}
	
	//------------------------------------------------------------------------------------------------
	// WEAPON AND LOADOUT UTILITIES
	//------------------------------------------------------------------------------------------------
	
	//! Gets weapon slot type in lowercase
	static string GetWeaponSlotType(BaseWeaponComponent weaponComponent) 
	{
		if (!weaponComponent)
			return string.Empty;
			
		string slotType = weaponComponent.GetWeaponSlotType();
		slotType.ToLower();
		return slotType;
	}
	
	//! Extracts loadout area type from inventory slot
	static bool ExtractLoadoutAreaType(InventoryStorageSlot inventorySlot, out typename areaType) 
	{
		LoadoutSlotInfo loadoutSlotInfo = LoadoutSlotInfo.Cast(inventorySlot);
		if (!loadoutSlotInfo)
			return false;
		
		LoadoutAreaType slotAreaType = loadoutSlotInfo.GetAreaType();
		if (!slotAreaType)
			return false;
		
		areaType = slotAreaType.Type();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	// CLOTHING MANAGEMENT
	//------------------------------------------------------------------------------------------------
	
	//! Replaces clothing item on entity
	static void SwapEntityClothing(IEntity targetEntity, ResourceName clothingPrefab, bool useLocalSpawn = true)
	{
		if (!targetEntity || !clothingPrefab)
			return;
        
        IEntity newClothingEntity = CreateClothingEntity(clothingPrefab, targetEntity, useLocalSpawn);
        if (!newClothingEntity)
        	return;
        	
        BaseLoadoutClothComponent newClothingComponent = BaseLoadoutClothComponent.Cast(
        	newClothingEntity.FindComponent(BaseLoadoutClothComponent)
        );
        if (!newClothingComponent)
        	return;

        SCR_InventoryStorageManagerComponent entityInventory = SCR_InventoryStorageManagerComponent.Cast(
        	targetEntity.FindComponent(SCR_InventoryStorageManagerComponent)
        );
        if (!entityInventory)
        	return;
        
        // Remove existing clothing in same slot
        RemoveConflictingClothing(targetEntity, newClothingComponent.GetAreaType().Type(), entityInventory);
        
        // Equip new clothing
        EquipClothingItem(targetEntity, newClothingEntity, useLocalSpawn, entityInventory);
	}
	
	//! Creates new clothing entity
	private static IEntity CreateClothingEntity(ResourceName clothingPrefab, IEntity targetEntity, bool useLocalSpawn)
	{
		EntitySpawnParams spawnParameters();
        spawnParameters.Transform[3] = targetEntity.GetOrigin();
        
        if (useLocalSpawn) 
        {
			Resource clothingResource = Resource.Load(clothingPrefab);
			return GetGame().SpawnEntityPrefabLocal(clothingResource, GetGame().GetWorld(), spawnParameters);
		} 
		else 
		{
			return GetGame().SpawnEntityPrefabEx(clothingPrefab, false, null, spawnParameters);
		}
	}
	
	//! Removes clothing that conflicts with new item
	private static void RemoveConflictingClothing(IEntity targetEntity, typename newClothingAreaType, 
		SCR_InventoryStorageManagerComponent entityInventory)
	{
		array<IEntity> currentClothing = GetEntityClothing(targetEntity);
        foreach (IEntity clothingItem : currentClothing)
		{
			Print("[ShopUtils] RemoveConflictingClothing foreach loop");
            BaseLoadoutClothComponent clothingComponent = BaseLoadoutClothComponent.Cast(
            	clothingItem.FindComponent(BaseLoadoutClothComponent)
            );
            if (!clothingComponent)
                continue;
            
            if (clothingComponent.GetAreaType().Type() == newClothingAreaType)
            {
                entityInventory.TryDeleteItem(clothingItem);
                break;
            }
        }
	}
	
	//! Equips clothing item to entity
	private static void EquipClothingItem(IEntity targetEntity, IEntity clothingEntity, bool useLocalSpawn, 
		SCR_InventoryStorageManagerComponent entityInventory)
	{
		if (useLocalSpawn) 
		{
			EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(
				targetEntity.FindComponent(EquipedLoadoutStorageComponent)
			);
			if (loadoutStorage)
			{
				BaseLoadoutClothComponent clothingComponent = BaseLoadoutClothComponent.Cast(
					clothingEntity.FindComponent(BaseLoadoutClothComponent)
				);
				if (clothingComponent)
				{
					InventoryStorageSlot targetSlot = loadoutStorage.GetSlotFromArea(clothingComponent.GetAreaType().Type());
					if (targetSlot)
						targetSlot.AttachEntity(clothingEntity);
				}
			}
		} 
		else 
		{
			entityInventory.TryInsertItem(clothingEntity);
		}
	}
	
	//! Retrieves all clothing items from entity
	static ref array<IEntity> GetEntityClothing(IEntity targetEntity)
    {
        array<IEntity> clothingItems();
        
        if (!targetEntity)
        	return clothingItems;
        	
        EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(
        	targetEntity.FindComponent(EquipedLoadoutStorageComponent)
        );
        if (!loadoutStorage)
            return clothingItems;
        
        int totalSlots = loadoutStorage.GetSlotsCount();
        for (int slotIndex = 0; slotIndex < totalSlots; slotIndex++)
        {
			Print("[ShopUtils] GetEntityClothing for loop");
            InventoryStorageSlot currentSlot = loadoutStorage.GetSlot(slotIndex);
            if (!currentSlot)
                continue;
            
            IEntity attachedEntity = currentSlot.GetAttachedEntity();
            if (!attachedEntity)
                continue;
            
            if (!attachedEntity.FindComponent(BaseLoadoutClothComponent))
                continue;

            clothingItems.Insert(attachedEntity);
        }
        
        return clothingItems;
    }
	static void RemoveAllClothing(IEntity playerEntity)
	{
		SCR_InventoryStorageManagerComponent entityInventory = SCR_InventoryStorageManagerComponent.Cast(
        	playerEntity.FindComponent(SCR_InventoryStorageManagerComponent)
        );
        if (!entityInventory)
        	return;
		
		EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(
        	playerEntity.FindComponent(EquipedLoadoutStorageComponent)
        );
        if (!loadoutStorage)
            return;
        
        int totalSlots = loadoutStorage.GetSlotsCount();
        for (int slotIndex = 0; slotIndex < totalSlots; slotIndex++)
        {
			Print("[ShopUtils] RemoveAllClothing for loop");
            InventoryStorageSlot currentSlot = loadoutStorage.GetSlot(slotIndex);
            if (!currentSlot)
                continue;
            
			
            IEntity attachedEntity = currentSlot.GetAttachedEntity();
            if (!attachedEntity)
                continue;
            
			SCR_EntityHelper.DeleteEntityAndChildren(attachedEntity);
        }
		/*
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(playerEntity);
		if(!character) return;
		SCR_CharacterControllerComponent cc = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		
		IEntity leftItem = cc.GetAttachedGadgetAtLeftHandSlot();
		if(leftItem)
			SCR_EntityHelper.DeleteEntityAndChildren(leftItem);
		
		IEntity rightItem = cc.GetRightHandItem();
		if(rightItem)
			SCR_EntityHelper.DeleteEntityAndChildren(rightItem);
		*/
 
	}
}