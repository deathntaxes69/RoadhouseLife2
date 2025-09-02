modded class SCR_ChimeraCharacter
{
	bool GetSeatBelt()
	{
		return m_bSeatBelt;
	}

	void SetSeatBelt(bool newSeatBeltValue)
	{
		if (m_bSeatBelt == newSeatBeltValue)
		{
			Print("not setting seatbelt");
			return;
		}
		Rpc(RpcAsk_SetSeatBelt, newSeatBeltValue);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_SetSeatBelt(bool newSeatBeltValue)
	{
		//Print("RpcAsk_SetSeatBelt");
		//Print(newSeatBeltValue);
		if (m_bSeatBelt == newSeatBeltValue)
		{
			//Print("not setting seatbelt");
			return;
		}
		m_bSeatBelt = newSeatBeltValue;
		Replication.BumpMe();
	}

	void DoSeatbeltRagdollLocal()
	{
		Rpc(RpcDo_SeatbeltRagdoll);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_SeatbeltRagdoll()
	{
		//Print("RpcDo_SeatbeltRagdoll");
		SCR_CompartmentAccessComponent access = SCR_CompartmentAccessComponent.Cast(this.GetCompartmentAccessComponent());
		if (!access)
			return;

		vector mat[4];
		this.GetTransform(mat);
		mat[3] = this.AimingPosition();

		access.GetOutVehicle_NoDoor(mat, false, false);

		GetGame().GetCallqueue().CallLater(SeatbeltRagdollVelocity, 105, false);
	}

	void SeatbeltRagdollVelocity()
	{
		this.GetCharacterController().Ragdoll();

		// launch them forward and up
		Physics entPhys = this.GetPhysics();
		entPhys.SetVelocity(entPhys.GetVelocity() + this.VectorToParent("0 5 5"));

		if (Math.RandomInt(0, 10) > 3) {
			// try to force break their leg
			SCR_CharacterDamageManagerComponent m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(this.FindComponent(SCR_CharacterDamageManagerComponent));
			m_DamageManager.GetHitZoneByName("LCalf").SetHealth(0);
		}
	}
}
