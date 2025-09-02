sealed class RL_DrugMapsWidget : RL_AppBaseWidget
{
    protected Widget m_contentLayout;
    private const string m_drugMapsLayout = "{CE9558ADD956BB36}UI/Layouts/Phone/Apps/DrugMaps/DrugMaps.layout";
	private const string m_sofferList = "offerList";
	private const string m_sDeclineButton = "DeclineButton";
	private const string m_sAcceptButton = "AcceptButton";
	
	protected SCR_ListBoxComponent m_offerList;
	protected SCR_InputButtonComponent m_DeclineButton;
	protected SCR_InputButtonComponent m_AcceptButton;
	
	protected ref array<ref RL_DrugDeliveryJob> m_aAvailableJobs;
	protected ref array<ref RL_DrugDeliveryJob> m_aAcceptedJobs;
	protected ref array<ref RL_DrugDeliveryJob> m_aAllJobs;
	protected RL_DrugDeliveryJob m_selectedJob;
	
	private const string m_sElementLayout = "{833CCBF08CB65886}UI/Layouts/Phone/Apps/DrugMaps/DrugMapsDealElement.layout";
	
	//-----------------------------------------------------------------------------------------------
	private bool ValidateInitialization()
	{
		return m_wRoot && m_character;
	}
	
	//-----------------------------------------------------------------------------------------------
	private bool SetupWidgetComponent(string widgetName, out Widget widget)
	{
		widget = Widget.Cast(m_wRoot.FindAnyWidget(widgetName));
		return widget != null;
	}
	
	//-----------------------------------------------------------------------------------------------
    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
        if (!ValidateInitialization()) 
		{
			return;
		}
        
        m_contentLayout = Widget.Cast(m_wRoot.FindAnyWidget("contentLayout"));
        if (!m_contentLayout)
		{
            return;
		}
		
		Widget offerListWidget;
		if (SetupWidgetComponent(m_sofferList, offerListWidget))
		{
			m_offerList = SCR_ListBoxComponent.Cast(offerListWidget.FindHandler(SCR_ListBoxComponent));
			if (!m_offerList)
				return;
			
			m_offerList.m_OnChanged.Insert(OnJobSelected);
		}
		
		SetupButtons();
		InitializeDataArrays();
		
		GetGame().GetCallqueue().CallLater(RefreshUI, 1000, true);
		RefreshJobsList();
	}
	
	//------------------------------------------------------------------------------------------------
	private void SetupButtons()
	{
		Widget declineButtonWidget, acceptButtonWidget;
		
		if (SetupWidgetComponent(m_sDeclineButton, declineButtonWidget))
		{
			m_DeclineButton = SCR_InputButtonComponent.Cast(declineButtonWidget.FindHandler(SCR_InputButtonComponent));
			if (m_DeclineButton)
				m_DeclineButton.m_OnActivated.Insert(OnDeclineJob);
		}
		
		if (SetupWidgetComponent(m_sAcceptButton, acceptButtonWidget))
		{
			m_AcceptButton = SCR_InputButtonComponent.Cast(acceptButtonWidget.FindHandler(SCR_InputButtonComponent));
			if (m_AcceptButton)
				m_AcceptButton.m_OnActivated.Insert(OnAcceptJob);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private void InitializeDataArrays()
	{
		m_aAvailableJobs = new array<ref RL_DrugDeliveryJob>;
		m_aAcceptedJobs = new array<ref RL_DrugDeliveryJob>;
		m_aAllJobs = new array<ref RL_DrugDeliveryJob>;
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
		GetGame().GetCallqueue().Remove(RefreshUI);
	}
	
	//------------------------------------------------------------------------------------------------
	void RefreshJobsList()
	{
		if (!ValidateInitialization() || !m_offerList)
			return;
		
		array<ref RL_DrugDeliveryJob> allJobs = m_character.GetDrugDeliveryJobs();
		m_aAvailableJobs = m_character.GetJobsByStatus(RL_DRUG_JOB_STATUS.Available);
		m_aAcceptedJobs = m_character.GetJobsByStatus(RL_DRUG_JOB_STATUS.Accepted);
		m_aAllJobs = allJobs;
		
		m_offerList.Clear();
		m_selectedJob = null;
		AddJobsToList(m_aAvailableJobs);
		AddJobsToList(m_aAcceptedJobs);
	}
	
	//------------------------------------------------------------------------------------------------
	private void AddJobsToList(array<ref RL_DrugDeliveryJob> jobs)
	{
		if (!jobs)
			return;
			
		foreach (RL_DrugDeliveryJob job : jobs)
		{
			if (job)
				AddJobToList(job);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void AddJobToList(RL_DrugDeliveryJob job)
	{
		if (!job || !m_offerList)
			return;
		
		string jobDescription = job.GetJobDescription();
		int itemIndex = m_offerList.AddItem(jobDescription, job);
		
		SCR_ListBoxElementComponent elementComp = m_offerList.GetElementComponent(itemIndex);
		if (!elementComp)
			return;
			
		Widget elementWidget = elementComp.GetRootWidget();
		if (!elementWidget)
			return;
		
		RL_DrugDealWidget dealWidget = RL_DrugDealWidget.Cast(elementWidget.FindHandler(RL_DrugDealWidget));
		if (dealWidget)
			dealWidget.SetData(job);
	}
	
	//------------------------------------------------------------------------------------------------
	void RefreshUI()
	{
		if (!m_character)
			return;
		
		CheckAndHandleExpiredJobs();
		array<ref RL_DrugDeliveryJob> currentJobs = m_character.GetDrugDeliveryJobs();
		int currentJobCount = 0;
		int cachedJobCount = 0;
		
		if (currentJobs)
			currentJobCount = currentJobs.Count();
		if (m_aAllJobs)
			cachedJobCount = m_aAllJobs.Count();
		
		bool needsRefresh = (currentJobCount != cachedJobCount);
		if (!needsRefresh && currentJobs && m_aAllJobs && currentJobCount == cachedJobCount)
		{
			for (int i = 0; i < currentJobCount; i++)
			{
				if (currentJobs[i] && m_aAllJobs[i] && 
					currentJobs[i].GetJobId() != m_aAllJobs[i].GetJobId())
				{
					needsRefresh = true;
					break;
				}
			}
		}
		
		if (needsRefresh)
			RefreshJobsList();
		else
			UpdateTimerDisplays();
		
		m_character.UpdateDrugDeliveryMarkers();
	}
	
	//------------------------------------------------------------------------------------------------
	void CheckAndHandleExpiredJobs()
	{
		array<ref RL_DrugDeliveryJob> acceptedJobs = m_character.GetJobsByStatus(RL_DRUG_JOB_STATUS.Accepted);
		if (!acceptedJobs)
			return;
			
		bool hasExpiredJobs = false;
		
		foreach (RL_DrugDeliveryJob job : acceptedJobs)
		{
			if (job && job.IsExpired())
			{
				job.SetStatus(RL_DRUG_JOB_STATUS.Expired);
				hasExpiredJobs = true;
			}
		}
		
		if (hasExpiredJobs)
			GetGame().GetCallqueue().CallLater(RefreshJobsList, 100);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateTimerDisplays()
	{
		if (!m_offerList)
			return;
		
		for (int i = 0; i < m_offerList.GetItemCount(); i++)
		{
			SCR_ListBoxElementComponent elementComp = m_offerList.GetElementComponent(i);
			if (!elementComp)
				continue;
			
			Widget elementWidget = elementComp.GetRootWidget();
			if (!elementWidget)
				continue;
			
			RL_DrugDealWidget dealWidget = RL_DrugDealWidget.Cast(elementWidget.FindHandler(RL_DrugDealWidget));
			if (dealWidget)
				dealWidget.UpdateDisplay();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnJobSelected(SCR_ListBoxComponent source, int item, bool selected)
	{
		if (!selected)
		{
			m_selectedJob = null;
			return;
		}
		
		m_selectedJob = RL_DrugDeliveryJob.Cast(source.GetItemData(item));
	}
	
	//------------------------------------------------------------------------------------------------
	private RL_DrugDeliveryJob GetSelectedValidJob()
	{
		if (!m_selectedJob || m_selectedJob.GetStatus() != RL_DRUG_JOB_STATUS.Available)
			return null;
			
		return m_selectedJob;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnAcceptJob(SCR_InputButtonComponent button)
	{
		if (m_character.IsSpamming())
			return;

		RL_DrugDeliveryJob selectedJob = GetSelectedValidJob();
		if (selectedJob)
		{
			m_character.AcceptDrugDeliveryJob(selectedJob.GetJobId());
			GetGame().GetCallqueue().CallLater(RefreshJobsList, 100);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnDeclineJob(SCR_InputButtonComponent button)
	{
		if (m_character.IsSpamming())
			return;

		RL_DrugDeliveryJob selectedJob = GetSelectedValidJob();
		if (selectedJob)
		{
			m_character.DeclineDrugDeliveryJob(selectedJob.GetJobId());
			GetGame().GetCallqueue().CallLater(RefreshJobsList, 100);
		}
	}
}