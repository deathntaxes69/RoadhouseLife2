modded class SCR_BandageDamageEffect
{

    protected float m_iMaxHealth = 0.9;
    protected float m_fHealthAdd = 0.25;
	protected float m_fSetBlood = 1;

	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)			
	{
		super.OnEffectAdded(dmgManager);
		Print("SCR_BandageDamageEffect OnEffectAdded");
		
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!damageMgr)
			return;

		Print("CustomBandageLogic");
        damageMgr.RemoveAllBleedings();

		float healthSet;
        HitZone healthZone = damageMgr.GetHitZoneByName("Health");
		if (healthZone) {

			float currentHealth = healthZone.GetHealth()/100;
			//Print("Current Health");
			//Print(currentHealth);
			//Print("MAX");
			Print(healthZone.GetMaxHealth());
            healthSet = currentHealth + m_fHealthAdd;
            healthSet = Math.Min(m_iMaxHealth, healthSet);
			if(healthSet < currentHealth)
				healthSet = currentHealth;
			//Print("Health Set");
			//Print(healthSet);
		} else {
			Print("SCR_BandageDamageEffect Set max health, this shouldnt happen");
			healthSet = m_iMaxHealth;
		}
		RL_MedicalUtils.Heal(damageMgr, healthSet, m_fSetBlood);

    }
	
}