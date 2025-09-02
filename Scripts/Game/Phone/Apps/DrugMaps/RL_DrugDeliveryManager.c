class RL_DrugDeliveryManager
{
	private static ref RL_DrugDeliveryManager s_Instance;
	private ref array<RL_DrugMapsDeliverComponent> m_aDeliveryLocations;
	private ref array<ref RL_DrugDeliveryJob> m_aPlayerJobs;
	private int m_iJobIdCounter;
	private bool m_bInitialized = false;
	
	private static const int ASSIGNMENT_INTERVAL = 180;
	private static const int DELIVERY_TIME_LIMIT = 600;
	private static const int MIN_JOBS_PER_PLAYER = 4;
	private static const int MAX_JOBS_PER_PLAYER = 8;
	
	//------------------------------------------------------------------------------------------------
	static RL_DrugDeliveryManager GetInstance()
	{
		if (Replication.IsServer())
			return null;
			
		if (!s_Instance)
			s_Instance = new RL_DrugDeliveryManager();

		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void RL_DrugDeliveryManager()
	{
		if (Replication.IsServer())
			return;
		
		m_aDeliveryLocations = new array<RL_DrugMapsDeliverComponent>;
		m_aPlayerJobs = new array<ref RL_DrugDeliveryJob>;
		m_iJobIdCounter = 0;
		
		GetGame().GetCallqueue().CallLater(Initialize, 10000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	private void Initialize()
	{
		if (m_bInitialized || Replication.IsServer())
			return;
			
		m_bInitialized = true;
		
		GetGame().GetCallqueue().CallLater(PeriodicJobAssignment, ASSIGNMENT_INTERVAL * 1000, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterDeliveryLocation(RL_DrugMapsDeliverComponent location)
	{
		if (Replication.IsServer() || !location || m_aDeliveryLocations.Contains(location))
			return;
		m_aDeliveryLocations.Insert(location);
		Print(string.Format("[DrugMaps] Registered delivery location, total locations: %1", m_aDeliveryLocations.Count()));
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterDeliveryLocation(RL_DrugMapsDeliverComponent location)
	{
		if (Replication.IsServer() || !location)
			return;
		int removed = m_aDeliveryLocations.RemoveItem(location);
	}
	
	//------------------------------------------------------------------------------------------------
	private array<RL_DrugMapsDeliverComponent> GetMultipleRandomLocations(int count, set<string> excludedLocationIds = null)
	{
		array<RL_DrugMapsDeliverComponent> result = new array<RL_DrugMapsDeliverComponent>;
		
		if (m_aDeliveryLocations.Count() == 0 || count <= 0)
			return result;

		array<RL_DrugMapsDeliverComponent> availableLocations = new array<RL_DrugMapsDeliverComponent>;
		foreach (RL_DrugMapsDeliverComponent location : m_aDeliveryLocations)
		{
			if (!location)
				continue;
			
			if (excludedLocationIds && excludedLocationIds.Contains(location.GetLocationId()))
				continue;
			
			availableLocations.Insert(location);
		}
		
		if (availableLocations.IsEmpty())
			return result;
		
		int locationsToSelect = Math.Min(count, availableLocations.Count());
		for (int i = 0; i < locationsToSelect; i++)
		{
			int randomIndex = Math.RandomIntInclusive(i, availableLocations.Count() - 1);
			RL_DrugMapsDeliverComponent temp = availableLocations[i];
			availableLocations[i] = availableLocations[randomIndex];
			availableLocations[randomIndex] = temp;
			result.Insert(availableLocations[i]);
		}
		
		return result;
	}

	//------------------------------------------------------------------------------------------------
	void PeriodicJobAssignment()
	{
		Print("[DrugMaps] PeriodicJobAssignment");
		SCR_ChimeraCharacter localCharacter = RL_Utils.GetLocalCharacter();
		if (!localCharacter)
			return;
		ProcessPlayerJobs(localCharacter);
	}
	
	//------------------------------------------------------------------------------------------------
	private void ProcessPlayerJobs(SCR_ChimeraCharacter character)
	{
		CleanupJobs();
		int currentJobCount = CountAvailableJobs();
		int maxNewJobs = MAX_JOBS_PER_PLAYER - currentJobCount;
		if (maxNewJobs <= 0)
			return;
		
		CreateJobs(character, maxNewJobs);
		NotifyJobsUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	private void CleanupJobs()
	{
		int removedCount = 0;
		for (int i = m_aPlayerJobs.Count() - 1; i >= 0; i--)
		{
			RL_DrugDeliveryJob job = m_aPlayerJobs[i];
			if (!job)
			{
				m_aPlayerJobs.Remove(i);
				removedCount++;
				continue;
			}
			
			if (job.IsExpired())
			{
				job.SetStatus(RL_DRUG_JOB_STATUS.Expired);
				m_aPlayerJobs.Remove(i);
				removedCount++;
				continue;
			}
			
			RL_DRUG_JOB_STATUS status = job.GetStatus();
			if (status == RL_DRUG_JOB_STATUS.Completed || 
				status == RL_DRUG_JOB_STATUS.Failed || 
				status == RL_DRUG_JOB_STATUS.Declined ||
				status == RL_DRUG_JOB_STATUS.Expired)
			{
				m_aPlayerJobs.Remove(i);
				removedCount++;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private void CreateJobs(SCR_ChimeraCharacter character, int maxNewJobs)
	{
		int newJobCount = Math.RandomIntInclusive(MIN_JOBS_PER_PLAYER, Math.Min(MAX_JOBS_PER_PLAYER, maxNewJobs));
		ref set<string> excludedLocations = GetActiveLocationIds();
		
		array<RL_DrugMapsDeliverComponent> selectedLocations = GetMultipleRandomLocations(newJobCount, excludedLocations);
		int createdJobs = 0;
		for (int i = 0; i < selectedLocations.Count(); i++)
		{
			RL_DrugDeliveryJob newJob = CreateJobFromLocation(character, selectedLocations[i]);
			if (newJob)
			{
				m_aPlayerJobs.Insert(newJob);
				createdJobs++;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private RL_DrugDeliveryJob CreateJobFromLocation(SCR_ChimeraCharacter character, RL_DrugMapsDeliverComponent selectedLocation)
	{
		if (!selectedLocation)
			return null;
		
		array<int> availableDrugTypes = selectedLocation.GetAvailableDrugTypes();
		if (availableDrugTypes.IsEmpty())
			return null;

		EDrugs selectedDrug = availableDrugTypes.GetRandomElement();
		string jobId = GenerateJobId(character.GetCharacterId());
		string clientName = selectedLocation.GetClientName();
		if (clientName.IsEmpty())
			clientName = RL_Utils.GetRandomName();
		
		vector deliveryPos = selectedLocation.GetOwner().GetOrigin();
		int quantity = Math.RandomIntInclusive(selectedLocation.GetMinQuantity(), selectedLocation.GetMaxQuantity());
		int basePrice = GetDrugPricePerUnit(selectedDrug);
		float priceMultiplier = selectedLocation.GetPriceMultiplier();
		string markerIcon = selectedLocation.GetMarkerIcon();
		Color markerColor = selectedLocation.GetMarkerColor();
		
		return new RL_DrugDeliveryJob(
			jobId, character.GetCharacterId(), clientName, selectedLocation.GetLocationId(),
			deliveryPos, selectedDrug, quantity, basePrice, priceMultiplier,
			DELIVERY_TIME_LIMIT, markerIcon, markerColor
		);
	}
	
	//------------------------------------------------------------------------------------------------
	private string GenerateJobId(string characterId)
	{
		m_iJobIdCounter++;
		return string.Format("DRUG_JOB_%1_%2", characterId, m_iJobIdCounter);
	}
	
	//------------------------------------------------------------------------------------------------
	private bool ValidateJobAction(string jobId, string action, out RL_DrugDeliveryJob job)
	{
		job = FindJobById(jobId);
		if (!job)
			return false;
		
		if (job.GetStatus() != RL_DRUG_JOB_STATUS.Available)
			return false;
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	void AcceptJob(string jobId)
	{
		RL_DrugDeliveryJob job;
		if (!ValidateJobAction(jobId, "accept", job))
			return;
		
		job.AcceptJob();
		NotifyJobsUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	void DeclineJob(string jobId)
	{
		RL_DrugDeliveryJob job;
		if (!ValidateJobAction(jobId, "decline", job))
			return;
		
		job.DeclineJob();
		NotifyJobsUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	void CompleteDelivery(SCR_ChimeraCharacter character, RL_DrugDeliveryJob job)
	{
		if (!character || !job)
			return;
		
		RL_DrugDeliveryJob playerJob = FindJobById(job.GetJobId());
		if (!IsValidJobForCompletion(playerJob, job))
			return;
		
		if (job.IsExpired())
		{
			HandleExpiredJob(character, job);
			return;
		}
		
		int qualityAdjustedPayment;
		if (!ProcessDrugRemoval(character, job, qualityAdjustedPayment))
			return;

		CompleteJobAndPayPlayer(character, job, qualityAdjustedPayment);
		RemoveJobFromPlayer(job);
		NotifyJobsUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	private bool IsValidJobForCompletion(RL_DrugDeliveryJob playerJob, RL_DrugDeliveryJob job)
	{
		return playerJob && playerJob == job && job.GetStatus() == RL_DRUG_JOB_STATUS.Accepted;
	}
	
	//------------------------------------------------------------------------------------------------
	private void HandleExpiredJob(SCR_ChimeraCharacter character, RL_DrugDeliveryJob job)
	{
		job.SetStatus(RL_DRUG_JOB_STATUS.Expired);
		character.Notify("Your delivery job has expired!", "DRUG DELIVERY");
		NotifyJobsUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	RL_DrugDeliveryJob FindJobById(string jobId)
	{
		foreach (RL_DrugDeliveryJob job : m_aPlayerJobs)
		{
			if (job && job.GetJobId() == jobId)
				return job;
		}
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	private ref set<string> GetActiveLocationIds()
	{
		ref set<string> activeLocationIds = new set<string>;
		
		foreach (RL_DrugDeliveryJob job : m_aPlayerJobs)
		{
			if (job && job.GetStatus() == RL_DRUG_JOB_STATUS.Accepted)
				activeLocationIds.Insert(job.GetLocationId());
		}
		
		return activeLocationIds;
	}
	
	//------------------------------------------------------------------------------------------------
	private int CountAvailableJobs()
	{
		int count = 0;
		foreach (RL_DrugDeliveryJob job : m_aPlayerJobs)
		{
			if (job && job.GetStatus() == RL_DRUG_JOB_STATUS.Available)
				count++;
		}
		return count;
	}
	
	//------------------------------------------------------------------------------------------------
	private void RemoveJobFromPlayer(RL_DrugDeliveryJob job)
	{
		m_aPlayerJobs.RemoveItem(job);
	}
	
	//------------------------------------------------------------------------------------------------
	private bool ProcessDrugRemoval(SCR_ChimeraCharacter character, RL_DrugDeliveryJob job, out int qualityAdjustedPayment)
	{
		qualityAdjustedPayment = 0;
		
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		if (!inventoryManager)
			return false;
		
		string drugPrefab = GetDrugPrefab(job.GetDrugType());
		if (drugPrefab.IsEmpty())
			return false;

		int amountInInventory = RL_InventoryUtils.GetAmount(inventoryManager, drugPrefab);
		if (amountInInventory < job.GetQuantity())
		{
			character.Notify(string.Format("You no longer have %1x %2 to deliver!", 
				job.GetQuantity(), GetDrugName(job.GetDrugType())), "DRUG DELIVERY");
			return false;
		}
		
		qualityAdjustedPayment = CalculateQualityAdjustedPayment(inventoryManager, job, drugPrefab);
		int amountRemoved = RL_InventoryUtils.RemoveAmount(inventoryManager, drugPrefab, job.GetQuantity());
		if (amountRemoved < job.GetQuantity())
		{
			character.Notify("Failed to remove drugs from inventory.", "DRUG DELIVERY");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	private int CalculateQualityAdjustedPayment(InventoryStorageManagerComponent inventoryManager, RL_DrugDeliveryJob job, string drugPrefab)
	{
		array<IEntity> drugItems = RL_InventoryUtils.FindItemsByPrefab(inventoryManager, drugPrefab);
		if (!drugItems || drugItems.IsEmpty())
			return job.GetTotalPayment();
		
		float basePricePerItem = job.GetBasePrice() * job.GetPriceMultiplier();
		int qualityAdjustedPayment = 0;
		int itemsToProcess = Math.Min(job.GetQuantity(), drugItems.Count());
		
		for (int i = 0; i < itemsToProcess; i++)
		{
			qualityAdjustedPayment += RL_ShopUtils.CalculateQualityAdjustedPrice(basePricePerItem, drugItems[i]);
		}
		
		if (qualityAdjustedPayment > 0)
			return qualityAdjustedPayment;
		else
			return job.GetTotalPayment();
	}
	
	//------------------------------------------------------------------------------------------------
	private void CompleteJobAndPayPlayer(SCR_ChimeraCharacter character, RL_DrugDeliveryJob job, int qualityAdjustedPayment)
	{
		job.SetStatus(RL_DRUG_JOB_STATUS.Completed);
		character.TransactMoney(qualityAdjustedPayment, 0);
		LogPlayerTransaction(character, job, qualityAdjustedPayment);
		string timeDisplay = RL_Utils.FormatTimeDisplay(job.GetElapsedTime());
		string completionMessage = string.Format("Drug Delivery Completed!\n\nClient: %1\nDelivered: %2x %3\nTime: %4\nPayment: %5", 
			job.GetClientName(), job.GetQuantity(), GetDrugName(job.GetDrugType()),
			timeDisplay, RL_Utils.FormatMoney(qualityAdjustedPayment));
		
		character.Notify(completionMessage, "DRUG DELIVERY");
	}
	
	//------------------------------------------------------------------------------------------------
	private void LogPlayerTransaction(SCR_ChimeraCharacter character, RL_DrugDeliveryJob job, int qualityAdjustedPayment)
	{
		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("location", character.GetOrigin().ToString());
		jsonData.WriteValue("prefab", GetDrugPrefab(job.GetDrugType()));
		jsonData.WriteValue("sellPriceEach", qualityAdjustedPayment / job.GetQuantity());
		jsonData.WriteValue("quantity", job.GetQuantity());
		jsonData.WriteValue("totalSellPrice", qualityAdjustedPayment);
		jsonData.WriteValue("shop", "drugMaps");
		character.PlayerLog("Shop Sell", jsonData.ExportToString());
	}
	
	//------------------------------------------------------------------------------------------------
	private void NotifyJobsUpdated()
	{
		SCR_ChimeraCharacter localCharacter = RL_Utils.GetLocalCharacter();
		if (!localCharacter)
			return;
		localCharacter.OnDrugDeliveryJobsUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_DrugDeliveryJob> GetPlayerJobs()
	{
		return m_aPlayerJobs;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetDeliveryLocationCount()
	{
		return m_aDeliveryLocations.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsInitialized()
	{
		return m_bInitialized;
	}
}