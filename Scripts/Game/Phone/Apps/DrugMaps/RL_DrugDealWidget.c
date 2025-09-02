class RL_DrugDealWidget : ScriptedWidgetComponent
{
	protected RL_DrugMapsWidget m_DrugMapsWidget;
	private const string m_sStatus = "status";
	private const string m_sClientName = "clientName";
	private const string m_sQuantity = "quantity";
	private const string m_sDrugName = "drugName";
	private const string m_sPrice = "price";
	private const string m_sTimeRemaining = "timeRemaining";
	
	protected TextWidget m_Status;
	protected TextWidget m_ClientName;
	protected TextWidget m_Quantity;
	protected TextWidget m_DrugName;
	protected TextWidget m_Price;
	protected TextWidget m_TimeRemaining;
	
	protected ref RL_DrugDeliveryJob m_JobData;
	
	//-----------------------------------------------------------------------------------------------
	override protected void HandlerAttached(Widget w)
	{
		
		Widget parentWidget = w;
		while (!m_DrugMapsWidget && parentWidget)
		{
			Print("[RL_DrugDealWidget] HandlerAttached while loop");
			parentWidget = parentWidget.GetParent();
			if (parentWidget)
				m_DrugMapsWidget = RL_DrugMapsWidget.Cast(parentWidget.FindHandler(RL_DrugMapsWidget));
		}
		
		if(!m_DrugMapsWidget)
		{
			return;
		}
		
		m_Status = TextWidget.Cast(w.FindAnyWidget(m_sStatus));
		if(!m_Status)
			return;
		
		m_ClientName = TextWidget.Cast(w.FindAnyWidget(m_sClientName));
		if(!m_ClientName)
			return;
		
		m_Quantity = TextWidget.Cast(w.FindAnyWidget(m_sQuantity));
		if(!m_Quantity)
			return;
		
		m_DrugName = TextWidget.Cast(w.FindAnyWidget(m_sDrugName));
		if(!m_DrugName)
			return;
		
		m_Price = TextWidget.Cast(w.FindAnyWidget(m_sPrice));
		if(!m_Price)
			return;
		
		m_TimeRemaining = TextWidget.Cast(w.FindAnyWidget(m_sTimeRemaining));
		if(!m_TimeRemaining)
			return;
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetData(Managed data)
	{
		m_JobData = RL_DrugDeliveryJob.Cast(data);
		UpdateDisplay();
	}
		
	//-----------------------------------------------------------------------------------------------
	Managed GetData()
	{
		return m_JobData;
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetJobData(RL_DrugDeliveryJob jobData)
	{
		m_JobData = jobData;
		UpdateDisplay();
	}
		
	//-----------------------------------------------------------------------------------------------
	RL_DrugDeliveryJob GetJobData()
	{
		return m_JobData;
	}
		
	//-----------------------------------------------------------------------------------------------
	void UpdateDisplay()
	{
		if (!m_JobData)
		{
			SetStatus("No Data", Color.FromRGBA(128, 128, 128, 255));
			SetClientName("N/A");
			SetQuantity("0");
			SetDrugName("N/A");
			SetPrice("0");
			SetTimeRemaining("N/A");
			return;
		}
		
		string statusText;
		Color statusColor;
		
		switch (m_JobData.GetStatus())
		{
			case RL_DRUG_JOB_STATUS.Available:
				statusText = "Available";
				statusColor = Color.FromRGBA(100, 255, 100, 255); // Green
				break;
				
			case RL_DRUG_JOB_STATUS.Accepted:
				statusText = "Accepted";
				statusColor = Color.FromRGBA(255, 255, 100, 255); // Yellow
				break;
				
			case RL_DRUG_JOB_STATUS.Expired:
				statusText = "Expired";
				statusColor = Color.FromRGBA(255, 100, 100, 255); // Red
				break;
				
			default:
				statusText = m_JobData.GetStatusString();
				statusColor = Color.FromRGBA(200, 200, 200, 255); // Gray
				break;
		}
		
		SetStatus(statusText, statusColor);
		SetClientName(m_JobData.GetClientName());
		SetQuantity(m_JobData.GetQuantity().ToString());
		SetDrugName(GetDrugName(m_JobData.GetDrugType()));
		SetPrice(m_JobData.GetTotalPayment().ToString());
		SetTimeRemaining(FormatTimeRemaining(m_JobData.GetRemainingTime()));
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetStatus(string text, Color color = Color.White)
	{
		if (m_Status)
		{
			m_Status.SetText(text);
			m_Status.SetColor(color);
		}
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetClientName(string text)
	{
		if (m_ClientName)
			m_ClientName.SetText(text);
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetQuantity(string text)
	{
		if (m_Quantity)
			m_Quantity.SetText(text);
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetDrugName(string text)
	{
		if (m_DrugName)
			m_DrugName.SetText(text);
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetPrice(string text)
	{
		if (m_Price)
			m_Price.SetText(text);
	}
		
	//-----------------------------------------------------------------------------------------------
	void SetTimeRemaining(string text)
	{
		if (m_TimeRemaining)
			m_TimeRemaining.SetText(text);
	}
		
	//-----------------------------------------------------------------------------------------------
	string FormatTimeRemaining(float seconds)
	{
		if (seconds <= 0)
			return "Expired";

		return RL_Utils.FormatTimeDisplay(seconds);
	}
}