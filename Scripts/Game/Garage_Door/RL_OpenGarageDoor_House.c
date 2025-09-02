class RL_OpenGarageDoor_House : ScriptedUserAction
{
	bool m_bDoorOpen = false;
    bool m_bIsAnimatingNow = false;
    SignalsManagerComponent m_SignalManager;

    const float sleepBetween = 20;
    const float movementInterval = 0.02;
    const float rotationInterval = 4;
    const float PANEL_HEIGHT = 0.656158;

    private int EMS_OPEN_RANK = 1;

    const string m_aYSignals[4] = {"Panel_Y01_IN", "Panel_Y02_IN", "Panel_Y03_IN", "Panel_Y04_IN"};
    const string m_aRSignals[4] = {"Panel_R01_IN", "Panel_R02_IN", "Panel_R03_IN", "Panel_R04_IN"};
    const string m_aXSignals[4] = {"Panel_X01_IN", "Panel_X02_IN", "Panel_X03_IN", "Panel_X04_IN"};
	
    protected RL_HouseComponent FindHouseComponent()
    {
        IEntity doorEntity = GetOwner();
        if (!doorEntity)
            return null;

        IEntity parent = doorEntity.GetParent();
        if (!parent)
            return null;

        return RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
    }

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
        m_SignalManager = SignalsManagerComponent.Cast(pOwnerEntity.FindComponent(SignalsManagerComponent));
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if(!character || character.IsSpamming()) return;
		m_bDoorOpen = !m_bDoorOpen;
        GetGame().GetScriptModule().Call(this, "AnimateDoor", true, null, m_bDoorOpen);
	}

    void AnimateDoor(bool isDoorOpening)
    {
        m_bIsAnimatingNow = true;
        //Print("AnimateOpenDoor");
        float currentHeights[4];
        
        float currentHorizontal[4] = {0, 0 , 0, 0};
        float currentRotation[4] = {0, 0 , 0, 0};
        float maxHeights[4] = {
            (PANEL_HEIGHT / 2)+(PANEL_HEIGHT+PANEL_HEIGHT+PANEL_HEIGHT),
            (PANEL_HEIGHT / 2)+(PANEL_HEIGHT+PANEL_HEIGHT),
            (PANEL_HEIGHT / 2)+(PANEL_HEIGHT),
            (PANEL_HEIGHT / 2)
        };
        float maxHorizontal[4] = {
            (PANEL_HEIGHT / 2),
            (PANEL_HEIGHT / 2)+(PANEL_HEIGHT),
            (PANEL_HEIGHT / 2)+(PANEL_HEIGHT+PANEL_HEIGHT),
            (PANEL_HEIGHT / 2)+(PANEL_HEIGHT+PANEL_HEIGHT+PANEL_HEIGHT)
        };
        float desiredHeights[4];
        float desiredHorizontal[4];
        if(isDoorOpening)
        {
            currentHeights = {0, 0 , 0, 0};
            currentHorizontal = {0, 0 , 0, 0};
            currentRotation = {0, 0 , 0, 0};
            desiredHeights = maxHeights;
            desiredHorizontal = maxHorizontal;
        } else {
            currentHeights = maxHeights;
            currentHorizontal = maxHorizontal;
            currentRotation = {-90, -90 , -90, -90};
            desiredHeights = {0, 0 , 0, 0};
            desiredHorizontal = {0, 0 , 0, 0};
        }

        while ((isDoorOpening && desiredHorizontal[3] != currentHorizontal[3]) || (!isDoorOpening && desiredHeights[1] != currentHeights[1]))
        {
            //Print("[OpenGarageDoor_House] AnimateDoor while loop");
            for (int i = 0; i < 4; i++)
            {
                //Print("[OpenGarageDoor_House] AnimateDoor for");
                // SET HEIGHT: If opening
                // or closing and horizontal done
                // and height less than max
                if(
                    ( isDoorOpening ||
                    ( !isDoorOpening && desiredHorizontal[i] == currentHorizontal[i])) && 
                    desiredHeights[i] != currentHeights[i]

                )
                {
                    float newHeight;
                    if(isDoorOpening)
                        newHeight = currentHeights[i] + movementInterval;
                    else 
                        newHeight = currentHeights[i] - movementInterval;
                    float clampedHeight = Math.Clamp(newHeight, 0, maxHeights[i]);
                    SetSignalValueStr(m_aYSignals[i],  clampedHeight);
                    currentHeights[i] = clampedHeight;


                }
                // SET ROTATION: If opening, close to max height and not fully rotated
                // or closing, close to min horizontal and not fully rotated
                if(
                    (isDoorOpening && ((desiredHeights[i] - currentHeights[i]) <= (PANEL_HEIGHT/2)) && currentRotation[i] != -90) ||
                    (!isDoorOpening && ((currentHorizontal[i]) <= (PANEL_HEIGHT/2)) && currentRotation[i] != 0)

                )
                {
                    float newRotation;
                    if(isDoorOpening)
                        newRotation = currentRotation[i] - rotationInterval;
                    else 
                        newRotation = currentRotation[i] + rotationInterval;

                    float clampedRotation = Math.Clamp(newRotation, -90, 0);
                    SetSignalValueStr(m_aRSignals[i],  clampedRotation);
                    currentRotation[i] = clampedRotation;

                }
                // SET HORIZONTAL: If opening and max height reached
                // or closing 
                // and horizontal less than desired
                if(
                    (
                        (isDoorOpening && desiredHeights[i] == currentHeights[i]) ||
                        (!isDoorOpening)
                    ) &&
                    desiredHorizontal[i] != currentHorizontal[i]

                )
                {
                    float newHorizontal;
                    if(isDoorOpening)
                        newHorizontal = currentHorizontal[i] + movementInterval;
                    else 
                        newHorizontal = currentHorizontal[i] - movementInterval;

                    float clampedHorizontal = Math.Clamp(newHorizontal, 0, maxHorizontal[i]);
                    SetSignalValueStr(m_aXSignals[i],  clampedHorizontal*-1); //Reverse it since garage door is backwards
                    currentHorizontal[i] = clampedHorizontal;

                }
            }
            Sleep(sleepBetween);
        }
        m_bIsAnimatingNow = false;
    }

    void SetSignalValueStr(string signalName, float newValue)
    {
        //(string.Format("Set %1 to %2", signalName, newValue));
        int signalIndex = m_SignalManager.FindSignal(signalName);
        if(signalIndex != -1)
            m_SignalManager.SetSignalValue(signalIndex, newValue);	

    }

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        if(m_bDoorOpen)
            outName = "Close Door";
        else
            outName = "Open Door";

		return true;
	}
    override bool CanBeShownScript(IEntity user)
 	{
		return CanBePerformedScript(user);
	}
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
        if (m_bIsAnimatingNow)
            return false;

        RL_HouseComponent houseComponent = FindHouseComponent();
        if (houseComponent && houseComponent.IsOwned())
        {
            SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
            if (!character)
                return false;
            
            int characterId = character.GetCharacterId().ToInt();

            if (houseComponent.GetOwnerCid() == characterId)
                return true;
            
            if (houseComponent.IsLocked())
            {
                if (character.IsPolice())
                    return true;
                
                if (character.IsMedic() && character.GetMedicRank() > EMS_OPEN_RANK)
                    return true;
                
                SetCannotPerformReason("House is locked");
                return false;
            }
        }

		return true;
 	}
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	override bool CanBroadcastScript() 
	{ 
		return true; 
	}
	
}