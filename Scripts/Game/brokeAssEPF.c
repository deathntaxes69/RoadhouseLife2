modded class EPF_CharacterControllerComponentSaveData
{

	//------------------------------------------------------------------------------------------------
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		EPF_EApplyResult result = EPF_EApplyResult.OK;

		// Apply stance
		m_pCharacterController = SCR_CharacterControllerComponent.Cast(component);
		if (m_pCharacterController.GetStance() != m_eStance)
		{
			switch (m_eStance)
			{
				case ECharacterStance.STAND:
				{
					m_pCharacterController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);
					break;
				}
				case ECharacterStance.CROUCH:
				{
					m_pCharacterController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
					break;
				}
				case ECharacterStance.PRONE:
				{
					m_pCharacterController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOPRONE);
					break;
				}
			}

			result = EPF_EApplyResult.AWAIT_COMPLETION;
			EPF_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::StanceChanged");
			GetGame().GetCallqueue().CallLater(ListenForStanceChangeComplete2, m_pCharacterController.GetStanceChangeDelayTime() * 2000, true, m_pCharacterController);
		}

		// Apply hand items
		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		IEntity rightHandEntity = persistenceManager.FindEntityByPersistentId(m_sRightHandItemId);
		if (rightHandEntity)
		{
			BaseWeaponManagerComponent weaponManager = EPF_Component<BaseWeaponManagerComponent>.Find(m_pCharacterController.GetOwner());
			if (weaponManager)
			{
				WeaponSlotComponent curWeaponSlot = weaponManager.GetCurrentSlot();
				if (!curWeaponSlot || curWeaponSlot.GetWeaponEntity() != rightHandEntity)
				{
					// Because of MP ownership transfer mid animation issues, we wait for the animation event of weapon change instead of the weapon manager scripted events
					EPF_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::RightHandItemEquipped");
					m_pCharacterController.GetOnAnimationEvent().Insert(ListenForWeaponEquipComplete);
					m_pCharacterController.TryEquipRightHandItem(rightHandEntity, m_eRightHandType, false);
				}
				else if (m_bRightHandRaised)
				{
					StartWeaponRaise();
				}

				result = EPF_EApplyResult.AWAIT_COMPLETION;
			}
		}
		else if (m_sLeftHandItemId)
		{
			// No weapon so gadget can be equipped as soon as manager is ready
			SCR_GadgetManagerComponent gadgetMananger = SCR_GadgetManagerComponent.GetGadgetManager(m_pCharacterController.GetOwner());
			if (gadgetMananger)
			{
				EPF_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::GadgetEquipped");
				gadgetMananger.GetOnGadgetInitDoneInvoker().Insert(OnGadgetInitDone);
				result = EPF_EApplyResult.AWAIT_COMPLETION;
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected void ListenForStanceChangeComplete2(SCR_CharacterControllerComponent pCharacterController)
	{
		if (pCharacterController && pCharacterController.IsChangingStance())
			return;

		GetGame().GetCallqueue().Remove(ListenForStanceChangeComplete2);
		EPF_DeferredApplyResult.SetFinished(this, "CharacterControllerComponentSaveData::StanceChanged");
	}

}
