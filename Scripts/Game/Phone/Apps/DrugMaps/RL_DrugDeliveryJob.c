enum RL_DRUG_JOB_STATUS
{
	Available,
	Accepted,
	Completed,
	Failed,
	Expired,
	Declined
}

class RL_DrugDeliveryJob
{
	protected string m_sJobId;
	protected string m_sCharacterId;
	protected string m_sClientName;
	protected string m_sLocationId;
	protected vector m_vDeliveryLocation;
	protected EDrugs m_eDrugType;
	protected int m_iQuantity;
	protected int m_iBasePrice;
	protected float m_fPriceMultiplier;
	protected int m_iTotalPayment;
	protected int m_iTimeLimit;
	protected float m_fStartTime;
	protected RL_DRUG_JOB_STATUS m_eStatus;
	protected string m_sMarkerIcon;
	protected ref Color m_cMarkerColor;
		
	//-----------------------------------------------------------------------------------------------
	void RL_DrugDeliveryJob(string jobId, string characterId, string clientName, string locationId, vector deliveryPos, EDrugs drugType, int quantity, int basePrice, float priceMultiplier, int timeLimit, string markerIcon, Color markerColor)
	{
		m_sJobId = jobId;
		m_sCharacterId = characterId;
		m_sClientName = clientName;
		m_sLocationId = locationId;
		m_vDeliveryLocation = deliveryPos;
		m_eDrugType = drugType;
		m_iQuantity = quantity;
		m_iBasePrice = basePrice;
		m_fPriceMultiplier = priceMultiplier;
		m_iTotalPayment = Math.Round(basePrice * quantity * priceMultiplier);
		m_iTimeLimit = timeLimit;
		m_eStatus = RL_DRUG_JOB_STATUS.Available;
		m_sMarkerIcon = markerIcon;
		m_cMarkerColor = markerColor;
		m_fStartTime = 0;
	}
	
	//-----------------------------------------------------------------------------------------------
	string GetJobId() { return m_sJobId; }
	string GetCharacterId() { return m_sCharacterId; }
	string GetClientName() { return m_sClientName; }
	string GetLocationId() { return m_sLocationId; }
	vector GetDeliveryLocation() { return m_vDeliveryLocation; }
	EDrugs GetDrugType() { return m_eDrugType; }
	int GetQuantity() { return m_iQuantity; }
	int GetBasePrice() { return m_iBasePrice; }
	float GetPriceMultiplier() { return m_fPriceMultiplier; }
	int GetTotalPayment() { return m_iTotalPayment; }
	int GetTimeLimit() { return m_iTimeLimit; }
	RL_DRUG_JOB_STATUS GetStatus() { return m_eStatus; }
	string GetMarkerIcon() { return m_sMarkerIcon; }
	Color GetMarkerColor() { return m_cMarkerColor; }
	
	//-----------------------------------------------------------------------------------------------
	void SetStatus(RL_DRUG_JOB_STATUS status) { m_eStatus = status; }
	
	//-----------------------------------------------------------------------------------------------
	void AcceptJob()
	{
		m_eStatus = RL_DRUG_JOB_STATUS.Accepted;
		m_fStartTime = GetGame().GetWorld().GetWorldTime() / 1000;
	}
		
	//-----------------------------------------------------------------------------------------------
	void DeclineJob()
	{
		m_eStatus = RL_DRUG_JOB_STATUS.Declined;
	}
		
	//-----------------------------------------------------------------------------------------------
	float GetElapsedTime()
	{
		if (m_eStatus != RL_DRUG_JOB_STATUS.Accepted || m_fStartTime <= 0)
			return 0;
			
		float currentTime = GetGame().GetWorld().GetWorldTime() / 1000;
		return currentTime - m_fStartTime;
	}
		
	//-----------------------------------------------------------------------------------------------
	float GetRemainingTime()
	{
		if (m_eStatus != RL_DRUG_JOB_STATUS.Accepted)
			return m_iTimeLimit;
		
		float elapsed = GetElapsedTime();
		return Math.Max(0, m_iTimeLimit - elapsed);
	}
		
	//-----------------------------------------------------------------------------------------------
	bool IsExpired()
	{
		return GetRemainingTime() <= 0 && m_eStatus == RL_DRUG_JOB_STATUS.Accepted;
	}
		
	//-----------------------------------------------------------------------------------------------
	string GetStatusString()
	{
		switch (m_eStatus)
		{
			case RL_DRUG_JOB_STATUS.Available: return "Available";
			case RL_DRUG_JOB_STATUS.Accepted: return "Accepted";
			case RL_DRUG_JOB_STATUS.Completed: return "Completed";
			case RL_DRUG_JOB_STATUS.Failed: return "Failed";
			case RL_DRUG_JOB_STATUS.Expired: return "Expired";
			case RL_DRUG_JOB_STATUS.Declined: return "Declined";
		}
		return "Unknown";
	}
		
	//-----------------------------------------------------------------------------------------------
	string GetJobDescription()
	{
		string description = string.Format("Deliver %1x %2 to %3", m_iQuantity, GetDrugName(m_eDrugType), m_sClientName);
		return description;
	}
		
	//-----------------------------------------------------------------------------------------------
	string GetJobSummary()
	{
		int timeMinutes = m_iTimeLimit / 60;
		int timeSeconds = m_iTimeLimit % 60;
		string timeDisplay;
		if (timeMinutes > 0)
			timeDisplay = string.Format("%1m %2s", timeMinutes, timeSeconds);
		else
			timeDisplay = string.Format("%1s", timeSeconds);
		
		return string.Format("Client: %1\nDrug: %2\nQuantity: %3x\nPayment: %4\nTime Limit: %5", 
			m_sClientName, 
			GetDrugName(m_eDrugType), 
			m_iQuantity,
			RL_Utils.FormatMoney(m_iTotalPayment), 
			timeDisplay);
	}
} 