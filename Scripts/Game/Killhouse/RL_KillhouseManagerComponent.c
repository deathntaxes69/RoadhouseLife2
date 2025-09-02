class RL_KillhouseManagerComponentClass : ScriptComponentClass {}

class RL_KillhouseManagerComponent : ScriptComponent
{
    protected bool m_bIsRunning = false;
    protected int m_iStartTick = 0;
    protected string m_sStarterCharacterId = "";
    protected ref array<RL_KillhouseTarget> m_aTargets = {};
    protected int m_iTotalTargets = 0;
    protected int m_iTotalHostages = 0;
    protected int m_iTargetsHit = 0;
    protected int m_iHostagesKilled = 0;
    protected int m_iHeadshots = 0;
    
	[Attribute(defvalue: "", desc: "Killhouse name", category: "Killhouse")]
	protected string m_sName;

    //------------------------------------------------------------------------------------------------
    bool IsRunning()
    {
        return m_bIsRunning;
    }

    //------------------------------------------------------------------------------------------------
    void RegisterTarget(RL_KillhouseTarget target)
    {
        if (!target)
            return;

        if (!m_aTargets)
            m_aTargets = new array<RL_KillhouseTarget>();

        if (m_aTargets.Find(target) == -1)
            m_aTargets.Insert(target);

        if (target.IsHostage())
            m_iTotalHostages++;
        else
            m_iTotalTargets++;
    }

    //------------------------------------------------------------------------------------------------
    void UnregisterTarget(RL_KillhouseTarget target)
    {
        if (!m_aTargets)
            return;

        int idx = m_aTargets.Find(target);
        if (idx > -1)
            m_aTargets.Remove(idx);
    }

    //------------------------------------------------------------------------------------------------
    void OnTargetHit(RL_KillhouseTarget target)
    {
        if (!m_bIsRunning || !target)
            return;

        if (target.IsHostage())
            m_iHostagesKilled++;
        else
            m_iTargetsHit++;
    }

    //------------------------------------------------------------------------------------------------
    void OnHeadshot(RL_KillhouseTarget target)
    {
        if (!m_bIsRunning || !target)
            return;

        if (!target.IsHostage())
            m_iHeadshots++;
    }

    //------------------------------------------------------------------------------------------------
    void StartRun(SCR_ChimeraCharacter starter)
    {
        if (!starter)
            return;

        if (m_bIsRunning)
            return;

        m_bIsRunning = true;
        m_iTargetsHit = 0;
        m_iHostagesKilled = 0;
        m_iHeadshots = 0;
        m_iStartTick = System.GetTickCount();
        m_sStarterCharacterId = starter.GetCharacterId();

        foreach (RL_KillhouseTarget target : m_aTargets)
            if (target) target.ResetLocalCounter();

        //base class already has replication
        if (!Replication.IsServer())
        {
            ResetAllTargets();
        }
    }

    //------------------------------------------------------------------------------------------------
    void EndRun(SCR_ChimeraCharacter ender)
    {
        if (!m_bIsRunning)
            return;

        if (!ender || ender.GetCharacterId() != m_sStarterCharacterId)
        {
            
            m_bIsRunning = false;
            m_sStarterCharacterId = "";
            return;
        }

        int elapsedMs = Math.Max(0, System.GetTickCount() - m_iStartTick);
        float elapsedSec = elapsedMs / 1000.0;

        string summary = string.Format(
            "Time: %1 s\nTargets: %2/%3\nHostages: %4/%5\nHeadshots: %6/%3",
            elapsedSec.ToString(false, 3),
            m_iTargetsHit,
            m_iTotalTargets,
            m_iHostagesKilled,
            m_iTotalHostages,
            m_iHeadshots
        );

        ender.Notify(summary, "KILLHOUSE RESULT", 10);
		ender.LogKillhouseRun(m_sName, elapsedSec, m_iTargetsHit, m_iTotalTargets, m_iHostagesKilled, m_iTotalHostages, m_iHeadshots);
        m_bIsRunning = false;
        m_sStarterCharacterId = "";
    }

    //-----------------------------------------------------------------------------------------------
    protected void ResetAllTargets()
    {
        if (!m_aTargets)
            return;
            
        foreach (RL_KillhouseTarget target : m_aTargets)
        {
            if (!target)
                continue;

			target.ResetForKillhouse();
            target.SetState(ETargetState.TARGET_UP);
            target.ResetForKillhouse();
        }
    }
}


