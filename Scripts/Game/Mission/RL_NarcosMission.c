modded class SCR_MissionHeader : MissionHeader
{
	[Attribute("0", desc: "Is this a real dedicated server?")]
	bool m_bDedicatedServer;

    [Attribute("0", desc: "No respawn on restart")]
	bool m_bNoRespawnOnRestart;

    bool IsDedicatedServer()
    {
        return m_bDedicatedServer;
    }
    bool NoRespawnOnRestart()
    {
        return m_bNoRespawnOnRestart;
    }
}
