class RL_EscortAction : ScriptedUserAction
{
	SCR_ChimeraCharacter m_ownerCharacter;
	
	BaseWorld m_world;
	SCR_ChimeraCharacter m_userCharacter;
	
	bool m_bStopInProgress;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		if(!m_ownerCharacter || m_ownerCharacter.IsSpamming()) return;
		//CacheComponents(pUserEntity);
		if(m_ownerCharacter.GetEscortState())
		{
			StopEscort();
		} else {
			StartEscort(pUserEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void StartEscort(IEntity pUserEntity)
	{	
		if (m_ownerCharacter.GetEscortState())
			return;
		
		if (Replication.IsServer())
		{
			m_ownerCharacter.SetEscortedState(true);
	      	Resource resource = Resource.Load("{70BE30FAFE3B319A}Prefabs/Drag/RL_EscortVehicle.et");
			EntitySpawnParams params = new EntitySpawnParams();
			vector angles = m_ownerCharacter.GetYawPitchRoll();
			angles[1] = 0;
			angles[2] = 0;
			Math3D.AnglesToMatrix(angles, params.Transform);
			params.Transform[3] = m_ownerCharacter.GetOrigin();
	      	RL_EscortVehicle escortVehicle = RL_EscortVehicle.Cast(GetGame().SpawnEntityPrefab(resource, m_world, params));
			if(!escortVehicle)
			{
				Print("Escort vehicle failed to spawn");
				return;
			}
			escortVehicle.InitServer(pUserEntity, m_ownerCharacter);
			m_ownerCharacter.SetEscortVehicle(escortVehicle);

		}
		
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		StopEscort();
	}
	
	override void OnRejected(IEntity pUserEntity)
	{
		StopEscort();
	}
	
	//------------------------------------------------------------------------------------------------
	void StopEscort()
	{
		if (m_bStopInProgress)
			return;
		if (!m_ownerCharacter || !m_ownerCharacter.GetEscortState())
			return;
		m_bStopInProgress = true;
		
		GetGame().GetCallqueue().Call(ResetEscort, 500);
		
		if (!Replication.IsServer())
			return;
		
		if (m_ownerCharacter.GetEscortVehicle())
		{
			m_ownerCharacter.GetEscortVehicle().StopEscort();
			m_ownerCharacter.SetEscortVehicle(null);
		}
	}
	
	void ResetEscort()
	{
		m_bStopInProgress = false;
		
		if (Replication.IsServer())
			m_ownerCharacter.SetEscortedState(false);
	}
	
	void CacheComponents(IEntity user)
	{
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (m_userCharacter != userCharacter)
		{
			m_userCharacter = userCharacter;
			m_world = userCharacter.GetWorld();
		}
	}
	override bool GetActionNameScript(out string outName)
    {

		if (m_ownerCharacter.GetEscortState())
			outName = "Stop Escorting";
		else
			outName = "Start Escorting";
		
        return true;
    }
	override bool CanBeShownScript(IEntity user)
 	{
		//CacheComponents(user);
		return CanBePerformedScript(user);
	}
	override bool CanBePerformedScript(IEntity user)
 	{
		m_userCharacter = SCR_ChimeraCharacter.Cast(user);
		m_world = m_userCharacter.GetWorld();
		
		bool baseConditions = (
			m_userCharacter != m_ownerCharacter &&
			m_ownerCharacter.GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE
		);
		
		if (!baseConditions)
			return false;
		
		// Police can escort handcuffed people
		if (m_userCharacter.IsPolice() && m_ownerCharacter.IsHandcuffed())
			return true;
			
		// Cius can escort ziptied people
		if (m_ownerCharacter.IsZiptied())
			return true;
		
		return false;
 	}
	override bool CanBroadcastScript() { return false; };

}