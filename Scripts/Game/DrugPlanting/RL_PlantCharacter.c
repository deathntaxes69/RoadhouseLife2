modded class SCR_ChimeraCharacter
{
    void SpawnPlantingSpot(int seedBagRplId, vector spawnPos, string groundMaterial, string plantingSpotPrefab)
    {
        Rpc(RpcAsk_SpawnPlantingSpot, seedBagRplId, spawnPos, groundMaterial, plantingSpotPrefab);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_SpawnPlantingSpot(int seedBagRplId, vector spawnPos, string groundMaterial, string plantingSpotPrefab)
    {
        Print("RpcAsk_SpawnPlantingSpot");
        if (!Replication.IsServer())
            return;

        
        IEntity seedBagEntity = EPF_NetworkUtils.FindEntityByRplId(seedBagRplId);
        if (!seedBagEntity) return;

        RL_WeedSeedComponent seedComponent = RL_WeedSeedComponent.Cast(seedBagEntity.FindComponent(RL_WeedSeedComponent));
        if(!seedComponent) return;

        RL_SeedBagGadgetComponent seedGadget = RL_SeedBagGadgetComponent.Cast(seedBagEntity.FindComponent(RL_SeedBagGadgetComponent));
        if(!seedGadget) return;


        string strainName = "Unknown Strain";
        float quality = 0.2;
        int yield = 1;
        int growSpeedInterval = 420;
        bool indoorOnly = false;
        string color = "green";

        strainName = seedComponent.GetStrainName();
        quality = seedComponent.GetQuality();
        yield = seedComponent.GetYield();
        growSpeedInterval = seedComponent.GetGrowSpeedInterval();
        indoorOnly = seedComponent.IsIndoorOnly();
        color = seedComponent.GetColor();
        
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
        if(!character) return;

        RL_EntityLocationManager npcManager = RL_EntityLocationManager.GetInstance();
        if (npcManager)
        {
            float distanceToDealer = npcManager.GetDistanceToEntityType(GetOrigin(), ELocationEntityType.DEALER);
            float distanceToWeedProcessor = npcManager.GetDistanceToEntityType(GetOrigin(), ELocationEntityType.WEED_PROCESSOR);
            if (distanceToDealer < 500 || distanceToWeedProcessor < 2000)
            {
        		character.Notify("You cannot plant seeds within 2000m of a processor, within 500m of a dealer.", "PLANT SEEDS");
                return;
            }
        }
        
        Resource plantingSpotResource = Resource.Load(plantingSpotPrefab);
        if (!plantingSpotResource)
            return;
        
        EntitySpawnParams spawnParams = new EntitySpawnParams();
        vector transform[4];
        Math3D.MatrixIdentity4(transform);
        transform[3] = spawnPos;
        spawnParams.Transform = transform;
        spawnParams.TransformMode = ETransformMode.WORLD;
        
        IEntity plantingSpot = GetGame().SpawnEntityPrefab(plantingSpotResource, GetGame().GetWorld(), spawnParams);
        if (!plantingSpot)
            return;

        RL_WeedSeedComponent plantedSeedComponent = RL_WeedSeedComponent.Cast(plantingSpot.FindComponent(RL_WeedSeedComponent));
        if (plantedSeedComponent) {
			plantedSeedComponent.SetIndoorOnly(indoorOnly);
            plantedSeedComponent.SetQuality(quality);
            plantedSeedComponent.SetYield(yield);
            plantedSeedComponent.SetGrowSpeedInterval(growSpeedInterval);
            plantedSeedComponent.SetColor(color);
            plantedSeedComponent.ChangeStrainName(strainName);
        }

        RL_GrowingComponent growComponent = RL_GrowingComponent.Cast(plantingSpot.FindComponent(RL_GrowingComponent));
        if (growComponent) {
            growComponent.SetGrowthInterval(growSpeedInterval);
        }
        
        if (plantingSpot.GetPhysics())
            plantingSpot.GetPhysics().SetActive(true);
        
        seedGadget.UseSeed();
        if (seedGadget.IsEmpty()) {
            InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(seedBagEntity);
            if(invManager)
            {
                invManager.TryDeleteItem(seedBagEntity);
            } else {
                SCR_EntityHelper.DeleteEntityAndChildren(seedBagEntity);
            }
        }
    }
    

}