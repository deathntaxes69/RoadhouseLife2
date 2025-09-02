modded class SCR_PlayerController
{
	override protected void UpdateLocalPlayerController()
	{
		super.UpdateLocalPlayerController();
		
		InputManager inputManager = GetGame().GetInputManager();
		if (!inputManager)
			return;
		
		GetGame().GetInputManager().AddActionListener("RL_PhoneOpen", EActionTrigger.DOWN, OnPhoneOpen);
		GetGame().GetInputManager().AddActionListener("RL_SurrenderAction", EActionTrigger.DOWN, OnSurrender);
		GetGame().GetInputManager().AddActionListener("RL_LockVehicle", EActionTrigger.DOWN, LockVehicle);
	}

	protected void OnPhoneOpen()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetControlledEntity());
		if (!character)
			return;
		
		character.OpenPhone();
	}
	
	protected void OnSurrender()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetControlledEntity());
		if (!character || character.IsSpamming(true))
			return;
		
		character.OnSurrender();
	}
	protected void LockVehicle()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetControlledEntity());
		if (!character)
			return;
		
		character.OnLockVehicleHotkey();
	}
}