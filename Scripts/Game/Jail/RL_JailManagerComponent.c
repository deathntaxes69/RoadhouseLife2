class RL_JailManagerComponentClass: ScriptComponentClass
{

};

class RL_JailManagerComponent: ScriptComponent
{

    protected static string m_jailWallEntityName = "JailWall";
    protected static int m_jailRadius = 195;
    protected static float m_cellRadius = 0.9;
    protected static ref array<vector> m_jailSpawnPoints = {};
    protected static IEntity m_jail;

	override void OnPostInit(IEntity owner)
	{		
		super.OnPostInit(owner);
        m_jail = owner;
        array<string> boneNames = {};
        owner.GetBoneNames(boneNames);
        foreach(string boneName: boneNames) {
            //Print("[RL_JailManagerComponent] OnPostInit foreach loop");
            if(!boneName.StartsWith("SOCKET_JAIL_SPAWN_")) continue;
            vector localMatrix[4];
            owner.GetBoneMatrix(owner.GetBoneIndex(boneName), localMatrix);
            m_jailSpawnPoints.Insert(owner.CoordToParent(localMatrix[3]));


        }
	}
    static vector GetFreeSpawnPoint()
	{
		if (m_jailSpawnPoints.IsEmpty()) 
			return vector.Zero;
        
        foreach(vector spawnPoint: m_jailSpawnPoints) {
            Print("[RL_JailManagerComponent] GetFreeSpawnPoint foreach loop");
            array<IEntity> nearbyCharacters = RL_Utils.GetNearbyCharacters(spawnPoint, m_cellRadius);
            if(nearbyCharacters.Count() == 0) 
                return spawnPoint;
        }
		// Default to random cell if all are occupied
		return m_jailSpawnPoints.GetRandomElement();
	}
    static bool IsOutsideJailBounds(IEntity player) 
    {
        Print(RL_Utils.GetDistance(m_jail, player));
        return !RL_Utils.WithinDistance(player, m_jail, m_jailRadius);
    }
    override event protected void OnDelete(IEntity owner)
	{
        
	}
	
	
};
