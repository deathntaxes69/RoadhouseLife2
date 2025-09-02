[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class RL_FishingZoneStateComponentClass : ScriptComponentClass {}

class RL_FishingZoneStateComponent : ScriptComponent
{
    SCR_CharacterBuoyancyComponent m_BuyancyComponent;
    protected bool m_IsInZone;
    protected bool m_IsInRestZone;
    protected bool m_IsInRareZone; 

    //------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
        SetEventMask(owner, EntityEvent.INIT);
    }

    //------------------------------------------------------------------------------------------------
    override void EOnInit(IEntity owner)
    {
        m_BuyancyComponent = SCR_CharacterBuoyancyComponent.Cast(owner.FindComponent(SCR_CharacterBuoyancyComponent));
        if (!m_BuyancyComponent)
            return;

        m_BuyancyComponent.GetOnWaterEnter().Insert(OnWaterEnter);
        m_BuyancyComponent.GetOnWaterExit().Insert(OnWaterExit);
    }

    //------------------------------------------------------------------------------------------------
    void OnWaterEnter()
    {
        SetIsInZone(true);
    }

    //------------------------------------------------------------------------------------------------
    void OnWaterExit()
    {
        SetIsInZone(false);
    }

    //------------------------------------------------------------------------------------------------
    void SetIsInZone(bool IsInZone)
    {
        m_IsInZone = IsInZone;
    }

    //------------------------------------------------------------------------------------------------
    bool GetIsInZone()
    {
        return m_IsInZone;
    }

    //------------------------------------------------------------------------------------------------
    void SetIsInRestZone(bool state)
    {
        m_IsInRestZone = state;
    }

    bool IsInRestZone()
    {
        return m_IsInRestZone;
    }

    //------------------------------------------------------------------------------------------------
    void SetIsInRareZone(bool state)
    {
        m_IsInRareZone = state;
    }

    bool IsInRareZone()
    {
        return m_IsInRareZone;
    }
}
