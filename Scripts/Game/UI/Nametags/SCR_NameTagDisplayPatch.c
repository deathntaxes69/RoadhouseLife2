modded class SCR_NameTagDisplay 
{																		
	//------------------------------------------------------------------------------------------------
	override protected void ProcessFiltered()
	{
		if (!s_NametagCfg) return;
		
		super.ProcessFiltered();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void InitializeTag(IEntity entity, bool friendlyOnly = true)
	{
		friendlyOnly = false;
		
		super.InitializeTag(entity, friendlyOnly);
	}
}
