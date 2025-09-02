sealed enum RL_TOW_VEHICLE_TYPE {
    Generic,
	TowTruck = 1,
	Trailer = 2,
    TrailerTruck = 3
}

class RL_TowComponentClass: ScriptComponentClass {}

class RL_TowComponent : ScriptComponent
{
	[Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_TOW_VEHICLE_TYPE))]
	private RL_TOW_VEHICLE_TYPE m_towVehicleType;

	[Attribute()]
	private ref PointInfo m_vTowAttachmentPoint;

	[Attribute()]
	private ref PointInfo m_vTruckAttachmentPoint;
	
	[Attribute(defvalue: "10")]
	private float m_fMaxTowAttachDistance;
	
	[Attribute(defvalue: "0.5")]
	private float m_fMaxTrailerAttachDistance;

	[RplProp()]
	private RplId m_iAttachedTo = Replication.INVALID_ID;
	
	[RplProp()]
	private bool m_bIsSlave = false;

	[RplProp()]
	private bool m_bHitchTow = false;
	
	//protected Physics6DOFJoint m_Joint;
	protected PhysicsJoint m_Joint;
	protected float m_iOrginalMass;
	protected vector m_iOrginalRelativePos;
	protected RplComponent m_RplComponent;
	protected NwkMovementComponent m_NwkMovement;
	protected NwkCarMovementComponent m_NwkCarMovement;

	[RplProp()]
	bool m_bIsLandingGearUp = false;

	
	
	override void OnPostInit(IEntity owner)
	{
		
		if (m_vTowAttachmentPoint)
		{
			m_vTowAttachmentPoint.Init(owner);
		}

		if (m_vTruckAttachmentPoint)
		{
			m_vTruckAttachmentPoint.Init(owner);
		}
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_NwkCarMovement = NwkCarMovementComponent.Cast(owner.FindComponent(NwkCarMovementComponent)); // trucks
		m_NwkMovement = NwkMovementComponent.Cast(owner.FindComponent(NwkMovementComponent)); // trailers
		
		SetEventMask(owner, EntityEvent.POSTFRAME);
		SetEventMask(owner, EntityEvent.VISIBLE);
		
		EventHandlerManagerComponent ev = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
		if (ev)
		{
			ev.RegisterScriptHandler("OnDamageStateChanged", this, OnDamageStateChanged);
			ev.RegisterScriptHandler("OnCompartmentEntered", this, OnCompartmentEntered);
			ev.RegisterScriptHandler("OnCompartmentLeft", this, OnCompartmentLeft);
		}
	}
	
	float lastSendTime = -50000;
	float updateFrequency = 20;
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
	
		//if (Replication.IsServer() || GetGame().GetIsClientAuthority())
		//	return;
		if (RL_Utils.IsDedicatedServer())
			return;


		if(!GetAttachedEntity())
			return;
        Print("PostFrame");

		float curTime = System.GetTickCount();
		if ((curTime - lastSendTime) > 1000/updateFrequency)
		{
            vector mat[4];
		    GetOwner().GetTransform(mat);
			Rpc(Rpc_Server_ReceiveNewStates, mat, GetOwner().GetPhysics().GetVelocity(), GetOwner().GetPhysics().GetAngularVelocity());
			lastSendTime = curTime;
		}
	}
	bool IsTowOrTrailerTruck()
	{
		return (GetVehicleType() == RL_TOW_VEHICLE_TYPE.TowTruck || GetVehicleType() == RL_TOW_VEHICLE_TYPE.TrailerTruck);
	}
	bool IsTrailerOrTrailerTruck()
	{
		return (GetVehicleType() == RL_TOW_VEHICLE_TYPE.Trailer || GetVehicleType() == RL_TOW_VEHICLE_TYPE.TrailerTruck);
	}
	RL_TOW_VEHICLE_TYPE GetVehicleType()
	{
		return m_towVehicleType;
	}
	[RplRpc(RplChannel.Unreliable, RplRcver.Server)]
	void Rpc_Server_ReceiveNewStates(vector mat[4], vector velocity, vector angularVelocity)
	{
        //Print("Rpc_Server_ReceiveNewStates");
		GetOwner().SetTransform(mat);
		GetOwner().GetPhysics().SetVelocity(velocity);	
		GetOwner().GetPhysics().SetAngularVelocity(angularVelocity);
	}
	
	IEntity GetAttachedEntity()
	{
		//Print("GetAttachedEntity");
		//Print(m_iAttachedTo);
		if (m_iAttachedTo == Replication.INVALID_ID)
		{
			return null;
		}
		
		return EPF_NetworkUtils.FindEntityByRplId(m_iAttachedTo);
	}
	
	int GetAttachedEntityRplId()
	{
		return m_iAttachedTo;
	}
	
	void SetAttachedEntity(IEntity entity, bool hitchTow, bool notifyOtherEntity = true)
	{
		Print("SetAttachedEntity");
		if (!entity || m_iAttachedTo != Replication.INVALID_ID)
		{
			return;
		}
		
		int id = EPF_NetworkUtils.GetRplId(entity);
		if (id == Replication.INVALID_ID) 
		{
			return;
		}
        int newOwnerRplIdentity = Replication.INVALID_ID;
        if(!RL_Utils.IsDedicatedServer())
        {
            Print("CreateJoint");
            SetNwk();
            bool didCreateJoint = CreateJoint(entity);
            if (!didCreateJoint)
            {
                Print("did not create joint");
                DetachEntity();
                return;
            }
            m_NwkMovement.EnableSimulation(true);
		    m_NwkMovement.EnableInterpolation(true);

			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			newOwnerRplIdentity = playerController.GetRplIdentity();
            Print(newOwnerRplIdentity);
		};
		Rpc(RpcAsk_AttachEntity, id, newOwnerRplIdentity, hitchTow, notifyOtherEntity);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_AttachEntity(RplId entityId, int newOwnerRplIdentity, bool hitchTow, bool notifyOtherEntity)
	{
		Print("RpcAsk_AttachEntity");
		IEntity attachedEntity = EPF_NetworkUtils.FindEntityByRplId(entityId);
		//Print(attachedEntity);
		RL_TowComponent otherAttachment = RL_TowComponent.Cast(attachedEntity.FindComponent(RL_TowComponent));

		if (!otherAttachment)
		{
			Print("Can not attach to something without RL_TowComponent!", LogLevel.ERROR);
			return;
		}
		
		m_iAttachedTo = entityId;
		m_bHitchTow = hitchTow;
        Replication.BumpMe();

        SetNwk();

		SignalLandingGear(true);
		
        Print(string.Format("Before transwer owner to %1", newOwnerRplIdentity));
        // Give ownership of trailer to driver
        if(newOwnerRplIdentity != Replication.INVALID_ID)
        {
            RplComponent rplComponent = RplComponent.Cast(attachedEntity.FindComponent(RplComponent));
			if (!rplComponent)
            {
                Print("Attached entity has not RplComp");
                return;
            }
			Print(string.Format("------------- Give owner to %1", newOwnerRplIdentity));
            Print(attachedEntity);

			rplComponent.GiveExt(newOwnerRplIdentity, false);

        }

		// Only run the joint logic on one of the entities (in this case it will be the entity getting attached to the entity where this was originally called from)
		// (if called from trailer, then the truck will be the one creating the joint)
		if (!notifyOtherEntity)
			return;
		
		otherAttachment.SetAttachedEntity(GetOwner(), hitchTow, false);
		Replication.BumpMe();
	}
	
	void DetachEntity(bool notifyOtherEntity = true)
	{
        Print("DetachEntity");
		if (m_iAttachedTo == Replication.INVALID_ID)
		{
            Print("DetachEntity called but noting to detach");
			return;
		}
		
		Rpc(RpcAsk_DetachEntity, notifyOtherEntity);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_DetachEntity(bool notifyOtherEntity)
	{
        Print("RpcAsk_DetachEntity");
		if (m_iAttachedTo == Replication.INVALID_ID)
		{
            Print("RpcAsk_DetachEntity called but noting to detach");
			return;
		}
		
		IEntity attachedEntity = GetAttachedEntity();
		if (attachedEntity)
		{
			RL_TowComponent otherAttachment = RL_TowComponent.Cast(attachedEntity.FindComponent(RL_TowComponent));
			if (otherAttachment && notifyOtherEntity)
			{
				otherAttachment.RpcAsk_DetachEntity(false);
			}
		}
		ServerDestroyAllJoints();
		if (notifyOtherEntity) {
		
				
			Physics physics = GetOwner().GetPhysics();
			Physics physicsOther = attachedEntity.GetPhysics();
			if (physics && physicsOther)
			{
				if(m_iOrginalRelativePos)
				{
					Print("SET NEW POS");
					vector newPos = GetOwner().VectorToParent(m_iOrginalRelativePos);
					attachedEntity.SetOrigin(newPos);
				}
				physics.SetActive(true);
				physicsOther.SetActive(true);
				if(m_iOrginalMass)
					physicsOther.SetMass(m_iOrginalMass);
			}
			//Rpc(Rpc_Broadcast_DestroyJoint);
			//Rpc(Rpc_Owner_DestroyJoint);
		}
		SignalLandingGear(false);
		m_bIsSlave = false;
		m_iAttachedTo = Replication.INVALID_ID;
		Replication.BumpMe();
	}
	
	void ServerDestroyAllJoints()
	{
		Print("Destroy all joints");
		DestroyJoint();
		Rpc(Rpc_Broadcast_DestroyJoint);
		Rpc(Rpc_Owner_DestroyJoint);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void Rpc_Broadcast_DestroyJoint()
	{
		Print("Destroy joint broadcast");
		DestroyJoint();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void Rpc_Owner_DestroyJoint()
	{
		Print("Destroy joint owner");
		DestroyJoint();
	}
	
	bool CreateJoint(IEntity attachedEntity)
	{
		if(!attachedEntity)
			return false;
		Print("CreateJoint DEBUG 1");
		RL_TowComponent otherAttachment = RL_TowComponent.Cast(attachedEntity.FindComponent(RL_TowComponent));
		if (!otherAttachment)
		{
			return false;
		}
		Print("CreateJoint DEBUG 2");
		Physics physics = GetOwner().GetPhysics();
		Physics physicsOther = attachedEntity.GetPhysics();
		
		if (!physics || !physicsOther)
		{
			return false;
		}
		Print("CreateJoint DEBUG 3");
		physics.SetActive(true);
		//int ogInteractionLayer = physicsOther.GetInteractionLayer();
		//physicsOther.SetInteractionLayer(0);
		
		//if(m_bHitchTow)
		if(IsTrailerOrTrailerTruck() && otherAttachment.IsTrailerOrTrailerTruck())
		{
			vector mat[4], otherMat[4];
			physicsOther.SetActive(true);
			GetTruckPointRelative(mat);
			otherAttachment.GetTruckPointRelative(otherMat);
			m_Joint = PhysicsJoint.Create6DOF(GetOwner(), attachedEntity, mat, otherMat, false, -1);
			Print(mat);
			Print(otherMat);
		} else {
			m_iOrginalRelativePos = GetOwner().VectorToLocal(attachedEntity.GetOrigin());
			m_iOrginalMass = physicsOther.GetMass();
			physicsOther.SetActive(true);
			physicsOther.SetMass(m_iOrginalMass * 0.4);
			attachedEntity.SetYawPitchRoll(GetOwner().GetYawPitchRoll());
			vector mat = GetAttachPoint();
			//vector otherMat = otherAttachment.GetAttachPoint();
			vector otherMat = otherAttachment.GetAutoAttachPoint(attachedEntity);
			m_Joint = PhysicsJoint.CreateFixed(GetOwner(), attachedEntity, mat, otherMat, true, -1);
			Print(mat);
			Print(otherMat);
		}
		//GetGame().GetCallqueue().CallLater(DelayedRestoreInteractionLayer, 5000, false, ogInteractionLayer);
		
		Print(GetOwner());
		Print(attachedEntity);
		Print(m_Joint);
		Print("CreateJoint DEBUG 4");
		return true;
	}
	void DelayedRestoreInteractionLayer( int ogLayer)
	{
		GetAttachedEntity().GetPhysics().SetInteractionLayer(ogLayer);
	}
	bool DestroyJoint()
	{
		if (m_Joint)
		{
            Print("--------- Actually destroy joint");
			m_Joint.Destroy();
			m_Joint = null;
		}
		
		return true;
	}
	
	void GetTruckPointRelative(out vector mat[4])
	{
		m_vTruckAttachmentPoint.GetModelTransform(mat);
		
		Physics physics = GetOwner().GetPhysics();
		if (!physics) return;
		
		mat[3] = mat[3] - physics.GetCenterOfMass();
	}
	vector GetAttachPoint(bool forTrailer = false)
	{
		vector mat[4];
		if(GetVehicleType() == RL_TOW_VEHICLE_TYPE.TowTruck || forTrailer)
			m_vTruckAttachmentPoint.GetModelTransform(mat);
		else
			m_vTowAttachmentPoint.GetModelTransform(mat);
		
		return mat[3];
	}
	vector GetAutoAttachPoint(IEntity ent)
	{
		vector mins, maxs;
		ent.GetBounds(mins, maxs);
		
		vector localPos = vector.Lerp(mins, maxs, 0.5);
		localPos[1] = mins[1];
		localPos[2] = maxs[2];
		return localPos;

	}	
	float GetMaxAttachDistance()
	{
		return m_fMaxTowAttachDistance;
	}
	
	bool IsSlave()
	{
		return m_bIsSlave;
	}
	void OnCompartmentEntered(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		if(!this.IsTowOrTrailerTruck())
            return;

		if (!RL_Utils.IsDedicatedServer())
			GetGame().GetInputManager().AddActionListener("RL_TowAction", EActionTrigger.DOWN, OnTowAction);
        
        bool isDriver = IsDriver(mgr, occupant, managerId, slotID);

        if (!RL_Utils.IsDedicatedServer() && isDriver)
			AddTowKeyBind();

        if(!GetAttachedEntity())
			return;


        SetNwk();
		
		if (Replication.IsServer())
			return;

		if(!isDriver) return;
		
		DestroyJoint();
		CreateJoint(GetAttachedEntity());
		m_NwkMovement.EnableSimulation(true);
		m_NwkMovement.EnableInterpolation(true);
	}
	void OnCompartmentLeft(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		//if(!GetGame().GetIsClientAuthority() && m_NwkCarMovement && m_RplComponent)
		Print("Tow OnCompartmentLeft ");
		if(!this.IsTowOrTrailerTruck())
            return;

		if (!RL_Utils.IsDedicatedServer())
			GetGame().GetInputManager().RemoveActionListener("RL_TowAction", EActionTrigger.DOWN, OnTowAction);
		Print("Tow OnCompartmentLeft Pass");
		if( m_NwkCarMovement && m_RplComponent)
		{
			if(!m_RplComponent.IsProxy())
				m_NwkCarMovement.SetAllowance(true, 300, 0.003, 0.25, 0.3);
			else if(m_RplComponent.IsProxy() && m_RplComponent.IsOwner())
				m_NwkCarMovement.SetPrediction(true);
		}
		
		if (Replication.IsServer())
			return;
		
		BaseCompartmentSlot slot = mgr.FindCompartment(slotID, managerId);
		if (!slot)
			return;
		
		if (slot.Type() != PilotCompartmentSlot)
			return;			
		
		DestroyJoint();
		m_NwkMovement.EnableSimulation(false);
		m_NwkMovement.EnableInterpolation(true);
	}
    void SetNwk()
	{
		if (m_NwkCarMovement && m_RplComponent)
		{
			if(!m_RplComponent.IsProxy())
			{
				m_NwkCarMovement.SetAllowance(false, 100, 100, 100, 100);
			} else if(m_RplComponent.IsProxy() && m_RplComponent.IsOwner()) {
				m_NwkCarMovement.SetPrediction(false);
			} 
		}
	}
	
    void AddTowKeyBind()
	{
        

	}
    bool IsDriver(BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController || occupant != playerController.GetMainEntity())
			return false;

		BaseCompartmentSlot slot = mgr.FindCompartment(slotID, managerId);
		if (!slot)
			return false;
			
		if (slot.Type() != PilotCompartmentSlot)
			return false;

		return true;
	}
    private IEntity m_SearchResult;
	private RL_TowComponent m_TowAttachement;
	void OnTowAction()
	{
		Print("TOW ACTION");
		if(!this.IsTowOrTrailerTruck())
			return;
			
		Print("RL_TowAction PerformAction");
		if (this.GetAttachedEntity())
		{
			this.DetachEntity(true);
		} else {
			GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), 30, ProcessTowAttachement, null, EQueryEntitiesFlags.ALL);
			if (m_SearchResult)
			{
				Print("FOUND, SET ATTACHED");
				Print(m_SearchResult);
				this.SetAttachedEntity(m_SearchResult, true);
				//m_TowAttachement = RL_TowComponent.Cast(GetOwner().FindComponent(RL_TowComponent));
			
				return;
			}
		}
	}
	protected bool ProcessTowAttachement(IEntity ent)
	{
		array<Managed> comps = new array<Managed>;
		ent.FindComponents(RL_TowComponent, comps);
		
		if(!comps) return true; 
		
		foreach (Managed comp : comps)
		{
			Print("[RL_TowComponent] ProcessTowAttachement foreach");

			RL_TowComponent otherAttachment = RL_TowComponent.Cast(comp);
			if(!otherAttachment)
				continue;

			if (!otherAttachment || GetOwner() == ent || otherAttachment.GetAttachedEntity() )
				continue;
		
			if(!otherAttachment.IsTowOrTrailerTruck() && !this.IsTowOrTrailerTruck())
				continue;


			bool trailerAttaching = (otherAttachment.IsTrailerOrTrailerTruck() && this.IsTrailerOrTrailerTruck());
			if(otherAttachment.GetAttachPoint(trailerAttaching) == vector.Zero) continue;		
			vector point1 = otherAttachment.GetOwner().CoordToParent(otherAttachment.GetAttachPoint(trailerAttaching));
			vector point2 = this.GetOwner().CoordToParent(this.GetAttachPoint(trailerAttaching));

			float distance = vector.Distance(point1, point2);
			if (!trailerAttaching && distance > m_fMaxTowAttachDistance)
			{
				continue;
			}
			if (trailerAttaching && distance > m_fMaxTrailerAttachDistance)
			{
				continue;
			}

			m_SearchResult = ent;
			return false;
		}
		return true; 
	}
	void OnDamageStateChanged(EDamageState state)
	{
		Print("RL_TowComponent OnDamageStateChanged");
		if (GetAttachedEntity())
		{
			Print("RL_TowComponent OnDamageStateChanged DetachEntity");
			DetachEntity(true);
		}

	}
	override event protected void OnDelete(IEntity owner)
	{
		if (!GetGame().InPlayMode()) return;
		Print("RL_TowComponent OnDelete");
		if (GetAttachedEntity())
		{
			Print("RL_TowComponent OnDelete DetachEntity");
			DetachEntity(true);
		}
	}
	void SignalLandingGear(bool raise)
	{
        if(!GetAttachedEntity())
        {
            Print("SignalLandingGear called with no Attached Entity");
            return;
        }

		RL_TowComponent ownerComp = RL_TowComponent.Cast(GetOwner().FindComponent(RL_TowComponent));
		RL_TowComponent otherComp = RL_TowComponent.Cast(GetAttachedEntity().FindComponent(RL_TowComponent));
		RL_TowComponent trailerComp;

		if(ownerComp && ownerComp.GetVehicleType() == RL_TOW_VEHICLE_TYPE.Trailer)
			trailerComp = ownerComp;
		else if(otherComp && otherComp.GetVehicleType() == RL_TOW_VEHICLE_TYPE.Trailer)
			trailerComp = otherComp;

		if(!trailerComp) return;
		trailerComp.m_bIsLandingGearUp = raise;
		Replication.BumpMe();
	}
	bool m_bIsLandingGearUpLocal = false;
	override void EOnVisible(IEntity owner, int frameNumber)
	{
		if(m_bIsLandingGearUpLocal == m_bIsLandingGearUp)
			return;
		Print("---- OnLandingGearToggled");

		SignalsManagerComponent m_SignalManager = SignalsManagerComponent.Cast(GetOwner().FindComponent(SignalsManagerComponent));
		int signalIndex = m_SignalManager.FindSignal("LandingGearAction");
		if(signalIndex == -1) return;
		if (m_bIsLandingGearUp)
		{
			m_SignalManager.SetSignalValue(signalIndex, 1);	
		} else {
			m_SignalManager.SetSignalValue(signalIndex, 0);	
		}
		m_bIsLandingGearUpLocal = m_bIsLandingGearUp;
	}
}
