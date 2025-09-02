class RL_HeroinCampfireKitGadgetComponentClass : SCR_GadgetComponentClass
{
}

class RL_HeroinCampfireKitGadgetComponent : SCR_GadgetComponent
{
	[Attribute(defvalue: "{059DAB67312FED3B}Prefabs/Heroin/HeroinFirePlace.et", desc: "Heroin campfire prefab")]
	protected ResourceName m_sHeroinCampfirePrefab;
	
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
    protected bool IsHeroinCampfireNearby(vector position, float radius)
    {
        m_bFoundNearbyHeroinCampfire = false;
        GetGame().GetWorld().QueryEntitiesBySphere(position, radius, FilterHeroinCampfires);
        
        return m_bFoundNearbyHeroinCampfire;
    }
    
    protected bool m_bFoundNearbyHeroinCampfire = false;
    
    //------------------------------------------------------------------------------------------------
    protected bool FilterHeroinCampfires(IEntity entity)
    {
        if (!entity)
            return true;
        
        if (entity == m_CharacterOwner)
            return true;
        
        EntityPrefabData prefabData = entity.GetPrefabData();
        if (prefabData)
        {
            ResourceName entityPrefab = SCR_BaseContainerTools.GetPrefabResourceName(prefabData.GetPrefab());
            if (entityPrefab == m_sHeroinCampfirePrefab)
            {
                m_bFoundNearbyHeroinCampfire = true;
                return false;
            }
        }
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    
    
    //------------------------------------------------------------------------------------------------
    override EGadgetType GetType()
    {
        return EGadgetType.HEROIN_CAMPFIRE_KIT;
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
        
        Print("Place m_CharacterOwner");
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_CharacterOwner);
        if(!character || character.IsSpamming()) return;

        vector characterPos = character.GetOrigin();
        vector characterDirection = character.GetAngles();
        vector placementDirection = Vector(Math.Sin(characterDirection[0] * Math.DEG2RAD), 0, Math.Cos(characterDirection[0] * Math.DEG2RAD));
        vector spawnPos = characterPos + (placementDirection * 1.0);
        
        if (IsHeroinCampfireNearby(spawnPos, 5.0))
        {
            RL_Utils.Notify("Another campfire is too close", "CAMPFIRE");
            return;
        }
        int campfireKitRplId = EPF_NetworkUtils.GetRplId(GetOwner());
        character.SpawnCampfire(campfireKitRplId, spawnPos, characterDirection, m_sHeroinCampfirePrefab);
    }
}
