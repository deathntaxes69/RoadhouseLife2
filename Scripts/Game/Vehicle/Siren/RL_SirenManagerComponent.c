class RL_SirenManagerComponentClass : ScriptComponentClass
{
}

// This component goes on the vehicle, all entities with SirenLightComp register to it
class RL_SirenManagerComponent : ScriptComponent
{
	protected Vehicle m_vehicle;
	protected SlotManagerComponent m_slotManager;

	protected ref array<RL_SirenLightComponent> m_LightbarLights = {};
	protected ref array<RL_SirenLightComponent> m_SideLights = {};

    protected bool m_lightsOn = false;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		EventHandlerManagerComponent ev = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
		if (ev)
		{
			ev.RegisterScriptHandler("OnCompartmentEntered", this, OnCompartmentEntered);
			ev.RegisterScriptHandler("OnCompartmentLeft", this, OnCompartmentLeft);
		}
		m_vehicle = Vehicle.Cast(GetOwner().GetRootParent());
		m_slotManager = SlotManagerComponent.Cast(m_vehicle.FindComponent(SlotManagerComponent));
		
	}
	
	
	//------------------------------------------------------------------------------------------------
	protected void OnCompartmentLeft(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		if(RL_Utils.IsDedicatedServer()) return;

		if(GetGame().GetPlayerController().GetControlledEntity() != occupant || mgr.FindCompartment(slotID).Type() != PilotCompartmentSlot)
			return;
		
		GetGame().GetInputManager().RemoveActionListener("RL_SirenToggleAction", EActionTrigger.DOWN, OnSirenToggle);
        GetGame().GetInputManager().RemoveActionListener("RL_SirenCycleAction", EActionTrigger.DOWN, OnSirenCycle);
        GetGame().GetInputManager().RemoveActionListener("RL_HornCycleAction", EActionTrigger.DOWN, OnHornCycle);
		GetGame().GetInputManager().RemoveActionListener("RL_SirenLightAction", EActionTrigger.DOWN, OnLightToggle);
		
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnCompartmentEntered(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotID)
	{
		if(RL_Utils.IsDedicatedServer()) return;

		// Enable siren controls if current player entered the pilot compartment
		if(GetGame().GetPlayerController().GetControlledEntity() != occupant || mgr.FindCompartment(slotID).Type() != PilotCompartmentSlot)
			return;
		
		GetGame().GetInputManager().AddActionListener("RL_SirenToggleAction", EActionTrigger.DOWN, OnSirenToggle);
        GetGame().GetInputManager().AddActionListener("RL_SirenCycleAction", EActionTrigger.DOWN, OnSirenCycle);
        GetGame().GetInputManager().AddActionListener("RL_HornCycleAction", EActionTrigger.DOWN, OnHornCycle);
		GetGame().GetInputManager().AddActionListener("RL_SirenLightAction", EActionTrigger.DOWN, OnLightToggle);
	}

	protected void OnSirenToggle(float value, EActionTrigger trigger)
	{
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if (character.IsSpamming())
			return;

        Rpc(RpcAsk_OnSirenToggle, character.GetCharacterId());
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_OnSirenToggle(string characterid)
    {
		Print("RpcAsk_OnSirenToggle");
		m_vehicle.ToggleSirenSound(characterid);
	}
    protected void OnSirenCycle(float value, EActionTrigger trigger)
	{
		if (RL_Utils.GetLocalCharacter().IsSpamming())
			return;

		Rpc(RpcAsk_OnSirenCycle);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_OnSirenCycle()
    {
		Print("RpcAsk_OnSirenCycle");
		m_vehicle.CycleSirenSound();
	}
    protected void OnHornCycle(float value, EActionTrigger trigger)
	{
		if (RL_Utils.GetLocalCharacter().IsSpamming())
			return;

        Rpc(RpcAsk_OnHornCycle);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_OnHornCycle()
    {
		Print("RpcAsk_OnHornCycle");
		m_vehicle.CycleHornSound();
	}
	protected void OnLightToggle(float value, EActionTrigger trigger)
	{
		if (RL_Utils.GetLocalCharacter().IsSpamming())
			return;

		Rpc(RpcAsk_OnLightToggle);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_OnLightToggle()
    {
		Print("RpcAsk_OnLightToggle");
		m_lightsOn = !m_lightsOn;
		if(RL_Utils.IsDedicatedServer())
			Rpc(RpcDo_OnLightToggle, m_lightsOn);
		else
			RpcDo_OnLightToggle(m_lightsOn);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_OnLightToggle(bool state)
    {
		m_lightsOn = state;
		Print("RpcDo_OnLightToggle Broadcast");
		GetGame().GetCallqueue().Remove(AnimateLights);
		if(state)
       		GetGame().GetCallqueue().CallLater(AnimateLights, 200, true);
		else
			TurnOffLights();
	}
	void RegisterLightbarLight(RL_SirenLightComponent light)
	{
		m_LightbarLights.Insert(light);

	}
	void RegisterSideLight(RL_SirenLightComponent light)
	{
		m_SideLights.Insert(light);
	}
	void TurnOffLights()
	{
		GetGame().GetCallqueue().Remove(AnimateLights);
		foreach (RL_SirenLightComponent light : m_LightbarLights)
		{
			//Print("[RL_SirenManagerComponent] TurnOffLights foreach");
			light.TurnOff();
		}
		foreach (RL_SirenLightComponent light : m_SideLights)
		{
			//Print("[RL_SirenManagerComponent] TurnOffLights foreach 2");
			light.TurnOff();
		}
	}
	void AnimateLights()
	{
		if (!m_lightsOn || (m_LightbarLights.IsEmpty() && m_SideLights.IsEmpty()))
		{
			TurnOffLights();
			return;
		}
		AnimateSideLights();
		AnimateLightBar();
	}

	bool m_LeftToggle = true;
	bool m_LeftColorToggle = true;
	bool m_RightColorToggle = false;

	float m_LastToggleTime = 0;
	float m_ToggleInterval = 1000; // Interval for left/right alternating

	float m_MiddleToggleTime = 0;
	bool m_MiddleToggle = true;
	float m_MiddleInterval = 500; // Middle strobe blink interval

	void AnimateSideLights()
	{
		float currentTime = GetGame().GetWorld().GetWorldTime();

		// Toggle left/right pattern
		if (currentTime - m_LastToggleTime >= m_ToggleInterval)
		{
			m_LeftToggle = !m_LeftToggle;
			m_LastToggleTime = currentTime;
			if (m_LeftToggle)
				m_LeftColorToggle = !m_LeftColorToggle;
			else
				m_RightColorToggle = !m_RightColorToggle;

		}

		// Toggle middle strobe pattern
		if (currentTime - m_MiddleToggleTime >= m_MiddleInterval)
		{
			m_MiddleToggle = !m_MiddleToggle;
			m_MiddleToggleTime = currentTime;
		}

		foreach (RL_SirenLightComponent light : m_SideLights)
		{
			//Print("[RL_SirenManagerComponent] AnimateSideLights foreach");
			RL_SIREN_LIGHT_SIDE_POS side = light.GetSidePos();
			switch (side)
			{
				case RL_SIREN_LIGHT_SIDE_POS.LEFT:
				{
					if (m_LeftToggle)
					{
						if (m_LeftColorToggle)
							light.SetColor(0xffff0000);
						else
							light.SetColor(0xffffffff);
						light.TurnOn();
					}
					else
						light.TurnOff();
					break;
				}
				case RL_SIREN_LIGHT_SIDE_POS.RIGHT:
				{
					if (!m_LeftToggle)
					{
						if (m_RightColorToggle)
							light.SetColor(0xffffffff);
						else
							light.SetColor(0xffff0000);
						light.TurnOn();

					}
					else
						light.TurnOff();
					break;
				}
				case RL_SIREN_LIGHT_SIDE_POS.MIDDLE:
				{
					if (m_MiddleToggle)
						light.TurnOn();
					else
						light.TurnOff();
					break;
				}
			}
		}
	}

	protected int m_Step = 0;
	protected bool m_Reverse = false;
	protected int m_Mode = 0; // 0 = Center-Outward, 1 = Flash Left-Right
	protected int m_FlashStep = 0;
	protected int m_FlashCount = 6; // Number of flash cycles before returning to mode 0
	void AnimateLightBar()
	{
		int totalLights = m_LightbarLights.Count();
		if(totalLights < 2) return;
		int midLeft = totalLights / 2;
		int midRight = (totalLights / 2) + 1;
		int leftSideMax = totalLights / 2; // First half goes to the left

		// Mode 0: Center-Outward animation
		if (m_Mode == 0)
		{
			int leftIndex = midLeft - m_Step;
			int rightIndex = midRight + m_Step;

			foreach (RL_SirenLightComponent light : m_LightbarLights)
			{
				//Print("[RL_SirenManagerComponent] AnimateLightBar foreach");
				int index = light.GetIndex();
				if (index == leftIndex || index == rightIndex)
				{
					light.Toggle();
				}
			}

			// Step control
			if (m_Reverse)
			{
				m_Step--;
				if (m_Step < 0)
				{
					m_Reverse = false;
					m_Step = 0;
					m_Mode = 1; // Switch to Flash mode
					m_FlashStep = 0;
				}
			}
			else
			{
				m_Step++;
				if (leftIndex <= 1 && rightIndex >= totalLights)
				{
					m_Reverse = true;
					m_Step--;
				}
			}
		}
		// Mode 1: Flash Left-Right
		else if (m_Mode == 1)
		{
			bool flashLeft = (m_FlashStep % 2 == 0);

			foreach (RL_SirenLightComponent light : m_LightbarLights)
			{
				//Print("[RL_SirenManagerComponent] AnimateLightBar foreach 2");
				int index = light.GetIndex();
				if ((flashLeft && index <= leftSideMax) || (!flashLeft && index > leftSideMax))
				{
					light.Toggle();
				}
			}

			m_FlashStep++;
			if (m_FlashStep >= m_FlashCount * 2) // Run flash cycle
			{
				m_Mode = 0; // Return to normal animation
				m_Step = 0;
				m_Reverse = false;
			}
		}
	}

	
}
