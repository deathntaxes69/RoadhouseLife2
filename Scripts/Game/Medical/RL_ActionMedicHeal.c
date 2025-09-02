[BaseContainerProps()]
class RL_MedicHeal : RL_ProgressBarAction {
	

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) {
		if (!Replication.IsServer())
			return;

		if (!pOwnerEntity || !pUserEntity)
			return;

		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageMgr)
			return;
		
		damageMgr.SetMedicallyHealed(true);

	}

	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}

	override bool CanBePerformedScript(IEntity user) {
        SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
        if (!userCharacter || !userCharacter.IsMedic())
            return false;
		SCR_CharacterDamageManagerComponent dmc = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));

		return dmc && !dmc.IsMedicallyHealed();
	}

	override bool GetActionNameScript(out string outName) {
		outName = "Medic Heal";
		return true;
	}

}
