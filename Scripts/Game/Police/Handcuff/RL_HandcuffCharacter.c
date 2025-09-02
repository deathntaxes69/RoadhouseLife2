modded class SCR_ChimeraCharacter
{
	[RplProp()]
	protected bool m_bIsHandcuffed = false;
	
	[RplProp()]
	protected bool m_bIsZiptied = false;

	protected int m_iCuffGesture = 231;

	bool IsRestrained()
	{
	    return m_bIsHandcuffed || m_bIsZiptied;
	}
	
	bool IsHandcuffed()
	{
		return m_bIsHandcuffed;
	}
	
	bool IsZiptied()
	{
		return m_bIsZiptied;
	}

	void RestrainPlayer(bool state, bool doRpc = true)
	{
		Print("RestrainPlayer");
		Print(m_bIsHandcuffed);
		Print("New");
		Print(state);
		m_bIsHandcuffed = state;
		m_bIsZiptied = false;
		Replication.BumpMe();
		if (doRpc)
			Rpc(RpcDo_RestrainPlayer, state, false);
	}
	
	void ZiptiePlayer(bool state, bool doRpc = true)
	{
		Print("ZiptiePlayer");
		Print(m_bIsZiptied);
		Print("New");
		Print(state);
		m_bIsZiptied = state;
		m_bIsHandcuffed = false;
		Replication.BumpMe();
		if (doRpc)
			Rpc(RpcDo_RestrainPlayer, state, false);
	}
	
	void RestrainAgain()
	{
		if (m_bIsHandcuffed)
			Rpc(RpcDo_RestrainPlayer, true, false);
		else if (m_bIsZiptied)
			Rpc(RpcDo_RestrainPlayer, true, false);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_RestrainPlayer(bool state, bool ignoreUncon)
	{
		Print("RpcDo_RestrainPlayer");
		SCR_CharacterControllerComponent cCtrl = SCR_CharacterControllerComponent.Cast(GetCharacterController());
		SCR_InventoryStorageManagerComponent storageManager = SCR_InventoryStorageManagerComponent.Cast(cCtrl.GetInventoryStorageManager());
		SCR_CharacterInventoryStorageComponent storage = storageManager.GetCharacterStorage();
		BaseWeaponManagerComponent weaponManager = cCtrl.GetWeaponManagerComponent();
		SCR_CompartmentAccessComponent compAccessComponent = SCR_CompartmentAccessComponent.Cast(this.FindComponent(SCR_CompartmentAccessComponent));

		Print("DEBUG0");
		if (state) {
			// Return if uncon, when they come back we will just run this again then ignore this
			if (cCtrl.IsUnconscious() && !ignoreUncon)
				return;
			Print("DEBUG1");

			private int waitTime = 0;

			auto menuManager = GetGame().GetMenuManager();
			if (menuManager.IsAnyMenuOpen() || menuManager.IsAnyDialogOpen()) {
				menuManager.CloseAllMenus();
			}
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

			if (waitTime > 0) {
				GetGame().GetCallqueue().CallLater(cCtrl.ForceStance, waitTime, false, 0);
				GetGame().GetCallqueue().CallLater(cCtrl.TryStartCharacterGesture, waitTime+ 500, false, m_iCuffGesture, 0);
				//Print("handcuff wait");
			} else if (cCtrl.IsPlayingGesture()) {
				Print("WasPlayingGesture");
				GetCharacterController().StopCharacterGesture();
				GetGame().GetCallqueue().CallLater(cCtrl.TryStartCharacterGesture, 500, false, m_iCuffGesture, 0);
				//Print("handcuff isplayingg");
			} else {
				Print("HANDCUFF ELSE");
				if (cCtrl.GetStance() == 2)
				{
					GetCharacterController().StopCharacterGesture();
					GetGame().GetCallqueue().CallLater(cCtrl.ForceStance, 1500, false, 0);
					GetGame().GetCallqueue().CallLater(cCtrl.TryStartCharacterGesture, 2000, false, m_iCuffGesture, 0);
				} else {
					cCtrl.TryStartCharacterGesture(m_iCuffGesture);
				}
			}
			cCtrl.ForceStance(0);
			//GetGame().GetCallqueue().CallLater(ControlMovement, 2000, true);
			compAccessComponent.GetOnCompartmentLeft().Insert(OnVehicleLeft);
		} else {
			m_bIsHandcuffed = false;
			m_bIsZiptied = false;
			Replication.BumpMe();
			
			ResetAnimationVars();
			cCtrl.StopCharacterGesture();
			//ControlMovement();
			compAccessComponent.GetOnCompartmentLeft().Remove(OnVehicleLeft);
		}

	}

	void ControlMovement()
	{
		if (!IsRestrained() || GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
		{
			GetCharacterController().StopCharacterGesture();
			GetGame().GetCallqueue().Remove(ControlMovement);
			return;
		}

		auto menuManager = GetGame().GetMenuManager();
		if (menuManager.IsAnyMenuOpen() || menuManager.IsAnyDialogOpen()) {
			menuManager.CloseAllMenus();
		}
		/*
		if (m_bIsHandcuffed && !IsInVehicle())
		{

			if (GetCharacterController().GetStance() != ECharacterStance.STAND)
			{
				//GetCharacterController().StopCharacterGesture();
				GetCharacterController().ForceStance(0);
				GetCharacterController().TryStartCharacterGesture(m_iCuffGesture, 0);
			}

	}*/
	}

	protected void OnVehicleLeft(IEntity vehicle, BaseCompartmentManagerComponent manager, int mgrID, int slotID)
	{
		if (!IsRestrained())
			return;

		//GetGame().GetCallqueue().CallLater(GetCharacterController().StopCharacterGesture, 500, false);
		//cCtrl.StopCharacterGesture();
		GetGame().GetCallqueue().CallLater(GetCharacterController().TryStartCharacterGesture, 1000, false, m_iCuffGesture, 0);
	}

}
