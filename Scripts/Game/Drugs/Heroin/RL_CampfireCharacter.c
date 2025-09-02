modded class SCR_ChimeraCharacter
{
    void SpawnCampfire(int gadgetRplId, vector spawnPos, vector spawnAngles, string prefabToPlace)
    {
        Rpc(RpcAsk_SpawnCampfire, gadgetRplId, spawnPos, spawnAngles, prefabToPlace);
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SpawnCampfire(int gadgetRplId, vector spawnPos, vector spawnAngles, string prefabToPlace)
    {
        Print("RpcAsk_SpawnCampfire");

        if (!Replication.IsServer() || !this)
            return;

        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
        if(!character) return;

        RL_EntityLocationManager npcManager = RL_EntityLocationManager.GetInstance();
        if (npcManager)
        {
            float distanceToDealer = npcManager.GetDistanceToEntityType(character.GetOrigin(), ELocationEntityType.DEALER);
            float distanceToProcessor = npcManager.GetDistanceToEntityType(character.GetOrigin(), ELocationEntityType.HEROIN_PROCESSOR);
            if (distanceToDealer < 500 || distanceToProcessor < 500)
            {
        		character.Notify("You cannot cook heroin within 500m of the processor, within 500m of a dealer.", "CAMPFIRE");
                return;
            }
        }
        IEntity campfireKitEntity = EPF_NetworkUtils.FindEntityByRplId(gadgetRplId);
        if (!campfireKitEntity) return;
        
        InventoryStorageManagerComponent invManager = RL_InventoryUtils.GetResponsibleStorageManager(campfireKitEntity);
        if(invManager)
        {
            invManager.TryDeleteItem(campfireKitEntity);
        } else {
            SCR_EntityHelper.DeleteEntityAndChildren(campfireKitEntity);
        }
        

        Resource campfireResource = Resource.Load(prefabToPlace);
        if (!campfireResource)
            return;
        
        EntitySpawnParams spawnParams = new EntitySpawnParams();
        vector transform[4];
        Math3D.MatrixIdentity4(transform);
        transform[3] = spawnPos;
        spawnParams.Transform = transform;
        spawnParams.TransformMode = ETransformMode.WORLD;
        
        IEntity campfire = GetGame().SpawnEntityPrefab(campfireResource, GetGame().GetWorld(), spawnParams);
        if (!campfire)
            return;
        
        if (campfire.GetPhysics())
            campfire.GetPhysics().SetActive(true);
        

        character.Notify("Campfire placed", "CAMPFIRE");
        
        
    }
}