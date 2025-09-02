modded class SCR_DoorUserAction : DoorUserAction
{

    [Attribute("", UIWidgets.CheckBox, "For police doors")]
	private bool m_bRequiresKeycard = false;
	
	[Attribute("", UIWidgets.CheckBox, "Is a bank door")]
	private bool m_bIsBankDoor = false;

    private int EMS_OPEN_RANK = 1;
    
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
    
    override bool CanBePerformedScript(IEntity user)
	{
        DoorComponent doorComponent = GetDoorComponent();
        if (!doorComponent)
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

        if(m_bRequiresKeycard) {
			
            SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);

			if(character && (character.IsPolice() || character.IsMedic()))
				return true;

            SetCannotPerformReason("Requires Keycard"); 
            return false; 
        }
		
		if(m_bIsBankDoor) {
			
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
			if(character && character.IsPolice())
				return true;
			
			IEntity bankDoorEntity = GetOwner();
			if (bankDoorEntity)
			{
				SCR_BankDoorComponent bankDoor = SCR_BankDoorComponent.Cast(bankDoorEntity.FindComponent(SCR_BankDoorComponent));
				if(bankDoor)
					{
						if (bankDoor.m_isOpen)
							return true;
						SetCannotPerformReason("Door is not open yet"); 
            			return false; 
					}
			}
		}
		
		return true;
	}
}