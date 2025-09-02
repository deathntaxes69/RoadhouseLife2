modded class SCR_EditBoxComponent 
{
    // This is to disable forced focusing on 1 edit box for PC players
    // It will affect all menus
    // Just remove the whole modded class if there are issues
    override void UpdateInteractionState(bool forceDisabled)
	{
		if(System.GetPlatform() == EPlatform.WINDOWS)
            return;
        
        super.UpdateInteractionState(forceDisabled);

	}
}
