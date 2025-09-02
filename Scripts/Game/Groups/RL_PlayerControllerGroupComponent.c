modded class SCR_PlayerControllerGroupComponent
{
	#ifndef WORKBENCH
	override void CreateAndJoinGroup(Faction faction)
	{
		
		if (!faction)
			return;

		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return;
		
		SCR_AIGroup group = groupsManager.GetFirstNotFullForFaction(faction, null, true);
		if (group)
			RequestJoinGroup(group.GetGroupID());
		else
			RequestCreateGroup(); //requestCreateGroup automatically puts player to the newly created group
	}
	#endif
}
