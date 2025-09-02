[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class RL_EditorActionKick : SCR_SelectedEntitiesContextAction
{
	// There's already a base game SCR_KickPlayerContextAction but it doesn't show up lol
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		SCR_EditablePlayerDelegateComponent playerDelegate = SCR_EditablePlayerDelegateComponent.Cast(selectedEntity);
		return playerDelegate && !SCR_Global.IsAdmin(playerDelegate.GetPlayerID());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		return CanBeShown(selectedEntity, cursorWorldPosition, flags);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Perform(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition,int flags, int param = -1)
	{
		SCR_EditablePlayerDelegateComponent playerDelegate;		
		foreach (SCR_EditableEntityComponent entity: selectedEntities)
		{
			Print("[RL_EditorActionKick] Perform foreach loop");
			playerDelegate = SCR_EditablePlayerDelegateComponent.Cast(entity);
			if (playerDelegate)
			{
				GetGame().GetPlayerManager().KickPlayer(playerDelegate.GetPlayerID(), PlayerManagerKickReason.KICK, 0);
			}			
		}
	}
};