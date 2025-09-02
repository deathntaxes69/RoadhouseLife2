class RL_MedicalUtils
{
    static void Heal(SCR_CharacterDamageManagerComponent damageMgr, float newHealth = 1, float newBlood = 1, bool revive = false, bool localonly = false, bool medicRevive = false)
    {
		if (!damageMgr)
			return;

		damageMgr.RemoveAllBleedings();

		HitZone healthZone = damageMgr.GetHitZoneByName("Health");
		if (healthZone)
			healthZone.SetHealth(newHealth);

		SCR_CharacterBloodHitZone bloodZone = SCR_CharacterBloodHitZone.Cast(damageMgr.GetHitZoneByName("Blood"));
		if (bloodZone)
			bloodZone.SetHealth(newBlood);

		// Heal remaining damage over time
		array<EDamageType> damageTypes = {};
		SCR_Enum.GetEnumValues(EDamageType, damageTypes);

		foreach (EDamageType type : damageTypes)
		{
			//Print("[RL_MedicalUtils] Heal foreach loop");

			if (damageMgr.IsDamagedOverTime(type))
				damageMgr.RemoveDamageOverTime(type);
		}
        array<HitZone> hitZones = {};
		damageMgr.GetAllHitZonesInHierarchy(hitZones);

		// Fix hitzones
		ECharacterHitZoneGroup group;
		foreach (HitZone hitZone : hitZones)
		{
			//Print("[RL_MedicalUtils] Heal foreach loop 2");
			//group = hitZone.GetHitZoneGroup();
			//if (group == ECharacterHitZoneGroup.LEFTLEG || group == ECharacterHitZoneGroup.RIGHTLEG)
			//	continue;
			if (hitZone && hitZone.GetDamageState() != EDamageState.UNDAMAGED && hitZone.GetHealthScaled() < newHealth)
				hitZone.SetHealthScaled(newHealth);
		}
		if(!revive)
			return;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(damageMgr.GetOwner());
		if(!character) return;

		if(Replication.IsServer() && !localonly)
		{
			Print("Medic Utils Heal Server");
			damageMgr.RpcAsk_ResetCprProps(false, medicRevive);
			character.AskOwnerToHeal(newHealth, newBlood, revive, true);

		}
		ForceConsciousness(character, damageMgr);

    }
	static void ForceConsciousness(SCR_ChimeraCharacter character, SCR_CharacterDamageManagerComponent damageMgr) 
	{
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller) return;

		if (controller.GetLifeState() == ECharacterLifeState.ALIVE) return;

		SCR_CharacterResilienceHitZone resilienceHZ = damageMgr.GetResilienceHitZone();
		if (resilienceHZ) resilienceHZ.SetHealthScaled(1.0);

		controller.SetUnconscious(false);
		//B_ResetBleedoutTimer();
		damageMgr.UpdateConsciousness();
	}
}
