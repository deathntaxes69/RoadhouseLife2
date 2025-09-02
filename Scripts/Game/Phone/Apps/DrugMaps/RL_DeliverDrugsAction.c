class RL_DeliverDrugsAction : ScriptedUserAction
{
	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_character;

	//-----------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerEntity = pOwnerEntity;
	}

	//-----------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!RL_Utils.CanPerformAction(pUserEntity))
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
				character.Notify("Cannot perform action right now.", "DRUG DELIVERY");
			return;
		}

		m_character = SCR_ChimeraCharacter.Cast(pUserEntity);
		
		if (!m_character)
			return;

		RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
		if (!manager)
		{
			m_character.Notify("Drug delivery system is currently unavailable.", "DRUG DELIVERY");
			return;
		}

		RL_DrugDeliveryJob activeJob = FindActiveJobForLocation(pOwnerEntity);
		if (!activeJob)
		{
			m_character.Notify("No active drug delivery for this location.", "DRUG DELIVERY");
			return;
		}

		RL_DrugMapsDeliverComponent deliverComponent = RL_DrugMapsDeliverComponent.Cast(pOwnerEntity.FindComponent(RL_DrugMapsDeliverComponent));
		if (!deliverComponent)
		{
			m_character.Notify("Not a valid delivery location.", "DRUG DELIVERY");
			return;
		}

		float playerDistance = vector.Distance(m_character.GetOrigin(), pOwnerEntity.GetOrigin());
		if (playerDistance > 5.0)
		{
			m_character.Notify("Too far from delivery location.", "DRUG DELIVERY");
			return;
		}

		if (!HasRequiredDrugs(activeJob))
		{
			string drugName = GetDrugName(activeJob.GetDrugType());
			m_character.Notify(string.Format("You don't have %1x %2 to deliver.", activeJob.GetQuantity(), drugName), "DRUG DELIVERY");
			return;
		}

		int randomChance = Math.RandomInt(1, 10);
		if (randomChance == 1)
		{
			string drugName = GetDrugName(activeJob.GetDrugType());
			string messageTitle = "SUSPICIOUS ACTIVITY REPORTED";
			string message = string.Format("Civilian reports potential drug dealing activity involving %1.", drugName);
			string markerText = "Drug Activity";
			m_character.SendEmergencyToPolice(messageTitle, message, markerText);
		}

		m_character.CompleteDrugDeliveryJob(activeJob.GetJobId());
		m_character.AddXp(RL_SkillsCategory.DRUG_DEALING, 25);
	}

	//-----------------------------------------------------------------------------------------------
	RL_DrugDeliveryJob FindActiveJobForLocation(IEntity locationEntity)
	{
		if (!m_character)
			return null;
		
		RL_DrugMapsDeliverComponent deliverComponent = RL_DrugMapsDeliverComponent.Cast(locationEntity.FindComponent(RL_DrugMapsDeliverComponent));
		if (!deliverComponent)
			return null;
		
		string locationId = deliverComponent.GetLocationId();
		if (locationId.IsEmpty())
			return null;
		
		array<ref RL_DrugDeliveryJob> acceptedJobs = m_character.GetJobsByStatus(RL_DRUG_JOB_STATUS.Accepted);
		
		foreach (RL_DrugDeliveryJob job : acceptedJobs)
		{
			if (!job)
				continue;
			
			if (job.GetLocationId() == locationId)
				return job;
		}
		
		return null;
	}
	
	//-----------------------------------------------------------------------------------------------
	bool HasRequiredDrugs(RL_DrugDeliveryJob job)
	{
		if (!job || !m_character)
			return false;
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(m_character);
		if (!inventoryManager)
			return false;
		
		string drugPrefab = GetDrugPrefab(job.GetDrugType());
		if (drugPrefab.IsEmpty())
			return false;
		
		int amountInInventory = RL_InventoryUtils.GetAmount(inventoryManager, drugPrefab);
		bool hasEnough = (amountInInventory >= job.GetQuantity());
		
		return hasEnough;
	}

	//-----------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_character)
		{
			RL_DrugDeliveryJob activeJob = FindActiveJobForLocation(m_ownerEntity);
			if (activeJob)
			{
				string drugName = GetDrugName(activeJob.GetDrugType());
				outName = string.Format("Deliver %1x %2", activeJob.GetQuantity(), drugName);
				return true;
			}
		}
		
		outName = "Deliver Drugs";
		return true;
	}
		
	//-----------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		m_character = SCR_ChimeraCharacter.Cast(user);
		if (!m_character)
			return false;
		
		RL_DrugDeliveryJob activeJob = FindActiveJobForLocation(m_ownerEntity);
		bool canShow = (activeJob != null);
		
		string activeJobId = "none";
		if (activeJob)
			activeJobId = activeJob.GetJobId();
		
		return canShow;
	}
		
	//-----------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return CanBeShownScript(user);
	}
		
	//-----------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
		
	//-----------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
} 