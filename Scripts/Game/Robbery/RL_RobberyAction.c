class RL_RobberyAction : ScriptedUserAction
{
    
	[Attribute(defvalue:"Gas Station", desc: "", category: "General")]
	protected string m_sRobberyName;

	[Attribute(defvalue:"600000", desc: "Amount of time it takes to rob", category: "General")]
	protected float m_fRobberyTime;

    [Attribute(defvalue:"10000", desc: "RobberyBaseReward", category: "General")]
	protected int m_iRobberyBaseReward;

	protected float m_fMaxDistance = 23;

	protected float m_fRobberyStartTime = 0;

	protected Widget m_wProgressBar;

    protected RL_RobberyManagerComponent m_robberyManagerComp;
    protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_userEntity;


    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
        m_robberyManagerComp = EL_Component<RL_RobberyManagerComponent>.Find(pOwnerEntity);
	}
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("Debug 1");
		m_ownerEntity = pOwnerEntity;
		m_userEntity = SCR_ChimeraCharacter.Cast(pUserEntity);
        if(!m_ownerEntity || !m_userEntity)
			return;
		
		// Return if canceling action
		if(m_fRobberyStartTime != 0)
		{
			CancelRobbery();
			return;
		}
		// Reset everything to be safe
		CancelRobbery();
		Print("Debug 3");

		// Start action
        m_userEntity.SetRobberyState(EPF_NetworkUtils.GetRplId(m_ownerEntity), true);
		m_fRobberyStartTime = m_ownerEntity.GetWorld().GetTimestamp().DiffMilliseconds(null);

		if(!RL_Utils.WithinDistance(m_ownerEntity, m_userEntity, m_fMaxDistance))
		{
			RL_Utils.Notify("Canceled, you walked too far away.", "GATHER");
			return;
		}
		// Create Progress Bar
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if(!hud) return;
		delete m_wProgressBar;
		m_wProgressBar = hud.CreateActionProgressBar(string.Format("Robbing %1", m_sRobberyName), m_fRobberyTime);
		GetGame().GetCallqueue().CallLater(FinishRobbery, m_fRobberyTime);
        GetGame().GetCallqueue().CallLater(RobberyCheck, 2000, true);
	}
    void RobberyCheck()
    {
		if(!m_ownerEntity || !m_userEntity)
		{
			RL_Utils.Notify("Canceled, error.", "ROBBERY");
            CancelRobbery();
			return;
		}
        if(!RL_Utils.WithinDistance(m_ownerEntity, m_userEntity, m_fMaxDistance))
		{
			RL_Utils.Notify("Canceled, you walked too far away.", "ROBBERY");
            CancelRobbery();
			return;
		}
		if(m_userEntity.IsHandcuffed())
		{
			RL_Utils.Notify("Canceled, you were arrested.", "ROBBERY");
            CancelRobbery();
			return;
		}
		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(m_userEntity);
        if (!characterController || !characterController.GetWeaponManagerComponent().GetCurrentWeapon())
        {
			RL_Utils.Notify("Canceled, requires weapon in hands.", "ROBBERY");
			CancelRobbery();
            return;
        }


    }
	void FinishRobbery()
	{
        m_userEntity.TransactMoney( m_iRobberyBaseReward );
		RL_Utils.Notify(string.Format("You stole %1", RL_Utils.FormatMoney(m_iRobberyBaseReward)), "ROBBERY");
        CancelRobbery();
	}
	void CancelRobbery()
	{
		m_userEntity.SetRobberyState(EPF_NetworkUtils.GetRplId(m_ownerEntity), false);
		m_fRobberyStartTime = 0;
		delete m_wProgressBar;
        GetGame().GetCallqueue().Remove(RobberyCheck);
		GetGame().GetCallqueue().Remove(FinishRobbery);
	}
	override bool GetActionNameScript(out string outName)
	{
		if(m_fRobberyStartTime != 0)
			outName = "Cancel Robbery";
		else
			outName = "Start Robbery";

		return true;
	}
	override bool CanBePerformedScript(IEntity user)
 	{
        //Always allow cancel
        if(m_fRobberyStartTime != 0)
        {
            return true;
        }
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);

		if (!character || character.IsPolice())
		{
			SetCannotPerformReason("You cannot rob on duty");
			return false;
		}
		
		if(RL_Utils.GetPoliceOnlineCount() <= 1)
		{
			SetCannotPerformReason("Not enough officers");
			return false;
		}
		
        // Check if robbery active or in break
		if (!m_robberyManagerComp || !m_robberyManagerComp.CanRobNow())
		{
			SetCannotPerformReason("Robbery in cooldown");
			return false;
		}

		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(user);
        //Check if weaspon in hands
        if (!characterController || !characterController.GetWeaponManagerComponent().GetCurrentWeapon())
        {
            SetCannotPerformReason("Requires weapon in hands");
            return false;
        }

		return true;
 	}
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
}
