modded class SCR_DynamicSimulationEditorComponent : SCR_BaseEditorComponent
{
	override protected void EnableStreamingForEntity(SCR_EditableEntityComponent entity, bool enable)
	{
		// Disable constant streaming of entities that are not players/vehicles, this causes replication flooding
		if (!entity.GetOwner() || (!EntityUtils.IsPlayer(entity.GetOwner()) && !Vehicle.Cast(entity.GetOwner()))) return;
		
		super.EnableStreamingForEntity(entity, enable);
	}
}