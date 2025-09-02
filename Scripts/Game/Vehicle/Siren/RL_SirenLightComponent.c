class RL_SirenLightComponentClass : ScriptComponentClass
{
}
// This component goes on each light entity attached to the vehicle
class RL_SirenLightComponent : ScriptComponent 
{
	
	[Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_SIREN_LIGHT_TYPE))]
	private RL_SIREN_LIGHT_TYPE m_sirenLightType;

	[Attribute()]
	protected ref array<ref RL_LightChild> m_Lights;
	
	[Attribute()]
	protected bool m_OverrideEmissiveColor;
	
	[Attribute("1 1 1", uiwidget: UIWidgets.ColorPicker)]
	protected vector m_EmissiveColor;
	
	[Attribute("2", uiwidget: UIWidgets.Slider, params: "0 1000")]
	protected int m_EmissiveMultiplier;

	[Attribute(desc: "Light index starting at 1", category: "Lightbar Settings")]
	protected int m_lightIndex;

	[Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_SIREN_LIGHT_SIDE), category: "Side Light Settings")]
	private RL_SIREN_LIGHT_SIDE m_sirenLightSide;

	[Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_SIREN_LIGHT_SIDE_POS), category: "Side Light Settings")]
	private RL_SIREN_LIGHT_SIDE_POS m_sirenLightSidePos;
	
	protected bool m_isOn = true;
	
	// Emissive multiplier when on/off
	const int EMISSIVE_OFF = 0;
	
	protected RL_SideLightEntitySlotInfo m_slotInfo;
	protected ParametricMaterialInstanceComponent m_Material;
	protected AnimationPlayerComponent m_Anim;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_Material = ParametricMaterialInstanceComponent.Cast(owner.FindComponent(ParametricMaterialInstanceComponent));
		m_Anim = AnimationPlayerComponent.Cast(owner.FindComponent(AnimationPlayerComponent));
		Register();
		if(!m_Material)
		{
			Print("No ParametricMaterialInstanceComponent in RL_SirenLightComponent", LogLevel.ERROR);
			return;
		}
		if(m_OverrideEmissiveColor) m_Material.SetEmissiveColor(Color.FromVector(m_EmissiveColor.Normalized()).PackToInt());
		//m_Material.SetEmissiveColor(Color.FromInt(0xff0000ff).PackToInt());
		// SET OFF BY DEFAULT
		m_Material.SetEmissiveMultiplier(EMISSIVE_OFF);
		//m_Material.SetEmissiveMultiplier(m_EmissiveMultiplier);
		SpawnLights();
	}
	RL_SIREN_LIGHT_TYPE GetType()
	{
		return m_sirenLightType;
	}
	int GetIndex()
	{
		return m_lightIndex;
	}
	RL_SIREN_LIGHT_SIDE_POS GetSidePos()
	{
		if(!m_slotInfo) return RL_SIREN_LIGHT_SIDE_POS.LEFT;
		return m_slotInfo.GetSidePos();
	}
	protected void SpawnLights()
	{
		foreach(RL_LightChild light :  m_Lights)
		{
			//Print("[RL_SirenLightComponent] SpawnLights foreach");
			light.Spawn(GetOwner());
		}
	}
	void TurnOn()
	{
		if(!m_isOn)
		{
			if(m_Material) m_Material.SetEmissiveMultiplier(m_EmissiveMultiplier);
			//SpawnLights();
			ToggleChildLights(true);
			m_isOn = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void TurnOff()
	{
		if(m_isOn)
		{
			//Print("turn off");
			if(m_Material) m_Material.SetEmissiveMultiplier(EMISSIVE_OFF);
			//DestroyLights();
			ToggleChildLights(false);
			m_isOn = false;
		}
	}
	void Toggle()
	{
		if(m_isOn) TurnOff();
		else TurnOn();
	}
	bool IsOn()
	{
		return m_isOn;
	}
	void SetColor(int newColor)
	{
		m_Material.SetEmissiveColor(newColor);
	}
	
	//------------------------------------------------------------------------------------------------
	void DestroyLights()
	{
		foreach(RL_LightChild light :  m_Lights)
		{
			//Print("[RL_SirenLightComponent] DestroyLights foreach");
			light.Destroy();
		}
	}
	void ToggleChildLights(bool state)
	{
		foreach(RL_LightChild light :  m_Lights)
		{
			//Print("[RL_SirenLightComponent] ToggleChildLights foreach");
			light.GetLight().SetEnabled(state);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Register()
	{
		GetGame().GetCallqueue().CallLater(RegisterDelayed, 1000, false);
	}
	void RegisterDelayed()
	{
		
		IEntity parent = GetOwner().GetParent();
		IEntity vehicle;
		RL_SirenManagerComponent manager;
		// Find big daddy
		while(parent && !manager)
		{
			//Print("[RL_SirenLightComponent] RegisterDelayed while loop");
			manager = RL_SirenManagerComponent.Cast(parent.FindComponent(RL_SirenManagerComponent));
			vehicle = parent;
			parent = parent.GetParent();
		}
		if(!manager){
			Print("Light component with no manager", LogLevel.WARNING);
			return;
		}
		array<EntitySlotInfo> slots = {};
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(vehicle.FindComponent(SlotManagerComponent));
		if(!slotManager) return;
		slotManager.GetSlotInfos(slots);
		foreach(EntitySlotInfo slot :  slots)
		{
			//Print("[RL_SirenLightComponent] RegisterDelayed foreach loop");
			RL_SideLightEntitySlotInfo castedSlot = RL_SideLightEntitySlotInfo.Cast(slot);
			if(castedSlot && castedSlot.GetAttachedEntity() == GetOwner())
				m_slotInfo = castedSlot;
		}
		
		if(GetType() == RL_SIREN_LIGHT_TYPE.Lightbar) 
			manager.RegisterLightbarLight(this);
		else if(m_slotInfo)
			manager.RegisterSideLight(this);

		
	}
	
	
	//------------------------------------------------------------------------------------------------
	void ~RL_SirenLightComponent()
	{
		DestroyLights();
	}
	
}


sealed enum RL_SIREN_LIGHT_TYPE {
    Lightbar,
	SideLight = 1,
}

sealed enum RL_SIREN_LIGHT_SIDE {
    Front,
	LEFT = 1,
	RIGHT = 2,
	BACK = 3
}