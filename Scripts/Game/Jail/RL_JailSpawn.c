[EntityEditorProps()]
class RL_JailReleasePointClass : SCR_PositionClass
{
}

class RL_JailReleasePoint : SCR_Position
{

	protected static ref array<RL_JailReleasePoint> s_aSpawnPoints = {};

	//------------------------------------------------------------------------------------------------
	static RL_JailReleasePoint GetRandomSpawnPoint()
	{
		if (s_aSpawnPoints.IsEmpty()) 
			return null;
		
		return s_aSpawnPoints.GetRandomElement();
	}
	
	//------------------------------------------------------------------------------------------------
	static array<RL_JailReleasePoint> GetSpawnPoints()
	{
		return s_aSpawnPoints;
	}

	//------------------------------------------------------------------------------------------------
	#ifdef WORKBENCH
	override void SetColorAndText()
	{
		m_sText = "Jail Spawn";
		m_iColor = Color.ORANGE;
	}
	#endif

	//------------------------------------------------------------------------------------------------
	void GetPosYPR(out vector position, out vector ypr)
	{
		position = GetOrigin();
		ypr = GetYawPitchRoll();
		SCR_WorldTools.FindEmptyTerrainPosition(position, position, 1);
	}

	//------------------------------------------------------------------------------------------------
	void RL_JailReleasePoint(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.STATIC, true);

		if (GetGame().InPlayMode()) 
			s_aSpawnPoints.Insert(this);
	}

	//------------------------------------------------------------------------------------------------
	void ~RL_JailReleasePoint()
	{
		if (s_aSpawnPoints) 
			s_aSpawnPoints.RemoveItem(this);
	}
}