[EntityEditorProps(category: "GameScripted/Components", description: "")]
class RL_DrugMapsDeliverComponentClass : ScriptComponentClass
{
	
	[Attribute("1.0", desc: "Multiplier applied to base drug prices")]
	float m_fPriceMultiplier;
	
	[Attribute("1", desc: "Minimum quantity that can be requested")]
	int m_iMinQuantity;
	
	[Attribute("5", desc: "Maximum quantity that can be requested")]
	int m_iMaxQuantity;
	
	[Attribute("1 3", desc: "List of drug types from EDrugs that this location accepts")]
	ref array<int> m_aAvailableDrugTypes;

	
	[Attribute("1.0 0.2 0.2 1.0", desc: "Map marker color")]
	ref Color m_cMarkerColor;	

}

class RL_DrugMapsDeliverComponent : ScriptComponent
{	
	protected string m_sClientName;
	protected IEntity m_ownerEntity;
	protected string m_sLocationId;
	protected ResourceName m_markerIcon = "{F4F27AA841E772C2}Prefabs/World/Locations/Common/RL_MapMarkerDrugDelivery.et";
	protected bool m_bRegistered = false;
	
	//------------------------------------------------------------------------------------------------
	private RL_DrugMapsDeliverComponentClass GetPrefabData()
	{
		return RL_DrugMapsDeliverComponentClass.Cast(GetComponentData(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_ownerEntity = owner;
		m_sClientName = RL_Utils.GetRandomName();
		m_sLocationId = string.Format("DrugLocation_%1", owner.GetID());
		
		if (!Replication.IsServer())
			GetGame().GetCallqueue().CallLater(RegisterWithManager, 2000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	private void RegisterWithManager()
	{
		if (Replication.IsServer() || m_bRegistered)
			return;
			
		RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
		if (manager)
		{
			manager.RegisterDeliveryLocation(this);
			m_bRegistered = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_DrugMapsDeliverComponent()
	{
		if (!Replication.IsServer() && m_bRegistered)
		{
			RL_DrugDeliveryManager manager = RL_DrugDeliveryManager.GetInstance();
			if (manager)
				manager.UnregisterDeliveryLocation(this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPriceMultiplier() 
	{ 
		RL_DrugMapsDeliverComponentClass prefabData = GetPrefabData();
		if (prefabData)
			return prefabData.m_fPriceMultiplier;
		else
			return 1.0;
	}
	
	//-----------------------------------------------------------------------------------------------	
	int GetMinQuantity() 
	{ 
		RL_DrugMapsDeliverComponentClass prefabData = GetPrefabData();
		if (prefabData)
			return prefabData.m_iMinQuantity;
		else
			return 1;
	}
		
	//-----------------------------------------------------------------------------------------------
	int GetMaxQuantity() 
	{ 
		RL_DrugMapsDeliverComponentClass prefabData = GetPrefabData();
		if (prefabData)
			return prefabData.m_iMaxQuantity;
		else
			return 5;
	}
		
	//-----------------------------------------------------------------------------------------------
	array<int> GetAvailableDrugTypes() 
	{ 
		RL_DrugMapsDeliverComponentClass prefabData = GetPrefabData();
		if (prefabData && prefabData.m_aAvailableDrugTypes)
			return prefabData.m_aAvailableDrugTypes;
		
		array<int> defaultTypes = {EDrugs.WEED, EDrugs.HEROIN};
		return defaultTypes;
	}
		

		
	//-----------------------------------------------------------------------------------------------
	string GetMarkerIcon() 
	{ 
		return "WEED";
	}
		
	//-----------------------------------------------------------------------------------------------
	Color GetMarkerColor() 
	{ 
		RL_DrugMapsDeliverComponentClass prefabData = GetPrefabData();
		if (prefabData && prefabData.m_cMarkerColor)
			return prefabData.m_cMarkerColor;
		return Color.FromRGBA(255, 51, 51, 255); // Red
	}
		
	//-----------------------------------------------------------------------------------------------
	string GetClientName() 
	{ 
		return m_sClientName; 
	}
		
	//-----------------------------------------------------------------------------------------------
	string GetLocationId()
	{
		return m_sLocationId;
	}
}