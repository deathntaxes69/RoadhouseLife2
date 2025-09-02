[EntityEditorProps(category: "NarcosLife/Vehicle", description: "Vehicle Color Modification Gadget")]
class RL_VehicleModSprayCanGadgetClass : SCR_GadgetComponentClass
{
	[Attribute("10.0", desc: "Maximum distance to find nearby vehicles")]
	float m_fMaxVehicleDistance;
}

class RL_VehicleModSprayCanGadget : SCR_GadgetComponent
{
	protected RL_VehicleModSprayCanGadgetClass m_ComponentData;
	protected IEntity m_NearestVehicle;
	protected RL_VehicleModsComponent m_VehicleModsComponent;
	protected bool m_bFoundNearbyVehicle = false;
	protected float m_fClosestDistance;
	protected vector m_vPlayerPosition;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_ComponentData = RL_VehicleModSprayCanGadgetClass.Cast(GetComponentData(owner));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ActivateAction()
	{
		if (m_CharacterOwner && m_iMode == EGadgetMode.IN_HAND)
			ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnToggleActive(bool state)
	{
		m_bActivated = state;
		
		if (!state) 
			return;
		
		if (!Replication.IsClient()) 
			return;
			
		if (!m_CharacterOwner)
			return;
			
		RplComponent rplComponent = RplComponent.Cast(m_CharacterOwner.FindComponent(RplComponent));
		if (!rplComponent || !rplComponent.IsOwner())
			return;
		
		if (!FindNearestVehicle())
		{
			RL_Utils.Notify("No vehicle found nearby", "VEHICLE");
			return;
		}
		
		OpenVehicleColorModUI();
	}
	
	//------------------------------------------------------------------------------------------------
	override EGadgetType GetType()
	{
		return EGadgetType.CONSUMABLE;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeRaised()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeHeld()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsVisibleEquipped()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool FindNearestVehicle()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetCharacterOwner());
		if (!character)
			return false;
			
		m_vPlayerPosition = character.GetOrigin();
		float maxDistance = 10.0;
		if (m_ComponentData)
			maxDistance = m_ComponentData.m_fMaxVehicleDistance;
		
		m_bFoundNearbyVehicle = false;
		m_fClosestDistance = maxDistance;
		m_NearestVehicle = null;
		m_VehicleModsComponent = null;
		GetGame().GetWorld().QueryEntitiesBySphere(m_vPlayerPosition, maxDistance, FilterVehicles);
		
		return m_bFoundNearbyVehicle;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OpenVehicleColorModUI()
	{
		if (!m_NearestVehicle || !m_VehicleModsComponent)
			return;
		
		RL_VehicleManagerComponent vehicleManager = RL_VehicleManagerComponent.Cast(m_NearestVehicle.FindComponent(RL_VehicleManagerComponent));
		if (!vehicleManager)
			return;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_CharacterOwner);
		if (!character)
			return;
			
		string playerCharacterId = character.GetCharacterId();
		string vehicleOwnerId = vehicleManager.GetOwnerId();
		
		if (!vehicleOwnerId || playerCharacterId != vehicleOwnerId)
		{
			RL_Utils.Notify("You don't own this vehicle", "VEHICLE");
			return;
		}
			
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_VehicleColorModMenu);
		RL_VehicleColorModUI colorModUI = RL_VehicleColorModUI.Cast(menuBase);
		if (colorModUI)
		{
			colorModUI.SetVehicleData(m_NearestVehicle, m_VehicleModsComponent, this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ApplyAllCurrentColors()
	{
		if (!m_NearestVehicle || !m_VehicleModsComponent)
			return;
		
		array<string> slotNames = {};
		array<int> slotColors = {};
		int bodyColor = -1;
		
		ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(m_NearestVehicle.FindComponent(ParametricMaterialInstanceComponent));
		if (matComp)
			bodyColor = matComp.GetColor();
		
		array<ref array<string>> slotsColorData = m_VehicleModsComponent.GetSlotsColor();
		if (slotsColorData)
		{
			foreach (array<string> slotData : slotsColorData)
			{
				if (slotData && slotData.Count() >= 2)
				{
					string slotName = slotData[0];
					int color = slotData[1].ToInt();
					slotNames.Insert(slotName);
					slotColors.Insert(color);
				}
			}
		}
		
		m_VehicleModsComponent.RequestApplyAllColors(slotNames, slotColors, bodyColor);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool FilterVehicles(IEntity entity)
	{
		if (!entity)
			return true;
		
		if (entity == m_CharacterOwner)
			return true;
		
		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle)
			return true;
			
		RL_VehicleModsComponent modsComp = RL_VehicleModsComponent.Cast(vehicle.FindComponent(RL_VehicleModsComponent));
		if (!modsComp)
			return true;
			
		float distance = vector.Distance(m_vPlayerPosition, vehicle.GetOrigin());
		if (distance < m_fClosestDistance)
		{
			m_fClosestDistance = distance;
			m_NearestVehicle = vehicle;
			m_VehicleModsComponent = modsComp;
			m_bFoundNearbyVehicle = true;
		}
		
		return true;
	}
}
