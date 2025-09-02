// seatbelt
modded class SCR_VehicleDamageManagerComponent
{
	override void HandlePassengerDamage(EDamageType damageType, float damage, notnull Instigator instigator)
    {
        float newDamage = Math.Min(damage/2, 20);
		Print("collision damage " + damage.ToString() + ", new " + newDamage.ToString());

        // ejecto seat cuz
		if (damage > 60 && damageType == EDamageType.COLLISION) // probably adjust this later - eject sensitivity basically
        {
			//Print("ejectoseat");
            // Damage occupants of the car that experienced a collision
            m_CompartmentManager.DamageOccupants(newDamage, EDamageType.COLLISION, instigator);
			SeatbeltEjectAllOccupants();

            return;
        }
        /*
        if (damageType == EDamageType.EXPLOSIVE)
        {
            // An explosion of minExplosionEjectionDamageThreshold or larger is capable of ejecting occupants.
            //if (damage > GetMinExplosionDamageEjectionThreshold())
            SeatbeltEjectAllOccupants(true);

            return;
        }
		*/
    }

    // Decrease the ability to damage vehicles
    override bool HijackDamageHandling(notnull BaseDamageContext damageContext)
    {
        Print("HijackDamageHandling");
        if(damageContext.damageValue > 500)
            return true;
        return false;
    }
	
    void SeatbeltEjectAllOccupants(bool force = false)
    {
        array<BaseCompartmentSlot> compartments = {};
		m_CompartmentManager.GetCompartments(compartments);

		SCR_ChimeraCharacter m_character;
		bool characterEjected;
		bool ejectedImmediately;

		foreach (BaseCompartmentSlot compartment : compartments)
		{
            //Print("[RL_VehicleDamageManagerComp] SeatbeltEjectAllOccupants foreach");
			if (compartment)
			{
				m_character = SCR_ChimeraCharacter.Cast(compartment.GetOccupant());

				if (m_character && (!m_character.GetSeatBelt() || force))
					m_character.DoSeatbeltRagdollLocal();
			}
		}
    }
}
