modded class SCR_PingEditorComponentClass
{
	override void ActivateEffects(SCR_PingEditorComponent component, bool isReceiver, int reporterID, bool reporterInEditor, bool unlimitedOnly, vector position, set<SCR_EditableEntityComponent> targets)
	{
		return;
	}
}

modded class SCR_PingEditorComponent
{
	override void SendPing(bool unlimitedOnly = false, vector position = vector.Zero, SCR_EditableEntityComponent target = null)
	{
		return;
	}	
}
