modded class SCR_ChimeraCharacter
{
	protected ref map<string, IEntity> m_mDrugDeliveryMarkers;
	protected ref map<string, RL_MapMarker> m_mCachedMarkerComponents;
	protected IEntity m_CachedLocalPlayer;
	protected bool m_bIsLocalPlayer;
	protected bool m_bCacheValid;
	
	//------------------------------------------------------------------------------------------------
	void OnDrugDeliveryJobsUpdated()
	{
		if (Replication.IsServer())
			return;
			
		InvalidateCache();
		UpdateDrugDeliveryMarkers();
	}

	//------------------------------------------------------------------------------------------------
	private void InvalidateCache()
	{
		m_bCacheValid = false;
	}

	//------------------------------------------------------------------------------------------------
	private void ValidateCache()
	{
		if (m_bCacheValid)
			return;
			
		m_CachedLocalPlayer = GetGame().GetPlayerController().GetControlledEntity();
		m_bIsLocalPlayer = (this == m_CachedLocalPlayer);
		m_bCacheValid = true;
	}

	//------------------------------------------------------------------------------------------------
	void AcceptDrugDeliveryJob(string jobId)
	{
		RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
		if (manager)
			manager.AcceptJob(jobId);
	}
		
	//------------------------------------------------------------------------------------------------
	void DeclineDrugDeliveryJob(string jobId)
	{
		RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
		if (manager)
			manager.DeclineJob(jobId);
	}
		
	//------------------------------------------------------------------------------------------------
	void CompleteDrugDeliveryJob(string jobId)
	{
		RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
		if (!manager)
			return;
		
		RL_DrugDeliveryJob job = manager.FindJobById(jobId);
		if (!job)
			return;
		
		manager.CompleteDelivery(this, job);
	}

	//------------------------------------------------------------------------------------------------
	array<ref RL_DrugDeliveryJob> GetDrugDeliveryJobs()
	{
		RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
		if (manager)
			return manager.GetPlayerJobs();
		
		return new array<ref RL_DrugDeliveryJob>;
	}

	//------------------------------------------------------------------------------------------------
	array<ref RL_DrugDeliveryJob> GetJobsByStatus(RL_DRUG_JOB_STATUS status)
	{
		array<ref RL_DrugDeliveryJob> filteredJobs = new array<ref RL_DrugDeliveryJob>;
		array<ref RL_DrugDeliveryJob> allJobs = GetDrugDeliveryJobs();
		
		foreach (RL_DrugDeliveryJob job : allJobs)
		{
			if (job && job.GetStatus() == status)
				filteredJobs.Insert(job);
		}
		
		return filteredJobs;
	}

	//------------------------------------------------------------------------------------------------
	void UpdateDrugDeliveryMarkers()
	{
		if (Replication.IsServer())
			return;
			
		ValidateCache();
		if (!m_bIsLocalPlayer)
			return;
		
		EnsureMarkerMapsExist();
		
		array<ref RL_DrugDeliveryJob> acceptedJobs = GetJobsByStatus(RL_DRUG_JOB_STATUS.Accepted);
		ref set<string> activeJobIds = new set<string>;
		
		if (!acceptedJobs.IsEmpty())
			activeJobIds.Reserve(acceptedJobs.Count());
		
		int jobCount = acceptedJobs.Count();
		for (int i = 0; i < jobCount; i++)
		{
			RL_DrugDeliveryJob job = acceptedJobs[i];
			if (!job)
				continue;
				
			string jobId = job.GetJobId();
			activeJobIds.Insert(jobId);
			RL_DRUG_JOB_STATUS jobStatus = job.GetStatus();
			if (job.IsExpired() || jobStatus == RL_DRUG_JOB_STATUS.Completed)
			{
				RemoveDrugDeliveryMarker(jobId);
				continue;
			}
			
			if (!m_mDrugDeliveryMarkers.Contains(jobId))
				CreateDrugDeliveryMarker(job);
			else
				UpdateDrugDeliveryMarker(job);
		}
		
		CleanupObsoleteMarkers(activeJobIds);
	}

	//------------------------------------------------------------------------------------------------
	private void EnsureMarkerMapsExist()
	{
		if (!m_mDrugDeliveryMarkers)
			m_mDrugDeliveryMarkers = new map<string, IEntity>;
		if (!m_mCachedMarkerComponents)
			m_mCachedMarkerComponents = new map<string, RL_MapMarker>;
	}

	//------------------------------------------------------------------------------------------------
	private void CleanupObsoleteMarkers(set<string> activeJobIds)
	{
		if (m_mDrugDeliveryMarkers.IsEmpty())
			return;
			
		array<string> keysToRemove = new array<string>;
		int markerCount = m_mDrugDeliveryMarkers.Count();
		keysToRemove.Reserve(markerCount);
		for (int i = 0; i < markerCount; i++)
		{
			string markerJobId = m_mDrugDeliveryMarkers.GetKey(i);
			if (!activeJobIds.Contains(markerJobId))
				keysToRemove.Insert(markerJobId);
		}
		int removeCount = keysToRemove.Count();
		for (int i = 0; i < removeCount; i++)
		{
			RemoveDrugDeliveryMarker(keysToRemove[i]);
		}
	}

	//------------------------------------------------------------------------------------------------
	void CreateDrugDeliveryMarker(RL_DrugDeliveryJob job)
	{
		if (Replication.IsServer() || !job)
			return;
		
		string jobId = job.GetJobId();
		vector deliveryPos = job.GetDeliveryLocation();
		string markerText = GetDrugDeliveryMarkerText(job);
		
		IEntity markerEntity = RL_MapUtils.CreateMarkerFromPrefabClient(
			deliveryPos,
			"{F4F27AA841E772C2}Prefabs/World/Locations/Common/RL_MapMarkerDrugDelivery.et",
			markerText
		);
		
		if (!markerEntity)
			return;
	
		RL_MapMarker markerComponent = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
		if (markerComponent)
		{
			Color markerColor = job.GetMarkerColor();
			string markerIcon = job.GetMarkerIcon();
			markerComponent.SetMarkerProperties(markerText, markerIcon, markerColor);
			markerComponent.SetVisible(true);
			EnsureMarkerMapsExist();
			m_mCachedMarkerComponents.Set(jobId, markerComponent);
		}
		
		m_mDrugDeliveryMarkers.Set(jobId, markerEntity);
	}

	//------------------------------------------------------------------------------------------------
	void UpdateDrugDeliveryMarker(RL_DrugDeliveryJob job)
	{
		if (Replication.IsServer() || !job)
			return;
		
		string jobId = job.GetJobId();
		if (!m_mDrugDeliveryMarkers || !m_mDrugDeliveryMarkers.Contains(jobId))
			return;
		RL_MapMarker markerComponent;
		if (m_mCachedMarkerComponents && m_mCachedMarkerComponents.Contains(jobId))
		{
			markerComponent = m_mCachedMarkerComponents.Get(jobId);
		}
		else
		{
			IEntity markerEntity = m_mDrugDeliveryMarkers.Get(jobId);
			if (!markerEntity)
				return;
			
			markerComponent = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
			if (markerComponent)
			{
				EnsureMarkerMapsExist();
				m_mCachedMarkerComponents.Set(jobId, markerComponent);
			}
		}
		
		if (!markerComponent)
			return;
		string markerText = GetDrugDeliveryMarkerText(job);
		markerComponent.SetText(markerText);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveDrugDeliveryMarker(string jobId)
	{
		if (Replication.IsServer() || !m_mDrugDeliveryMarkers || !m_mDrugDeliveryMarkers.Contains(jobId))
			return;
		
		IEntity markerEntity = m_mDrugDeliveryMarkers.Get(jobId);
		if (markerEntity)
			RL_MapUtils.DeleteMarkerEntity(markerEntity);
		
		m_mDrugDeliveryMarkers.Remove(jobId);
		if (m_mCachedMarkerComponents && m_mCachedMarkerComponents.Contains(jobId))
			m_mCachedMarkerComponents.Remove(jobId);
	}

	//------------------------------------------------------------------------------------------------
	void CleanupAllDrugDeliveryMarkers()
	{
		if (Replication.IsServer() || !m_mDrugDeliveryMarkers || m_mDrugDeliveryMarkers.IsEmpty())
			return;
		
		ValidateCache();
		if (!m_bIsLocalPlayer)
			return;
		
		int markerCount = m_mDrugDeliveryMarkers.Count();
		for (int i = 0; i < markerCount; i++)
		{
			IEntity markerEntity = m_mDrugDeliveryMarkers.GetElement(i);
			if (markerEntity)
				RL_MapUtils.DeleteMarkerEntity(markerEntity);
		}
		m_mDrugDeliveryMarkers.Clear();
		if (m_mCachedMarkerComponents)
			m_mCachedMarkerComponents.Clear();
	}

	//------------------------------------------------------------------------------------------------
	string GetDrugDeliveryMarkerText(RL_DrugDeliveryJob job)
	{
		if (!job)
			return "Drug Delivery";
		
		string drugName = GetDrugName(job.GetDrugType());
		int quantity = job.GetQuantity();
		string clientName = job.GetClientName();
		
		return string.Format("DELIVER %1x %2\nto %3", quantity, drugName, clientName);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!Replication.IsServer())
		{
			EnsureMarkerMapsExist();
			InvalidateCache();
		}
	}
}