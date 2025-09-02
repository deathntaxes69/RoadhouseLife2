modded class SCR_CharacterControllerComponent
{
	override void OnConsciousnessChanged(bool conscious)
	{
		Print("--------- OnConsciousnessChanged");
		// normally it just returns
		if (GetLifeState() != ECharacterLifeState.INCAPACITATED)
		{
			if(RL_Utils.IsDedicatedServer())
				return;
			// just call the rpc func directly since the var is already set server side
			if (GetCharacter().IsHandcuffed() || GetCharacter().IsZiptied())
				GetGame().GetCallqueue().CallLater(GetCharacter().RpcDo_RestrainPlayer, 1000, false, true, true);
			
			return;
		}

		// This is the only other part changed so gun does not drop when knocked uncon
        //AIControlComponent aiControl = AIControlComponent.Cast(GetOwner().FindComponent(AIControlComponent));
		//if (!aiControl || !aiControl.IsAIActivated())
		//	return;
		
		IEntity currentWeapon;
		BaseWeaponManagerComponent wpnMan = GetWeaponManagerComponent();
		if (wpnMan && wpnMan.GetCurrentWeapon())
			currentWeapon = wpnMan.GetCurrentWeapon().GetOwner();
				
		if (currentWeapon)
		{
			bool dropGrenade = false;
			
			SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(GetAnimationComponent().GetCommandHandler());
			
			EWeaponType wt = wpnMan.GetCurrentWeapon().GetWeaponType();
			if (currentWeapon.FindComponent(GrenadeMoveComponent))
			{
				BaseTriggerComponent triggerComp = BaseTriggerComponent.Cast(currentWeapon.FindComponent(BaseTriggerComponent));
				
				if ((triggerComp && triggerComp.WasTriggered()) || (handler && handler.IsThrowingAction()))
				{
					dropGrenade = true;
				}
			}
			
			if (dropGrenade)
				handler.DropLiveGrenadeFromHand(false); 
			else 
				TryEquipRightHandItem(null, EEquipItemType.EEquipTypeUnarmedContextual, true);
		}
	}
	protected override void UpdateDrowning(float timeSlice, vector waterLevel)
	{
		if (GetLifeState() == ECharacterLifeState.DEAD)
			return;
		
		ChimeraCharacter char = GetCharacter();
		if (!char)
			return;

		SCR_CharacterDamageManagerComponent dmgr = SCR_CharacterDamageManagerComponent.Cast(char.FindComponent(SCR_CharacterDamageManagerComponent));
		if(!dmgr)
			return;
		
		CompartmentAccessComponent accesComp = char.GetCompartmentAccessComponent();
		bool isInWatertightCompartment = accesComp && accesComp.GetCompartment() && accesComp.GetCompartment().GetIsWaterTight();

		const float drowningTimeStartFX = 4;
		//This the only custom part, no drowing if waiting for CPR
		if (waterLevel[2] > 0 && !isInWatertightCompartment && !dmgr.IsWaitingForCpr())
		{
			if (m_fDrowningTime < drowningTimeStartFX && (m_fDrowningTime + timeSlice) > drowningTimeStartFX)
			{
				m_OnPlayerDrowning.Invoke(m_fDrowningDuration, drowningTimeStartFX);
				m_bCharacterIsDrowning = true;
			}
			
			m_fDrowningTime += timeSlice;
		}
		else
		{
			if (m_fDrowningTime != 0)
			{
				m_OnPlayerStopDrowning.Invoke();
				m_bCharacterIsDrowning = false;
			}

			m_fDrowningTime = 0;
			return;
		}

		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageMan)
			return;
		
		if (m_fDrowningTime > m_fDrowningDuration)
		{
			HitZone targetHitZone;
			if (IsUnconscious())
				targetHitZone = damageMan.GetHeadHitZone();
			else
				targetHitZone = damageMan.GetResilienceHitZone();
			
			if (!targetHitZone)
				return;
			
			vector hitPosDirNorm[3];
			SCR_DamageContext context = new SCR_DamageContext(EDamageType.TRUE, 1000, hitPosDirNorm, char, targetHitZone, null, null, -1, -1);
			context.damageEffect = new SCR_DrowningDamageEffect();
			damageMan.HandleDamage(context);
		}
	}
}