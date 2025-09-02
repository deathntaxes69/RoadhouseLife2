modded class SCR_ChimeraCharacter
{
	[RplProp()]
	protected bool m_bSurrendering = false;

	void InitAnimationKeyBinds()
	{
		ResetAnimationVars();
		RL_Utils.GetLocalCharacter().RestrainPlayer(false, false);
	}

	void ResetAnimationVars()
	{
		m_bSurrendering = false;
		Print("Surrender ResetAnimationVars");
	}

	void OnSurrender()
	{
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();

		if (character.IsRestrained())
			return;

		if (m_bSurrendering)
		{
			Rpc(RpcAsk_SetSurrender, false);
			character.GetCharacterController().StopCharacterGesture();
			return;
		}

		character.GetCharacterController().StopCharacterGesture();
		PreformGesture(character, 232);
		Rpc(RpcAsk_SetSurrender, true);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SetSurrender(bool state)
	{
		m_bSurrendering = state;
		Replication.BumpMe();
	}

	bool IsSurendering()
	{
		return m_bSurrendering;
	}

	void PreformGesture(SCR_ChimeraCharacter character, int gestureId)
	{
		Print("++++++++++ PreformGesture");
		private int waitTime = 0;

		SCR_CharacterControllerComponent cCtrl = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		SCR_InventoryStorageManagerComponent storageManager = SCR_InventoryStorageManagerComponent.Cast(cCtrl.GetInventoryStorageManager());
		SCR_CharacterInventoryStorageComponent storage = storageManager.GetCharacterStorage();
		BaseWeaponManagerComponent weaponManager = cCtrl.GetWeaponManagerComponent();

		if (cCtrl.IsGadgetInHands())
		{
			waitTime = 1000;
			cCtrl.RemoveGadgetFromHand();
		}
		if (weaponManager.GetCurrentSlot() && weaponManager.GetCurrentSlot().GetWeaponEntity())
		{
			waitTime = 1000;
			storage.UnequipCurrentItem();
		}
		if (cCtrl.IsPlayingGesture())
			cCtrl.StopCharacterGesture();

		GetGame().GetCallqueue().CallLater(cCtrl.ForceStance, waitTime, false, 0);
		GetGame().GetCallqueue().CallLater(cCtrl.TryStartCharacterGesture, waitTime+ 500, false, gestureId, 0);
	}

}
