modded class SCR_RegeneratingHitZone : SCR_HitZone
{
	//-----------------------------------------------------------------------------------------------------------
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		IEntity hzOwner = GetOwner();
		if (!hzOwner)
			return;
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(hzOwner);
		SCR_PlayerController playerController =  SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;

		if (EntityUtils.IsPlayer(hzOwner) && !playerController.IsPossessing())
		{
            // Dont let health go to 0
			if(GetHealthScaled() < 0.01)
			{

				SetHealthScaled(0.01);
				
			}
		}
		
		super.OnDamage(damageContext);
	}
	/*
	override void OnHealthSet()
	{
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageMgr)
			return;

		// Dont allow regen if waiting for CPR
		if(damageMgr.IsWaitingForCpr())
			return;
		
		super.OnHealthSet();

		
	}*/
};