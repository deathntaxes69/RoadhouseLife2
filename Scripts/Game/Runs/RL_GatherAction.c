class RL_GatherAction : ScriptedUserAction
{
	[Attribute(desc: "Prefab what item is gathered", category: "General")]
	protected ResourceName m_GatherItemPrefab;

	[Attribute(defvalue:"1", desc: "Amount of items to receive", category: "General")]
	protected int m_GatherAmount;

	[Attribute(defvalue:"{354D8B8AC6135035}Prefabs/Supplies/Pickaxe.et", desc: "Item required for gathering", category: "General")]
	protected ResourceName m_GatherToolRequirement;

	[Attribute(defvalue:"3", desc: "Max distance they can move after starting", category: "General")]
	protected int m_maxMoveDistance;

	[Attribute(defvalue: "1", category: "Skill", desc: "Skill Category", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RL_SkillsCategory))]
	RL_SkillsCategory m_eSkillCategory;

	[Attribute(defvalue: "1", category: "Skill", desc: "XP per item gathered", uiwidget: UIWidgets.Auto)]
	int m_iXpPerItem;

	protected string m_sDisplayName;
	protected string m_sGatherToolDisplayName;

	protected int m_iGatherLoopInterval = 5000;
	protected int m_iGatherMaxLoopTime = 50000;
	protected float m_fGatherLoopStartTime = 0;
	protected vector m_vStartPosition;

	protected Widget m_wProgressBar;

	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_userEntity;
	protected RL_Hud m_userHud;

	//------------------------------------------------------------------------------------------------
	int GetModifiedGatherTime(int baseGatherTime)
	{
		if (!m_userEntity)
			return baseGatherTime;
		
		if (m_GatherToolRequirement != "{354D8B8AC6135035}Prefabs/Supplies/Pickaxe.et")
			return baseGatherTime;
		
		float timeMultiplier = 1.0;

		if (m_userEntity.HasPerk("mining_efficiency_ii"))
		{
			timeMultiplier = 0.8; // 20% reduction
		}
		else if (m_userEntity.HasPerk("mining_efficiency"))
		{
			timeMultiplier = 0.9; // 10% reduction
		}
		
		return Math.Round(baseGatherTime * timeMultiplier);
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_ownerEntity = pOwnerEntity;
		m_userEntity = SCR_ChimeraCharacter.Cast(pUserEntity);
		m_userHud = RL_Hud.GetCurrentInstance();

		// Return if canceling action
		if(m_fGatherLoopStartTime)
		{
			CancelGatherLoop();
			return;
		}

		// Reset everything to be safe
		CancelGatherLoop();

		// Start action loop
		m_fGatherLoopStartTime = pOwnerEntity.GetWorld().GetTimestamp().DiffMilliseconds(null);
		m_vStartPosition = m_userEntity.GetOrigin();
		Print("START POS");
		Print(m_vStartPosition);
		StartGather();
	}
	void StartGather()
	{

		if(!GatherChecks()) return;


		int modifiedGatherTime = GetModifiedGatherTime(m_iGatherLoopInterval);
		//delete m_wProgressBar;
		m_wProgressBar = m_userHud.CreateActionProgressBar(string.Format("Gathering %1", m_sDisplayName), modifiedGatherTime);
		if(!m_wProgressBar)
		{
			CancelGatherLoop();
			return;
		}
		
		GetGame().GetCallqueue().Remove(GatherChecks);
		// Only run periodic checks if interval is > 2 seconds
		if(modifiedGatherTime > 2000)
			GetGame().GetCallqueue().CallLater(GatherChecks, 1000, true);

		GetGame().GetCallqueue().CallLater(FinishGather, modifiedGatherTime+100);
	}
	bool GatherChecks()
	{
		if(!m_ownerEntity || !m_userEntity)
		{
			CancelGatherLoop();
			return false;
		}
		float currentTime = m_ownerEntity.GetWorld().GetTimestamp().DiffMilliseconds(null);
		if((m_fGatherLoopStartTime - currentTime) > 0)
		{
			CancelGatherLoop();
			return false;
		}
		float distance = vector.Distance(m_vStartPosition, m_userEntity.GetOrigin());
		if(distance > m_maxMoveDistance || !RL_Utils.CanPerformAction(m_userEntity))
		{
			CancelGatherLoop();
			RL_Utils.Notify("Canceled", "GATHER");
			return false;
		}
		if(!m_userHud)
		{
			CancelGatherLoop();
			return false;
		}

		return true;
	}
	void FinishGather()
	{
		if (!m_userEntity)
            return;
		
		CharacterControllerComponent controller = EL_Component<CharacterControllerComponent>.Find(m_userEntity);
        if (controller)
        	controller.TryPlayItemGesture(EItemGesture.EItemGesturePickUp, null, "Character_ActionGrab");

		m_userEntity.AddAmount(m_GatherItemPrefab, m_GatherAmount, this, "AddAmountCallback");
	}
	void AddAmountCallback(int gatheredAmount)
	{
		if (gatheredAmount > 0)
		{
			RL_Utils.Notify(string.Format("Gathered %1 x %2", m_GatherAmount, m_sDisplayName), "GATHER");
			SCR_InventoryStorageManagerComponent inventoryManager = EL_Component<SCR_InventoryStorageManagerComponent>.Find(m_userEntity);
			inventoryManager.RpcAsk_PlaySound(EPF_NetworkUtils.GetRplId(m_userEntity), "SOUND_PICK_UP");

			if (m_iXpPerItem > 0)
			{
				int totalXp = m_iXpPerItem * gatheredAmount;
				m_userEntity.AddXp(m_eSkillCategory, totalXp);
			}
		}
		if(gatheredAmount < m_GatherAmount)
		{
			RL_Utils.Notify("Your inventory is full", "GATHER");
			CancelGatherLoop();
			return;
		}

		StartGather();
	}
	private void PlayGrabAnimation()
    {
        if (!m_userEntity)
            return;
        CharacterControllerComponent controller = CharacterControllerComponent.Cast(m_userEntity.FindComponent(CharacterControllerComponent));
        if (!controller)
            return;
        controller.TryPlayItemGesture(
            EItemGesture.EItemGesturePickUp,
            null,
            "Character_ActionGrab"
        );
    }
	void CancelGatherLoop()
	{
		m_fGatherLoopStartTime = 0;
		if(m_userHud && m_wProgressBar)
			m_userHud.OnProgressBarComplete(m_wProgressBar);

		GetGame().GetCallqueue().Remove(GatherChecks);
		GetGame().GetCallqueue().Remove(FinishGather);
	}
	override bool GetActionNameScript(out string outName)
	{
		if (!m_sDisplayName)
		{
			m_sDisplayName = "Unknown";
			UIInfo uiInfo = RL_ShopUtils.GetItemDisplayInfo(m_GatherItemPrefab);
			if (uiInfo)
				m_sDisplayName = string.Format("%1", uiInfo.GetName());
		}

		if(m_fGatherLoopStartTime != 0)
			outName = "Cancel Gathering";
		else
			outName = string.Format("Gather %1", m_sDisplayName);

		return true;
	}
	override bool CanBePerformedScript(IEntity user)
 	{

		// If not required we dont need to check anything
		if (!m_GatherToolRequirement) return true;
		
		if (!m_sGatherToolDisplayName)
			m_sGatherToolDisplayName = RL_ShopUtils.GetItemDisplayInfo(m_GatherToolRequirement).GetName();
		
		SetCannotPerformReason(string.Format("Requires %1 in hands", m_sGatherToolDisplayName));

		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(user);
		if (characterController)
		{
			IEntity rightHandItem = characterController.GetRightHandItem();
			if (EL_Utils.GetPrefabName(rightHandItem) == m_GatherToolRequirement) return true;

			IEntity leftHandItem = characterController.GetAttachedGadgetAtLeftHandSlot();
			if (EL_Utils.GetPrefabName(leftHandItem) == m_GatherToolRequirement) return true;
		}

		return false;
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
