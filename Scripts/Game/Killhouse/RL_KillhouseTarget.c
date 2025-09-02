[EntityEditorProps(category: "GameScripted/FiringRange", description: "Target")]
class RL_KillhouseTargetClass : SCR_FiringRangeTargetClass
{
	
};

class RL_KillhouseTarget : SCR_FiringRangeTarget
{
    [Attribute(defvalue: "0", desc: "Is hostage", category: "Killhouse")]
    protected bool m_bIsHostage;

    protected RL_KillhouseManagerComponent m_Manager;
    protected bool m_bReportedHit = false;
    protected bool m_bReportedHeadshot = false;

    //----------------------------------------------------------------------------------------------
    bool IsHostage()
    {
        return m_bIsHostage;
    }

    //----------------------------------------------------------------------------------------------
    void ResetLocalCounter()
    {
        m_bReportedHit = false;
        m_bReportedHeadshot = false;
    }

    //----------------------------------------------------------------------------------------------
    void ResetForKillhouse()
    {
        ResetLocalCounter();
        m_bTargetHit = false;
        m_bWaitInPosition = false;
        m_fFallenTargetTimer = 0;
        ClearEventMask(EntityEvent.FRAME);
    }

    //----------------------------------------------------------------------------------------------
    override void EOnInit(IEntity owner)
    {
        super.EOnInit(owner);

		IEntity parent = owner.GetParent();
		if (!parent)
			return;

		m_Manager = RL_KillhouseManagerComponent.Cast(parent.FindComponent(RL_KillhouseManagerComponent));
        if (m_Manager)
            m_Manager.RegisterTarget(this);
    }

    //----------------------------------------------------------------------------------------------
	void ~RL_KillhouseTarget()
	{
		if (m_Manager)
			m_Manager.UnregisterTarget(this);
	}

	//----------------------------------------------------------------------------------------------
	override void OnDamage(float damage,
			  EDamageType type,
			  IEntity pHitEntity,
			  inout vector outMat[3],
			  IEntity damageSource,
			  notnull Instigator instigator,
			  int colliderID,
			  float speed)
	{

		if (IsProxy())
			return;

		vector localHitPos = CoordToLocal(outMat[0]);
		if (!IsHitValid(localHitPos))
			return;

        bool wasTargetUp = GetState() == ETargetState.TARGET_UP;
		super.OnDamage(damage, type, pHitEntity, outMat, damageSource, instigator, colliderID, speed);

		if (!m_Manager)
			return;

		if (!m_bReportedHit && m_Manager.IsRunning() && wasTargetUp && GetState() == ETargetState.TARGET_DOWN)
		{
			m_bReportedHit = true;
			m_Manager.OnTargetHit(this);
		}

        if (!m_bReportedHeadshot && m_Manager.IsRunning())
        {
            if (IsHeadshot(localHitPos))
            {
                m_bReportedHeadshot = true;
                m_Manager.OnHeadshot(this);
            }
        }
	}

    //----------------------------------------------------------------------------------------------
    protected bool IsHeadshot(vector localHitPos)
    {
        float x = localHitPos[0];
        float y = localHitPos[1];
        bool inHeadshotX = (x >= -0.066 && x <= 0.033);
        bool inHeadshotY = (y >= 1.52 && y <= 1.67);
        return inHeadshotX && inHeadshotY;
    }

    //----------------------------------------------------------------------------------------------
    override void GetHitValue(vector coordOfHit, int playerID)
    {
        super.GetHitValue(coordOfHit, playerID);
    }
}