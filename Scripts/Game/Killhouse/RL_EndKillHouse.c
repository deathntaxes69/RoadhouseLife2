[BaseContainerProps]
class RL_EndKillHouse : ScriptedUserAction
{
    protected RL_KillhouseManagerComponent m_Manager;

    //----------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
 		IEntity parent = pOwnerEntity.GetParent();
		if (!parent)
			return;

		m_Manager = RL_KillhouseManagerComponent.Cast(parent.FindComponent(RL_KillhouseManagerComponent));
    }

    //----------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        return m_Manager && m_Manager.IsRunning();
    }

    //----------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        return m_Manager && m_Manager.IsRunning();
    }

    //----------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        SCR_ChimeraCharacter ender = SCR_ChimeraCharacter.Cast(pUserEntity);
        if (!ender || !m_Manager)
            return;

        m_Manager.EndRun(ender);
    }

    //----------------------------------------------------------------------------------------------
    override bool HasLocalEffectOnlyScript()
    {
        return true;
    }
}


