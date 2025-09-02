class RL_GrowingComponentClass: ScriptComponentClass {}

class RL_GrowingComponent: ScriptComponent {
  [RplProp()]
  int growthStage;

  [RplProp()]
  bool watered;

  [RplProp()]
  bool isHybridSpawn;

  [RplProp()]
  bool m_bHasCheckedForBreeding;

  [Attribute(desc: "List of models for each growth stage", params: "xob")]
  private ref array < ResourceName > models;

  [Attribute(defvalue: "420", desc: "Default growth interval in seconds")]
  private float GrowthInterval;

  [Attribute(defvalue: "420", desc: "Growth interval when watered")]
  private float WateredGrowthInterval;

  [Attribute(desc: "Prefab name of the item to be harvested")]
  private ResourceName harvestedItemPrefab; // toDO Ben: Is set and getted inside the harvest action -> here obsolete?

  [Attribute(desc: "Required watering item")]
  private ResourceName wateringItem;

  [Attribute(defvalue: "10", desc: "Water used per watering")]
  protected int m_iWaterUsage;

  [Attribute(defvalue: "0", desc: "Enable debug print statements", category: "Debug")]
  protected bool m_bDebugEnabled;

  protected ref array<IEntity> m_aNearbyPlants;
  protected bool m_bFoundNearbyPlantingSpot = false;

  float growthTime;
  float m_DebugPrintTime;

  override void OnPostInit(IEntity owner) {
    super.OnPostInit(owner);
    SetEventMask(owner, EntityEvent.FRAME);
  }

  protected override bool RplSave(ScriptBitWriter writer) {
    bool result = super.RplSave(writer);
    writer.WriteInt(growthStage);
    writer.WriteBool(watered);
    writer.WriteBool(isHybridSpawn);
    writer.WriteBool(m_bHasCheckedForBreeding);
    return result;
  }

  protected override bool RplLoad(ScriptBitReader reader) {
    bool result = super.RplLoad(reader);
    reader.ReadInt(growthStage);
    reader.ReadBool(watered);
    reader.ReadBool(isHybridSpawn);
    reader.ReadBool(m_bHasCheckedForBreeding);
    UpdateGrowth(false);
    return result;
  }

  override void EOnFrame(IEntity owner, float timeSlice) {
    if (Replication.IsClient())
      return;

    growthTime += timeSlice;
    m_DebugPrintTime += timeSlice;

    float interval;
    RL_WeedSeedComponent seedComponent = RL_WeedSeedComponent.Cast(owner.FindComponent(RL_WeedSeedComponent));
    
    if (seedComponent) {
      interval = seedComponent.GetGrowSpeedInterval();
      
      // TODO DELETE ME IN FUTURE, FIX FOR DEBUG MODE WEED INCIDENT OF 24/07/2025
      if (interval <= 199) {
        interval = 420;
      }
      
      // Apply watering bonus 15% faster
      if (watered) {
        interval *= 0.85;
      }
      
      // Apply weed soil bonus 15% faster
      if (IsOnWeedSoil(owner)) {
        interval *= 0.85;
      }
    } else {
      if (watered) {
        interval = WateredGrowthInterval;
      } else {
        interval = GrowthInterval;
      }
    }

    if (m_DebugPrintTime >= 1.0) {
      if (m_bDebugEnabled)
        PrintFormat("GrowthTime=%1 / Interval=%2 | Watered=%3", growthTime, interval, watered);

      m_DebugPrintTime = 0;
    }

    if (growthTime >= interval) {
      if (m_bDebugEnabled)
        PrintFormat("Growth triggered at stage %1", growthStage);

      UpdateGrowth();
      growthTime = 0;
      Replication.BumpMe();
    }
  }

