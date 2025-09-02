modded class SCR_CharacterDamageManagerComponent
{
	override void HandleRagdollFallDamage(notnull HitZone contactingHitZone, float damage)
	{
		//return;

		array<int> colliderIDs = {};
		contactingHitZone.GetColliderIDs(colliderIDs);
		if (colliderIDs.IsEmpty())
			return;

		vector colliderTransform[4];
		Physics physics = GetOwner().GetPhysics();

		physics.GetGeomWorldTransform(colliderIDs[0], colliderTransform);

		array<HitZone> targetHitZones = {};
		GetPhysicalHitZones(targetHitZones);
		GetNearestHitZones(colliderTransform[3], targetHitZones, 5);

		if (targetHitZones.IsEmpty())
			return;

		vector hitPosDirNorm[3];
		hitPosDirNorm[0] = colliderTransform[3];

		// less damage from ragdoll for seatbelt
		SCR_DamageContext context = new SCR_DamageContext(EDamageType.COLLISION, (damage * 23) / targetHitZones.Count(), hitPosDirNorm, GetOwner(), null, GetInstigator(), null, -1, -1);
		context.damageEffect = new SCR_RagdollFallDamageEffect();

		foreach (HitZone hitZone : targetHitZones)
		{
			//Print("[SCR_CharacterDamageManagerComponent] HandleRagdollFallDamage foreach loop");
			context.struckHitZone = hitZone;
			HandleDamage(context);
		}
	}

	override void HandleAnimatedFallDamage(float damage)
	{
		array<HitZone> targetHitZones = {};
		GetHitZonesOfGroup(ECharacterHitZoneGroup.LEFTLEG, targetHitZones, true);
		GetHitZonesOfGroup(ECharacterHitZoneGroup.RIGHTLEG, targetHitZones, false);

		if (targetHitZones.IsEmpty())
			return;
		damage /= 2.5; // less fall damage

		const float overDamageCutOff = 50;
		if (damage > overDamageCutOff)
			GetHitZonesOfGroup(ECharacterHitZoneGroup.LOWERTORSO, targetHitZones, false);

		damage /= 2;
		vector hitPosDirNorm[3];
		SCR_DamageContext context = new SCR_DamageContext(EDamageType.COLLISION, damage/targetHitZones.Count(), hitPosDirNorm, GetOwner(), null, Instigator.CreateInstigator(GetOwner()), null, -1, -1);
		context.damageEffect = new SCR_AnimatedFallDamageEffect();

		foreach (HitZone hitZone : targetHitZones)
		{
			//Print("[SCR_CharacterDamageManagerComponent] HandleAnimatedFallDamage foreach loop");
			context.struckHitZone = hitZone;
			HandleDamage(context);
		}

		// Give character impulse when dying from animated falldamage to make the ragdoll transition more seamless and smooth
		SCR_CharacterAnimationComponent animComp = SCR_CharacterAnimationComponent.Cast(GetOwner().FindComponent(SCR_CharacterAnimationComponent));
		if (animComp)
		{
			float randomDirectional = Math.RandomFloat(-0.5, 0.3);
			animComp.AddRagdollEffectorDamage(Vector(randomDirectional, 1, 0), Vector(randomDirectional, -0.5, 0.3), 82, 15, 2);
		}
	}

}
