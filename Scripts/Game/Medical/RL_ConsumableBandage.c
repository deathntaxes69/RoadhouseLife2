
[BaseContainerProps()]
modded class SCR_ConsumableBandage : SCR_ConsumableEffectHealthItems
{
	// Allow bandages to work even if they are not bleeding
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
        Print("SCR_ConsumableBandage CanApplyEffect");
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return false;
	
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageMgr)
			return false;
        Print("SCR_ConsumableBandage CanApplyEffect");
        bool test = damageMgr.CanBeHealed() || damageMgr.IsBleeding();
        if(test)
            Print("return true");
		return (damageMgr.CanBeHealed() || damageMgr.IsBleeding());
	}
    
    override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
        Print("SCR_ConsumableBandage CanApplyEffectToHZ");
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return false;
		
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		return damageMgr && (damageMgr.CanBeHealed() || damageMgr.GetGroupDamageOverTime(group, EDamageType.BLEEDING) > 0);
	}
    override ItemUseParameters GetAnimationParameters(IEntity item, notnull IEntity target, ECharacterHitZoneGroup group = ECharacterHitZoneGroup.VIRTUAL)
	{
        Print("GetAnimationParameters");
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return null;
		
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageMgr)
			return null;

		EBandagingAnimationBodyParts bodyPartToBandage = EBandagingAnimationBodyParts.Invalid;
		
		if (group != ECharacterHitZoneGroup.VIRTUAL)
		{
			if (!damageMgr.GetGroupIsBeingHealed(group))
				bodyPartToBandage = damageMgr.FindAssociatedBandagingBodyPart(group);
		}
		else
		{
			group = damageMgr.GetCharMostDOTHitzoneGroup(EDamageType.BLEEDING, ignoreIfBeingTreated: true);
			array<HitZone> hitzones = {};
			damageMgr.GetHitZonesOfGroup(group, hitzones);
			if (!hitzones || hitzones.IsEmpty())
				return null;
				
			SCR_CharacterHitZone hitzone = SCR_CharacterHitZone.Cast(hitzones[0]);
            //this usually returns null
			if (hitzone)
			    bodyPartToBandage = hitzone.GetBodyPartToHeal();
		}
		//usually returns null
		if (bodyPartToBandage == EBandagingAnimationBodyParts.Invalid)
			bodyPartToBandage = EBandagingAnimationBodyParts.LowerTorso;
		
		bool allowMovement = bodyPartToBandage != EBandagingAnimationBodyParts.RightLeg && bodyPartToBandage != EBandagingAnimationBodyParts.LeftLeg;
		
		ItemUseParameters params = ItemUseParameters();
		params.SetEntity(item);
		params.SetAllowMovementDuringAction(allowMovement);
		params.SetKeepInHandAfterSuccess(false);
		params.SetCommandID(GetApplyToSelfAnimCmnd(target));
		params.SetCommandIntArg(1);
		params.SetCommandFloatArg(0.0);
		params.SetMaxAnimLength(m_fApplyToSelfDuration);
		params.SetIntParam(bodyPartToBandage);
			
		return params;
	}
}


