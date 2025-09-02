// Custom gadget component that plants seeds and spawns planting spots, do not use the old RL_SeedBagComponent.
class RL_SeedBagGadgetComponentClass : SCR_GadgetComponentClass
{
}

class RL_SeedBagGadgetComponent : SCR_GadgetComponent
{
	[Attribute(defvalue: "5", desc: "Initial seed count"), RplProp()]
	protected int m_iSeedCount;
	
	[Attribute(defvalue: "{651CC1329DCACEF8}Prefabs/DrugPlanting/Weed/Planting_Spot.et", desc: "Planting spot prefab")]
	protected ResourceName m_sPlantingSpotPrefab;
	
	[Attribute(desc: "Allowed ground materials for planting")]
	protected ref array<string> m_aAllowedMaterials;
	
    //------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner) 
    {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.INIT);
    }
    
    //------------------------------------------------------------------------------------------------
    override void ActivateAction()
    {
        super.ActivateAction();
    }
    
    //------------------------------------------------------------------------------------------------
    protected void PlantSeed()
    {

        Print("RL_SeedBagGadgetComponent PlantSeed");

		
        if (!m_CharacterOwner)
            return;

        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_CharacterOwner);
        if(!character) return;
        
        if(!character || character.IsSpamming()) return;
        
        vector groundPos;
        string groundMaterial;
        if (!IsValidPlantingGround(character.GetOrigin(), groundPos, groundMaterial)) {
			RL_Utils.Notify("You cannot plant seeds on this ground", "PLANT SEEDS");
            return;
        }
        
        RL_WeedSeedComponent seedComponent = RL_WeedSeedComponent.Cast(GetOwner().FindComponent(RL_WeedSeedComponent));
        if(!seedComponent)
        {
            RL_Utils.Notify("Something jacked up with dez seeds man", "PLANT SEEDS");
            return;
        }
        float plantingDistance = 3.0;
        if (seedComponent.IsIndoorOnly()) {
            plantingDistance = 1.5;
        }
        
        if (IsPlantingSpotNearby(groundPos, plantingDistance)) {
			RL_Utils.Notify("Another plant is too close", "PLANT SEEDS");
            return;
        }
        

        int seedBagRplId = EPF_NetworkUtils.GetRplId(GetOwner());
        character.SpawnPlantingSpot(seedBagRplId, groundPos, groundMaterial, m_sPlantingSpotPrefab);
    }
    
    //------------------------------------------------------------------------------------------------
    protected bool IsValidPlantingGround(vector position, out vector groundPosition, out string groundMaterial)
    {
        TraceParam traceParam = new TraceParam();
        traceParam.Start = position + Vector(0, 2, 0);
        traceParam.End = position + Vector(0, -100, 0);
        traceParam.LayerMask = 0xffffffff;
        traceParam.Flags = TraceFlags.DEFAULT;
        traceParam.Exclude = m_CharacterOwner;

        float traceDistance = GetGame().GetWorld().TraceMove(traceParam, null);
        if (traceDistance <= 0 || traceDistance >= 102)
        {
            groundPosition = position;
            groundMaterial = "";
            return false;
        }
        
        groundPosition = position;
        if (traceParam.TraceMaterial)
            groundMaterial = traceParam.TraceMaterial;
        else
            groundMaterial = "";
        RL_WeedSeedComponent seedComponent = RL_WeedSeedComponent.Cast(GetOwner().FindComponent(RL_WeedSeedComponent));
        if (seedComponent && seedComponent.IsIndoorOnly()) {
            if (traceParam.TraceEnt) {
                EntityPrefabData prefabData = traceParam.TraceEnt.GetPrefabData();
                if (prefabData) {
                    ResourceName entityPrefab = SCR_BaseContainerTools.GetPrefabResourceName(prefabData.GetPrefab());
                    if (entityPrefab == "{F481D0D8EF127397}Prefabs/Labs/TracingFloor.et") {
                        return true;
                    }
                }
            }
            return false;
        }
        
        if (!traceParam.TraceMaterial || traceParam.TraceMaterial == "")
        {
            return false;
        }
        
        if (m_aAllowedMaterials && m_aAllowedMaterials.Count() > 0) {
            if (!m_aAllowedMaterials.Contains(groundMaterial))
                return false;
        }

        if (seedComponent) {
            array<string> suitableSoils = seedComponent.GetSuitableSoilTypes();
            if (suitableSoils && suitableSoils.Count() > 0) {
                if (!suitableSoils.Contains(groundMaterial))
                    return false;
            }
        }
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    protected bool IsPlantingSpotNearby(vector position, float radius)
    {
        m_bFoundNearbyPlantingSpot = false;
        GetGame().GetWorld().QueryEntitiesBySphere(position, radius, FilterPlantingSpots);
        
        return m_bFoundNearbyPlantingSpot;
    }
    
    protected bool m_bFoundNearbyPlantingSpot = false;
    
    //------------------------------------------------------------------------------------------------
    protected bool FilterPlantingSpots(IEntity entity)
    {
        if (!entity)
            return true;
        
        EntityPrefabData prefabData = entity.GetPrefabData();
        if (prefabData)
        {
            ResourceName entityPrefab = SCR_BaseContainerTools.GetPrefabResourceName(prefabData.GetPrefab());
            if (entityPrefab == m_sPlantingSpotPrefab)
            {
                m_bFoundNearbyPlantingSpot = true;;
                return false;
            }
        }
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    
    //------------------------------------------------------------------------------------------------
    bool UseSeed() 
    {
        if (m_iSeedCount <= 0)
            return false;

        m_iSeedCount--;
        Replication.BumpMe();
        return true;
    }

    //------------------------------------------------------------------------------------------------
    bool IsEmpty() 
    {
        return m_iSeedCount <= 0;
    }

    //------------------------------------------------------------------------------------------------
    int GetSeedCount() 
    {
        return m_iSeedCount;
    }

    //------------------------------------------------------------------------------------------------
    void SetSeedCount(int count) 
    {
        m_iSeedCount = count;
        Replication.BumpMe();
    }
    
    //------------------------------------------------------------------------------------------------
    override EGadgetType GetType()
    {
        return EGadgetType.SEED_BAG;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBeHeld()
    {
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool IsVisibleEquipped()
    {
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override void OnToggleActive(bool state)
    {
        super.OnToggleActive(state);

        if (!state) return;
        if(RL_Utils.IsDedicatedServer()) return;
        if (!m_CharacterOwner)
            return;
            
        RplComponent rplComponent = RplComponent.Cast(m_CharacterOwner.FindComponent(RplComponent));
        if (!rplComponent || !rplComponent.IsOwner())
            return;

        PlantSeed();
    }
} 