[EntityEditorProps(category: "Persistence/Respawn", description: "Character spawn point")]
class RL_SpawnPointClass : EPF_SpawnPointClass
{
}

class RL_SpawnPoint : EPF_SpawnPoint
{
	[Attribute("Spawn Title", desc: "Spawn point title used in spawn menu.")]
	protected string m_sTitle;

	[Attribute("Spawn Type", defvalue: "0", uiwidget: UIWidgets.ComboBox, desc: "Spawn Type", enums: {
		ParamEnum("Global", "0"),
		ParamEnum("Police", "1"),
		ParamEnum("EMS", "2"),
		ParamEnum("Debug", "3")
	})]
	int m_iSpawnType;
	
	[Attribute("CID Restrict", desc: "CID Restrict")]
	protected int m_iCidRestrict = 0;

	string GetTitle()
	{
		return m_sTitle;
	}

	int GetType()
	{
		return m_iSpawnType;
	}

	void SetCidRestrict(int cid)
	{
		m_iCidRestrict = cid;
	}

	int GetCidRestrict()
	{
		return m_iCidRestrict;
	}

	bool IsAvailableForPlayer(int playerCid, bool isPolice, bool isEms)
	{
		bool canSpawn = (m_iSpawnType == 0) || (m_iSpawnType == 1 && isPolice) || (m_iSpawnType == 2 && isEms);
		
		if (m_iCidRestrict == -1)
			canSpawn = false;
		else if (m_iCidRestrict > 0)
			canSpawn = canSpawn && (playerCid == m_iCidRestrict);
		
		return canSpawn;
	}

	static EPF_SpawnPoint GetDebugSpawnPoint()
	{
		foreach (EPF_SpawnPoint spawnPoint : s_aSpawnPoints)
		{
			//todo can we optimize? it loops a lot server side
			//Print("[RL_SpawnPoint] GetDebugSpawnPoint foreach");
			RL_SpawnPoint castedSpawn = RL_SpawnPoint.Cast(spawnPoint);
			if (castedSpawn && castedSpawn.GetType() == 3)
				return spawnPoint;
		}
		return null;
	}

	static array<EPF_SpawnPoint> GetSpawnPointsForPlayer(int playerCid, bool isPolice, bool isEms)
	{
		array<EPF_SpawnPoint> filteredSpawnPoints = {};
		
		foreach (EPF_SpawnPoint spawnPoint : s_aSpawnPoints)
		{
			//todo can we optimize? it loops a lot server side
			//Print("[RL_SpawnPoint] GetSpawnPointsForPlayer foreach");
			RL_SpawnPoint castedSpawn = RL_SpawnPoint.Cast(spawnPoint);
			if (castedSpawn && castedSpawn.IsAvailableForPlayer(playerCid, isPolice, isEms))
				filteredSpawnPoints.Insert(spawnPoint);
		}

		return filteredSpawnPoints;
	}

	override static array<EPF_SpawnPoint> GetSpawnPoints()
	{
		if (Replication.IsServer())
		{
			SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
			if (character)
			{
				return GetSpawnPointsForPlayer(
					character.GetCharacterId().ToInt(), 
					character.IsPolice(), 
					character.IsMedic()
				);
			}
		}
		return {};
	}

	override static RL_SpawnPoint GetRandomSpawnPoint()
	{
		array<EPF_SpawnPoint> spawnPoints = GetSpawnPoints();
		if (spawnPoints.IsEmpty())
			return null;
		return spawnPoints.GetRandomElement();
	}
}
