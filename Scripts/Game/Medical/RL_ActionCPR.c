[BaseContainerProps()]
class RL_CprAction : RL_ProgressBarAction {
	
    protected float m_fCivReviveChance  = 0.05;
	protected float m_fEmsReviveChance = 0.90;
    protected float m_fHealthOnRevive = 1;
    protected float m_fBloodOnRevive = 1;

	protected int m_iEmsReiviveCost = 500;
	protected int m_iEmsReiviveCostMinBank = 2000;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) {
		if (!Replication.IsServer())
			return;

		if (!pOwnerEntity || !pUserEntity)
			return;

		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterControllerComponent));
		if (!controller)
			return;

		if (controller.GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return;
	
		float reviveChance;
		
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (userCharacter && userCharacter.IsMedic())
			reviveChance = m_fEmsReviveChance;
		else
		{
			reviveChance = m_fCivReviveChance;
			if (RL_Utils.GetEMSOnlineCount() == 0)
				reviveChance = 0.1;
		}
/*
		if (RL_Utils.IsDevServer())
		{
			userCharacter.Notify(string.Format("REVIVE CHANCE OVERRIDE, ORGINAL CHANCE %1", reviveChance));
			reviveChance = 1.00;
		}
*/
		Print(string.Format("------- REVIVE CHANCE %1", reviveChance));
		float roll = Math.RandomFloat01();
		if (roll > reviveChance)
		{
			userCharacter.Notify("Failed to revive, try again", "CPR");
			return;
		}
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageMgr)
			return;
		
		//This will also reset CPR props
		RL_MedicalUtils.Heal(damageMgr, m_fHealthOnRevive, m_fBloodOnRevive, true, false, userCharacter.IsMedic());


		RL_MetabolismComponent m_Metabolisum = RL_MetabolismComponent.Cast(pOwnerEntity.FindComponent(RL_MetabolismComponent));

		if (m_Metabolisum.GetFoodLevel() < 0.1)
			m_Metabolisum.SetFoodLevel(0.1);

		if (m_Metabolisum.GetThirstLevel() < 0.1)
			m_Metabolisum.SetThirstLevel(0.1);

		if(damageMgr.GetDeathId() != "")
		{
			SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
			jsonData.WriteValue("death_id", damageMgr.GetDeathId());
			jsonData.WriteValue("revive_cid", userCharacter.GetCharacterId());
			jsonData.WriteValue("revive_name", userCharacter.GetCharacterName());
			userCharacter.PlayerLog("Revive", jsonData.ExportToString());
			damageMgr.ResetDeathId();
		}

		SCR_ChimeraCharacter ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if(userCharacter.IsMedic() && ownerCharacter.GetBank() >= m_iEmsReiviveCostMinBank)
		{
			ownerCharacter.RpcAsk_TransactMoney(ownerCharacter.GetCharacterId(), 0, -m_iEmsReiviveCost);
			userCharacter.RpcAsk_TransactMoney(userCharacter.GetCharacterId(), 0, m_iEmsReiviveCost);
			ownerCharacter.Notify(string.Format("You have been charged $%1 for a medical revive", m_iEmsReiviveCost), "CPR");
			userCharacter.Notify(string.Format("You have received $%1 for reviving", m_iEmsReiviveCost), "CPR");
		} else {

		}

	}

	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}

	override bool CanBePerformedScript(IEntity user) {
		SCR_CharacterDamageManagerComponent dmc = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
		SCR_CharacterControllerComponent cc = SCR_CharacterControllerComponent.Cast(GetOwner().FindComponent(SCR_CharacterControllerComponent));

		return dmc && dmc.IsWaitingForCpr() && cc && cc.GetLifeState() != ECharacterLifeState.DEAD;
	}

	override bool GetActionNameScript(out string outName) {
		outName = "Perform CPR";
		return true;
	}

}
