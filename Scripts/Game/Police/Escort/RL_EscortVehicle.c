//------------------------------------------------------------------------------------------------
class RL_EscortVehicleClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
class RL_EscortVehicle : GenericEntity
{
	RplComponent m_vehicleRplComp;
	
	SCR_ChimeraCharacter m_escortedCharacter;
	RplComponent m_escortedRplComp;
	SCR_CompartmentAccessComponent m_compAccessComponent;
	Physics m_escortedCharacterPhysics;
	
	BaseCompartmentManagerComponent m_compartmentComp;
	
	RplComponent m_userRplComp;
	SCR_ChimeraCharacter m_userCharacter;
	CharacterControllerComponent m_ownerControlComp;
	CharacterControllerComponent m_userControlComp;
	
	bool m_bNoCollide;
	
	void InitServer(IEntity character, IEntity escortCharacter)
	{
		// Await Rpl component
		GetGame().GetCallqueue().Call(InitServerLate, character, escortCharacter);
	}
	void InitServerLate(IEntity ownerEntity, IEntity escortedCharacter)
	{
		// Give authority to client
		RplComponent rplComponent = RplComponent.Cast(this.FindComponent(RplComponent));
		PlayerManager playerManager = GetGame().GetPlayerManager();
		int playerId = playerManager.GetPlayerIdFromControlledEntity(ownerEntity);
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		rplComponent.Give(playerController.GetRplIdentity());
		
		// Move target player to temp vehicle
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(escortedCharacter.FindComponent(SCR_CompartmentAccessComponent));
		
		bool inCompartment = compartmentAccess.MoveInVehicleAny(this);
		if (!inCompartment)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(this);
			return;
		}

		compartmentAccess.GetOnCompartmentLeft().Insert(DeleteVehicle);
		
		//escortCharacter.GetPhysics().SetInteractionLayer(0);
		
		// Start escort on client
		RplComponent escortedRplComp = RplComponent.Cast(escortedCharacter.FindComponent(RplComponent));
		RplComponent ownerCharacter = RplComponent.Cast(ownerEntity.FindComponent(RplComponent));
		Rpc(RpcDo_InitClient, ownerCharacter.Id(), escortedRplComp.Id());
		RpcDo_InitClient(ownerCharacter.Id(), escortedRplComp.Id());
		
		GetGame().GetCallqueue().CallLater(TryDelete, 1000, true);
		//SetEventMask(EntityEvent.POSTFRAME);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_InitClient(RplId characterId, RplId escortCharacterId)
	{
		Print("RpcDo_InitClient Broadcast");
		// Get Characters from RplId
		m_vehicleRplComp = RplComponent.Cast(this.FindComponent(RplComponent));
		m_escortedRplComp = RplComponent.Cast(Replication.FindItem(escortCharacterId));
		m_userRplComp = RplComponent.Cast(Replication.FindItem(characterId));
		m_escortedCharacter = SCR_ChimeraCharacter.Cast(m_escortedRplComp.GetEntity());
		m_userCharacter = SCR_ChimeraCharacter.Cast(m_userRplComp.GetEntity());
		
		if (m_escortedCharacter.IsInVehicle())
		{
			StopEscort();
			return;
		}

		// Get components from characters
		m_compAccessComponent = SCR_CompartmentAccessComponent.Cast(m_escortedCharacter.FindComponent(SCR_CompartmentAccessComponent));
		m_escortedCharacterPhysics = m_escortedCharacter.GetPhysics();
		m_ownerControlComp = m_escortedCharacter.GetCharacterController();
		m_userControlComp = m_userCharacter.GetCharacterController();
		m_compartmentComp = BaseCompartmentManagerComponent.Cast(FindComponent(BaseCompartmentManagerComponent));
		//m_escortedCharacterPhysics.SetInteractionLayer(0);
		
		// Hack to stop errors?
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if(!character) return;

		//SetEventMask(EntityEvent.POSTFRAME);
		if (m_vehicleRplComp.IsOwner())
		{
			SetEventMask(EntityEvent.FRAME); // Drag
			
		}
	}
	void TryDelete()
	{
		if(!m_ownerControlComp || !m_userControlComp || m_userControlComp.GetLifeState() != ECharacterLifeState.ALIVE || m_ownerControlComp.GetLifeState() != ECharacterLifeState.ALIVE)
		{
			GetGame().GetCallqueue().Remove(TryDelete);
			if(m_escortedCharacter)
				m_escortedCharacter.GetOutEscort();
		}

	}
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if(!this || !m_userCharacter || !m_userControlComp || !m_escortedCharacterPhysics)
		{
			StopEscort();
			//if(Replication.IsServer())
			//	TryDelete();
			return;
		}

		vector userRotation = m_userCharacter.GetYawPitchRoll();
		vector userOrigin = m_userCharacter.GetOrigin();
		vector userForward = userRotation.AnglesToVector();
		float userSpeed = m_userControlComp.GetMovementSpeed();
		
		if(userSpeed > 1 && !m_bNoCollide) {
			m_bNoCollide = true;
			m_escortedCharacterPhysics.SetInteractionLayer(0);
		} else if (userSpeed < 1 && m_bNoCollide) {
			m_bNoCollide = false;
			m_escortedCharacterPhysics.SetInteractionLayer(540672);
		}

		vector targetOrigin = userOrigin + userForward * 1.1;
		
		this.SetOrigin(targetOrigin);
		this.SetYawPitchRoll(userRotation);
	}
	void StopEscort()
	{
		Print("StopEscort");
		ClearEventMask(EntityEvent.FRAME);
		ResetEscort();
		m_escortedCharacter.GetOutEscort();
		GetGame().GetCallqueue().Remove(TryDelete);
		
		//GetGame().GetCallqueue().CallLater(StopEscort, 500);
		
		//RpcDo_GetOutVehicle();
		//Rpc(RpcDo_GetOutVehicle);
	}
	void DeleteVehicle(IEntity targetEntity, BaseCompartmentManagerComponent manager, int mgrID, int slotID, bool move)
	{
		Print("DeleteVehicle");
		if(m_compAccessComponent)
		{
			m_compAccessComponent.GetOnCompartmentLeft().Remove(DeleteVehicle);
			//m_compAccessComponent.GetOutVehicle(EGetOutType.TELEPORT, -1, false, false);
		}
		if(!Replication.IsServer())
			return;
		ResetEscort();
		SCR_EntityHelper.DeleteEntityAndChildren(this);
	}
	
	void ResetEscort()
	{
		Print("ResetEscort");
		if(m_escortedCharacter)
			m_escortedCharacter.SetEscortedState(false);
		GetGame().GetCallqueue().Remove(TryDelete);
	}

}