  [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
  protected void RpcAsk_UpdateGrowth(int newGrowthStage) {
    Print("RpcAsk_UpdateGrowth");
    growthStage = newGrowthStage;
    UpdateGrowth(false);
    Replication.BumpMe();
  }

  void UpdateGrowth(bool inc = true) {
    if (growthStage < models.Count()) {
      IEntity owner = GetOwner();
      if (owner) {
        ResourceName modelRes = models.Get(growthStage);
        Resource res = Resource.Load(modelRes);
        if (res && res.IsValid()) {
          VObject obj = res.GetResource().ToVObject();
          if (obj)
            owner.SetObject(obj, "");
        }
      }

      if (inc) {
        growthStage++;
        if (m_bDebugEnabled)
          PrintFormat("Incremented GrowthStage to %1", growthStage);

        Rpc(RpcAsk_UpdateGrowth, growthStage);

        // Check if we should spawn a hybrid plant when fully grown
        if (CanHarvest() && !m_bHasCheckedForBreeding) {
          m_bHasCheckedForBreeding = true;
          GetGame().GetCallqueue().CallLater(CheckForCrossBreeding, 1000);
          Replication.BumpMe();
        }
      }
    }
  }

  bool CanHarvest() {
    return growthStage >= models.Count();
  }
  bool IsWatered() {
    return watered;
  }
  int GetGrowthStage() {
    return growthStage;
  }
  float GetGrowthTime() {
    return growthTime;
  }
  int GetMaxGrowthStage() {
    return models.Count();
  }
  ResourceName GetWateringItem() {
    return wateringItem;
  }
  int GetWaterUsage() {
    return m_iWaterUsage;
  }

  void SetWatered() {
    if (!watered) {
      watered = true;
      if (m_bDebugEnabled)
        Print("RL_GrowingComponent: Set watered = true");
      Replication.BumpMe();
    }
  }

  //------------------------------------------------------------------------------------------------
  protected bool IsOnWeedSoil(IEntity owner) {
    if (!owner)
      return false;
      
    vector plantPosition = owner.GetOrigin();

    TraceParam traceParam = new TraceParam();
    traceParam.Start = plantPosition + Vector(0, 1, 0);
    traceParam.End = plantPosition + Vector(0, -5, 0);
    traceParam.LayerMask = 0xffffffff;
    traceParam.Flags = TraceFlags.WORLD;

    float traceDistance = GetGame().GetWorld().TraceMove(traceParam, null);
    
    if (traceDistance > 0 && traceParam.TraceMaterial) {
      return traceParam.TraceMaterial == "Terrains/Common/Surfaces/WeedSoilPaint.emat";
    }
    
    return false;
  }

  //------------------------------------------------------------------------------------------------
  void SetGrowthInterval(float interval) {
    GrowthInterval = interval;
  }

  //------------------------------------------------------------------------------------------------
  void SetHybridSpawn(bool hybrid) {
    isHybridSpawn = hybrid;
    Replication.BumpMe();
  }

  //------------------------------------------------------------------------------------------------
  bool IsHybridSpawn() {
    return isHybridSpawn;
  }

  //------------------------------------------------------------------------------------------------
  protected void CheckForCrossBreeding() {
    if (!Replication.IsServer())
      return;

    if (isHybridSpawn)
      return;

    if (growthStage != models.Count())
      return;

    IEntity owner = GetOwner();
    if (!owner)
      return;

    vector plantPosition = owner.GetOrigin();
    m_aNearbyPlants = {};
    
    GetGame().GetWorld().QueryEntitiesBySphere(plantPosition, 9.0, FilterNearbyFullyGrownPlants);
    if (m_aNearbyPlants.Count() == 0)
      return;

    if (HasRecentBreedingNearby(plantPosition, 25.0))
      return;

    //50% chance to crossbreed
    if (Math.RandomFloat01() > 0.50)
      		return;

    IEntity partnerPlant = m_aNearbyPlants[0];
    PerformCrossBreeding(owner, partnerPlant, plantPosition);
  }

  //------------------------------------------------------------------------------------------------
  protected bool FilterNearbyFullyGrownPlants(IEntity entity) {
    if (!entity || entity == GetOwner())
      return true;

    RL_GrowingComponent otherGrowComponent = RL_GrowingComponent.Cast(entity.FindComponent(RL_GrowingComponent));
    if (!otherGrowComponent || !otherGrowComponent.CanHarvest())
      return true;

    m_aNearbyPlants.Insert(entity);
    return true;
  }

  //------------------------------------------------------------------------------------------------
  protected bool HasRecentBreedingNearby(vector position, float radius) {
    m_bFoundNearbyPlantingSpot = false;
    GetGame().GetWorld().QueryEntitiesBySphere(position, radius, FilterPlantingSpots);
    return m_bFoundNearbyPlantingSpot;
  }

  //------------------------------------------------------------------------------------------------
  protected bool FilterPlantingSpots(IEntity entity) {
    if (!entity || entity == GetOwner())
      return true;
    
    RL_GrowingComponent growComponent = RL_GrowingComponent.Cast(entity.FindComponent(RL_GrowingComponent));
    if (growComponent && growComponent.IsHybridSpawn()) {
      m_bFoundNearbyPlantingSpot = true;
      return false;
    }
    
    return true;
  }

  //------------------------------------------------------------------------------------------------
  protected void PerformCrossBreeding(IEntity parent1, IEntity parent2, vector breedingPosition) {

    RL_WeedSeedComponent seed1 = RL_WeedSeedComponent.Cast(parent1.FindComponent(RL_WeedSeedComponent));
    RL_WeedSeedComponent seed2 = RL_WeedSeedComponent.Cast(parent2.FindComponent(RL_WeedSeedComponent));
    
    if (!seed1 || !seed2)
      return;

    vector parent1Pos = parent1.GetOrigin();
    vector parent2Pos = parent2.GetOrigin();
    vector spawnPosition;
    string groundMaterial;
    
    if (!FindBreedingSpawnPosition(parent1Pos, parent2Pos, spawnPosition, groundMaterial))
      return;

    bool hybridIndoorOnly = seed1.IsIndoorOnly() || seed2.IsIndoorOnly();
    float maxQuality;
    int maxYield;
    int minGrowSpeed;
    
    if (hybridIndoorOnly) {
      maxQuality = 1.0;
      maxYield = 12;
      minGrowSpeed = 200;
    } else {
      maxQuality = 0.7;
      maxYield = 6;
      minGrowSpeed = 260;
    }

    // QUALITY 25% chance for variance
    float hybridQuality = (seed1.GetQuality() + seed2.GetQuality()) / 2.0;
    if (Math.RandomFloat01() < 0.25) {
      int varianceStep = Math.RandomIntInclusive(-10, 10);
      if (varianceStep == 0) varianceStep = 1;
      hybridQuality += varianceStep * 0.01;
    }
    hybridQuality = Math.Clamp(hybridQuality, 0.0, maxQuality);

    // YIELD 25% chance for variance
    int hybridYield = Math.Round((seed1.GetYield() + seed2.GetYield()) / 2.0);
    if (Math.RandomFloat01() < 0.25) {
      hybridYield += Math.RandomIntInclusive(-1, 1);
    }
    hybridYield = Math.ClampInt(hybridYield, 1, maxYield);

    // GROW SPEED 25% chance for variance
    int hybridGrowSpeed = Math.Round((seed1.GetGrowSpeedInterval() + seed2.GetGrowSpeedInterval()) / 2.0);
    if (Math.RandomFloat01() < 0.25) {
      hybridGrowSpeed += Math.RandomIntInclusive(-30, 30);
    }
    hybridGrowSpeed = Math.ClampInt(hybridGrowSpeed, minGrowSpeed, 600);

    // COLOR inherits with 1 in 1000 chance for purple mutation
    string hybridColor = seed1.GetColor();
    if (Math.RandomFloat01() < 0.001) {
      hybridColor = "purple";
    } else if (Math.RandomFloat01() < 0.5) {
      hybridColor = seed2.GetColor();
    }
    
    string hybridStrainName = GenerateRandomStrainName(hybridColor, hybridQuality);

    Rpc(RpcAsk_SpawnHybridPlant, spawnPosition, groundMaterial, hybridStrainName, hybridQuality, hybridYield, hybridGrowSpeed, hybridColor, hybridIndoorOnly);
  }

  //------------------------------------------------------------------------------------------------
  [RplRpc(RplChannel.Reliable, RplRcver.Server)]
  protected void RpcAsk_SpawnHybridPlant(vector spawnPos, string groundMaterial, string strainName, float quality, int yield, int growSpeed, string color, bool indoorOnly) {
    if (!Replication.IsServer())
      return;
    Print("RpcAsk_SpawnHybridPlant");
      
    SpawnHybridPlant(spawnPos, groundMaterial, strainName, quality, yield, growSpeed, color, indoorOnly);
  }

  //------------------------------------------------------------------------------------------------
  protected bool FindBreedingSpawnPosition(vector parent1Pos, vector parent2Pos, out vector spawnPosition, out string groundMaterial) {

    for (int i = 0; i < 15; i++) {
      Print("[RL_GrowingComponent] FindBreedingSpawnPosition for");

      vector testPos;
      
      if (i < 5) {
        float ratio = Math.RandomFloatInclusive(0.2, 0.8);
        testPos = vector.Lerp(parent1Pos, parent2Pos, ratio);
        float offsetDistance = Math.RandomFloatInclusive(1.0, 3.0);
        float offsetAngle = Math.RandomFloat01() * Math.PI2;
        testPos = testPos + Vector(Math.Cos(offsetAngle) * offsetDistance, 0, Math.Sin(offsetAngle) * offsetDistance);
      } else {
        vector midpoint = (parent1Pos + parent2Pos) * 0.5;
        float angle = Math.RandomFloat01() * Math.PI2;
        float distance = Math.RandomFloatInclusive(2.0, 6.0);
        testPos = midpoint + Vector(Math.Cos(angle) * distance, 0, Math.Sin(angle) * distance);
      }

      vector adjustedGroundPos;
      if (IsValidBreedingPosition(testPos, adjustedGroundPos, groundMaterial)) {
        spawnPosition = adjustedGroundPos;
        return true;
      }
    }
    
    spawnPosition = vector.Zero;
    groundMaterial = "";
    return false;
  }

  //------------------------------------------------------------------------------------------------
  protected bool IsValidBreedingPosition(vector position, out vector groundPosition, out string groundMaterial) {
    TraceParam traceParam = new TraceParam();
    traceParam.Start = position + Vector(0, 2, 0);
    traceParam.End = position + Vector(0, -100, 0);
    traceParam.LayerMask = 0xffffffff;
    traceParam.Flags = TraceFlags.WORLD;

    float traceDistance = GetGame().GetWorld().TraceMove(traceParam, null);
  
    if (traceDistance <= 0 || traceDistance >= 102 || !traceParam.TraceMaterial || traceParam.TraceMaterial == "")
    {
      groundPosition = position;
      groundMaterial = "";
      return false;
    }
    
    groundPosition = position;
    groundMaterial = traceParam.TraceMaterial;
    m_bFoundNearbyPlantingSpot = false;
    GetGame().GetWorld().QueryEntitiesBySphere(groundPosition, 1.0, FilterPlantingSpots);
    
    if (m_bFoundNearbyPlantingSpot)
      return false;
    
    return true;
  }

  //------------------------------------------------------------------------------------------------
  protected string GenerateRandomStrainName(string color, float quality) {
    array<string> adjectives = {
      "Northern", "Sour", "Blue", "White", "Green", "Golden", "Silver", "Fire", "Ice", "Thunder",
      "Lightning", "Crystal", "Diamond", "Royal", "Cosmic", "Mystic", "Sacred", "Ancient", "Wild",
      "Alien", "Fire", "Dank", "Star", "Guava", "Pineapple", "Mango", "GMO", "Strawberry", "Pink",
      "Lemon", "Chem", "Grape", "Blueberry", "Forbidden", "Animal", "Sundae", "Obama", "Candy",
      "Motor", "Wedding", "Borat", "Gaslight", "Masons", "Red"
    };
    
    array<string> nouns = {
      "Kush", "Haze", "Dream", "Express", "Diesel", "Widow", "Skunk", "Jack", "Cheese", "Berry",
      "Cookie", "Cake", "Punch", "Glue", "Ghost", "Train", "Wreck", "Breath", "Ice", "Thunder",
      "Dawg", "Runtz", "Zkittlez", "Gelato", "Boof", "Cherries", "Oreoz", "Poison", "Cough",
      "Mints", "Piss", "OG", "Truffle", "Bomb", "Fuel", "Nerds", "Rainbow", "Soap", "Smoothie",
	    "Rip"
    };
    
    array<string> bonusKeywords = {
      "Ultra", "Super", "Mega", "Prime", "Supreme", "Elite", "Premium",
      "Exotic", "Rare", "Special", "Vintage", "Master", "Grand"
    };
    
    string purplePrefix = "";
    string bonusKeyword = "";
    
    if (color == "purple") {
      purplePrefix = "Purple ";
    }
    
    // Add bonus keyword if quality > 0.8 
    if (quality > 0.8) {
      bonusKeyword = bonusKeywords.GetRandomElement();
    }
    
    string adjective = adjectives.GetRandomElement();
    string noun = nouns.GetRandomElement();
    
    return string.Format("%1 %2 %3 %4", purplePrefix, bonusKeyword, adjective, noun);
  }

  //------------------------------------------------------------------------------------------------
  protected void SpawnHybridPlant(vector spawnPos, string groundMaterial, string strainName, float quality, int yield, int growSpeed, string color, bool indoorOnly) {

    ResourceName plantingSpotPrefab = "{A76E0AE40D5CF303}Prefabs/DrugPlanting/Weed/Planting_Spot.et";
    Resource plantingSpotResource = Resource.Load(plantingSpotPrefab);
    if (!plantingSpotResource)
      return;
    
    EntitySpawnParams spawnParams = new EntitySpawnParams();
    vector transform[4];
    Math3D.MatrixIdentity4(transform);
    transform[3] = spawnPos;
    spawnParams.Transform = transform;
    spawnParams.TransformMode = ETransformMode.WORLD;
    
    IEntity hybridSpot = GetGame().SpawnEntityPrefab(plantingSpotResource, GetGame().GetWorld(), spawnParams);
    if (!hybridSpot)
      return;

    RL_WeedSeedComponent hybridSeedComponent = RL_WeedSeedComponent.Cast(hybridSpot.FindComponent(RL_WeedSeedComponent));
    if (hybridSeedComponent) {
	    hybridSeedComponent.SetIndoorOnly(indoorOnly);
      hybridSeedComponent.SetQuality(quality);
      hybridSeedComponent.SetYield(yield);
      hybridSeedComponent.SetGrowSpeedInterval(growSpeed);
      hybridSeedComponent.SetColor(color);
      hybridSeedComponent.ChangeStrainName(strainName);
      Replication.BumpMe();
		}

    RL_GrowingComponent hybridGrowComponent = RL_GrowingComponent.Cast(hybridSpot.FindComponent(RL_GrowingComponent));
    if (hybridGrowComponent) {
      hybridGrowComponent.SetGrowthInterval(growSpeed);
      hybridGrowComponent.SetHybridSpawn(true);
      hybridGrowComponent.growthTime = 0;
      hybridGrowComponent.growthStage = 0;
      hybridGrowComponent.watered = false;
      hybridGrowComponent.UpdateGrowth(false);
      Replication.BumpMe();
		}
		    
    if (hybridSpot.GetPhysics())
      hybridSpot.GetPhysics().SetActive(true);
  }
}