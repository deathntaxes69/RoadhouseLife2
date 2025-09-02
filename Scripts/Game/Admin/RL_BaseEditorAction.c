/*enum EStaffActionLevel {
	SUPPORT = 1,
    ADMIN = 2
}

modded class SCR_BaseEditorAction
{
	[Attribute(defvalue: "2", desc: "Required minimum staff level", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EStaffActionLevel))]
	EStaffActionLevel m_eStaffActionLevel;

	EStaffActionLevel GetStaffActionLevelForUid(string uid)
	{
		// logic here depending on how we add whitelisting
		EStaffActionLevel level = EStaffActionLevel.ADMIN;
		return level;
	}
	
	override bool CanBePerformed(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		SCR_ChimeraCharacter localCharacter = RL_Utils.GetLocalCharacter();
		if (!localCharacter)
			return false;
		
		string localUid = localCharacter.GetIdentityId();
		EStaffActionLevel userLevel = GetStaffActionLevelForUid(localUid);

		if (!localUid || userLevel < m_eStaffActionLevel)
			PrintFormat("GAME MASTER PERMISSION DENIED");
			return false;

		PrintFormat("GAME MASTER PERMISSION GRANTED");
		return true;
	}
}*/