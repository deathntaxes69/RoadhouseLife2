class RL_OpenGarageDoor : ScriptedUserAction
{
	bool m_bDoorOpen = false;

	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{

	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if(!character || character.IsSpamming()) return;
		
        SignalsManagerComponent m_SignalManager = SignalsManagerComponent.Cast(pOwnerEntity.FindComponent(SignalsManagerComponent));
		int signalIndex = m_SignalManager.FindSignal("DoorSignalInput");
		//Print(signalIndex);
		//Print(string.Format("Current Value =  %1", m_SignalManager.GetSignalValue(signalIndex)));
		if (m_bDoorOpen)
		{
			m_SignalManager.SetSignalValue(signalIndex, 0);	
		} else {
			m_SignalManager.SetSignalValue(signalIndex, 1);	
		}
		m_bDoorOpen = !m_bDoorOpen;
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Door";

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return CanBePerformedScript(user);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return (
			true
		);
 	}
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
	
}