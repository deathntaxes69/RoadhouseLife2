// seatbelt
modded class SCR_CarControllerComponent
{
	protected SCR_ChimeraCharacter m_character;

	override void OnCompartmentEntered(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		super.OnCompartmentEntered(vehicle, mgr, occupant, managerId, slotID);
		if (Replication.IsServer() || occupant != GetGame().GetPlayerController().GetControlledEntity())
			return;

		//Print("OnCompartmentEntered");

		m_character = RL_Utils.GetLocalCharacter();
		if (!m_character) return;
		GetGame().GetCallqueue().CallLater(m_character.SetSeatBelt, 100, false, false);

		GetGame().GetInputManager().AddActionListener("RL_SeatbeltToggleAction", EActionTrigger.DOWN, OnSeatbeltToggle);

		CompartmentAccessComponent compartmentAccess = m_character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return;
		
		BaseCompartmentSlot compartment = compartmentAccess.GetCompartment();
		if (!compartment)
			return;

		string characterId = m_character.GetCharacterId();
		VehicleWheeledSimulation m_VehicleWheeledSimulation = VehicleWheeledSimulation.Cast(vehicle.FindComponent(VehicleWheeledSimulation));
		//PrintFormat("ISPiloting = %1", compartment.IsPiloting());

		//if (!characterId || !m_VehicleWheeledSimulation || !compartment.IsPiloting())
			//return;

		GetGame().GetCallqueue().Remove(DoorCloseCheck);
		GetGame().GetCallqueue().CallLater(DoorCloseCheck, 5000, true, m_VehicleWheeledSimulation, compartmentAccess, mgr, vehicle);
	}

	override void OnCompartmentLeft(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		super.OnCompartmentLeft(vehicle, mgr, occupant, managerId, slotID);

		if (Replication.IsServer())
		{
			if (mgr.FindCompartment(slotID).Type() == PilotCompartmentSlot)
			{
				//Print("++++++++++++ AUTO HANDBREAK !!!");
				CarControllerComponent m_Vehicle_c = CarControllerComponent.Cast(mgr.FindComponent(CarControllerComponent));
				if (m_Vehicle_c)
					m_Vehicle_c.SetPersistentHandBrake(true);
			}
		} else {
			if (occupant != GetGame().GetPlayerController().GetControlledEntity())
				return;

			GetGame().GetInputManager().RemoveActionListener("RL_SeatbeltToggleAction", EActionTrigger.DOWN, OnSeatbeltToggle);

			GetGame().GetCallqueue().Remove(DoorCloseCheck);
		}
	}

	protected void OnSeatbeltToggle(float value, EActionTrigger trigger)
	{
		m_character = RL_Utils.GetLocalCharacter();
		if (m_character.IsSpamming())
			return;

		if (!m_character.GetParent()) {
			GetGame().GetInputManager().RemoveActionListener("RL_SeatbeltToggleAction", EActionTrigger.DOWN, OnSeatbeltToggle);
			return;
		}

		m_character.SetSeatBelt(!m_character.GetSeatBelt());
		m_character.PlayCommonSoundLocally("SOUND_SEATBELT");
	}

	protected void DoorCloseCheck(VehicleWheeledSimulation m_VehicleWheeledSimulation, CompartmentAccessComponent compartmentAccess, BaseCompartmentManagerComponent mgr, IEntity vehicle)
	{
		//PrintFormat("isDoorOpen %1 %2 %3 %4", mgr.IsDoorOpen(0), mgr.IsDoorOpen(1), mgr.IsDoorOpen(2), mgr.IsDoorOpen(3));
		if (!m_VehicleWheeledSimulation || !compartmentAccess || !mgr || !vehicle)
		{
			//Print("Removing DoorCloseCheck");
			GetGame().GetCallqueue().Remove(DoorCloseCheck);
			return;
		}

		if (m_VehicleWheeledSimulation.GetSpeedKmh() > 20)
		{
			for (int i = 0; i < 4; i++)
			{
				//Print("[RL_CarControllerComponent] DoorCloseCheck for");
				if (mgr.IsDoorOpen(i))
					GetGame().GetCallqueue().CallLater(compartmentAccess.CloseDoor, 150 * i, false, vehicle, ECharacterDoorAnimType.NO_CHARACTER_ANIM, i);
			}
		}
	}

}
