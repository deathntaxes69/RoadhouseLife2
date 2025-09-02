class RL_Utils
{

	static void RotateRelative(IEntity parent, IEntity object, int rotation)
	{
		vector parentAngles = parent.GetAngles(); // <0.000000,-47.339584,0.000000> ---- [1] = rotation -180 to +180
		parentAngles[1] = parentAngles[1] + rotation;
		object.SetAngles(parentAngles);
	}
	
	static bool IsDedicatedServer()
	{
		return Replication.IsServer() && RplSession.Mode() == RplMode.Dedicated;
	}
	static int GetPlayerId()
	{
		return GetGame().GetPlayerController().GetPlayerId();

	}

	static string FormatMoney(int value)
	{
		string result = "";
		if (!value)
			return "$ 0";

		string formatted = string.Format("%1", value);
		int len = formatted.Length();

		for (int i = 0; i < len; i++)
		{
			Print("[RL_Utils] FormatMoney for");
			// Append characters from right to left
			result = formatted.Get(len - i - 1) + result;
			// Add a comma every 3 digits (but not at the start)
			if ((i + 1) % 3 == 0 && i + 1 < len)
			{
				result = "," + result;
			}
		}

		return "$ " + result;
	}

	static void TeleportPlayer(IEntity player, vector position, vector rotation = vector.Zero)
	{
		vector emptyPosition;
		SCR_WorldTools.FindEmptyTerrainPosition(emptyPosition, position, 2);
		emptyPosition = emptyPosition + "0 0.1 0";
		player.SetOrigin(emptyPosition);

	}

	static float GetDistance(IEntity thingOne, IEntity thingTwo)
	{
		return (vector.Distance(thingOne.GetOrigin(), thingTwo.GetOrigin()));
	}

	static bool WithinDistance(IEntity thingOne, IEntity thingTwo, float distance)
	{
		return (GetDistance(thingOne, thingTwo) < distance);
	}

	static bool UptimeGreaterThan(int minutes)
	{
		float uptimeMs = GetGame().GetWorld().GetWorldTime();
		return (uptimeMs / 60000) > minutes;
	}

	static bool LastSavedGreaterThan(EPF_MetaDataDbEntity dbEntity, int minutes)
	{
		int currentTime = System.GetUnixTime();
		float diff = currentTime - dbEntity.m_iLastSaved;
		return (diff/ 60) > minutes;
	}

	static ref array<IEntity> m_nearbyEntities = new array<IEntity>;
	static string requiredComp = "";

	static array<IEntity> GetNearbyVehicles(vector center, float radius, string comp = "")
	{
		m_nearbyEntities = new array<IEntity>;
		requiredComp = comp;
		GetGame().GetWorld().QueryEntitiesBySphere(center, radius, InsertEntity, FilterVehicles);
		return m_nearbyEntities;
	}

	static array<IEntity> GetNearbyCharacters(vector center, float radius)
	{
		m_nearbyEntities = new array<IEntity>;
		GetGame().GetWorld().QueryEntitiesBySphere(center, radius, InsertEntity, FilterCharacters);
		return m_nearbyEntities;
	}

	static bool InsertEntity(IEntity ent)
	{
		if (ent)
		{
			m_nearbyEntities.Insert(ent);
			return true;
		}
		return false;
	}

	static bool FilterVehicles(IEntity ent)
	{
		return (ent.IsInherited(Vehicle) && (requiredComp.IsEmpty() || ent.FindComponent(requiredComp.ToType())));
	}

	static bool FilterCharacters(IEntity ent)
	{
		return (!!SCR_ChimeraCharacter.Cast(ent));
	}

	static void Notify(string content, string title = "", int duration = 5)
	{
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if (!hud)
			return;
		hud.AddNotification(content, title, duration);
	}

	static void NotifyAllPolice(string message, string messageTitle)
	{
		if (!Replication.IsServer())
			return;

		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("[RL_Utils] NotifyAllPolice foreach loop");
			if (!character || !character.IsPolice())
				continue;
			character.Notify(message, messageTitle);
		}
	}
	
	static void NotifyAllCivs(string message, string messageTitle)
	{
		if (!Replication.IsServer())
			return;

		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("[RL_Utils] NotifyAllCivs foreach loop");
			if (!character || character.IsPolice() || character.IsMedic())
				continue;
			character.Notify(message, messageTitle);
		}
	}
	
	static void NotifyAllEMS(string message, string messageTitle)
	{
		if (!Replication.IsServer())
			return;

		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("[RL_Utils] NotifyAllEMS foreach loop");
			if (!character || !character.IsMedic())
				continue;
			character.Notify(message, messageTitle);
		}
	}

	static int GetPoliceOnlineCount()
	{
		RL_ServerEventManagerComponent eventManager = RL_ServerEventManagerComponent.GetEventManager();
		if(!eventManager) return 0;
		return eventManager.GetPoliceOnlineCount();
	}

	static int GetEMSOnlineCount()
	{
		RL_ServerEventManagerComponent eventManager = RL_ServerEventManagerComponent.GetEventManager();
		if(!eventManager) return 0;
		return eventManager.GetEMSOnlineCount();
	}

	static SCR_ChimeraCharacter FindCharacterById(string characterId)
	{
		if (!characterId)
			return null;

		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		if(!persistenceManager)
		{
			Print("FindCharacterById could not find EPF_PersistenceManager", LogLevel.ERROR);
			return null;
		}
		IEntity entity = persistenceManager.FindEntityByPersistentId(characterId);
		if (!entity)
		{
			Print(string.Format("Failed to find character ID %1 entity", characterId), LogLevel.ERROR);
			return null;
		}
		return SCR_ChimeraCharacter.Cast(entity);
	}

	static string GetPlayerName(int playerId)
	{
		IEntity playerEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(playerId);
		if (!playerEntity)
			return "";

		CharacterIdentityComponent identityComp = EL_Component<CharacterIdentityComponent>.Find(playerEntity);
		if (!identityComp)
			return "";
		Identity identity = identityComp.GetIdentity();
		//Print(identity);
		if (!identity || !identity.GetFullName())
			return "";

		return identity.GetFullName();
	}

	static string GetCharacterIdByPlayerId(int playerId)
	{
		IEntity playerEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(playerId);
		if (!playerEntity)
			return "";
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(playerEntity);
		if (!character)
			return "";

		return character.GetCharacterId();
	}

	static SCR_ChimeraCharacter GetLocalCharacter()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return null;
		IEntity playerEntity = pc.GetControlledEntity();
		if (!playerEntity)
			return null;

		return SCR_ChimeraCharacter.Cast(playerEntity);
	}

	static array<SCR_ChimeraCharacter> GetAllCharacters()
	{
		//todo cache this???
		array<SCR_ChimeraCharacter> characters = {};
		PlayerManager playerManager = GetGame().GetPlayerManager();
		array<int> playerIds = new array<int>();
		playerManager.GetPlayers(playerIds);
		foreach (int playerId : playerIds) {
			//todo can we optimize?
			//Print("[RL_Utils] GetAllCharacters foreach loop");
			IEntity playerEntity = playerManager.GetPlayerControlledEntity(playerId);
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(playerEntity);
			characters.Insert(character);
		}
		return characters;
	}

	static SCR_ChimeraCharacter GetCharacterByCharacterIdLocal(string characterId)
	{
		//optimize? this is run a lot
		foreach (SCR_ChimeraCharacter character : GetAllCharacters()) {
			//Print("[RL_Utils] GetCharacterByCharacterIdLocal foreach loop");
			if(character)
			{
				if (character.GetCharacterId() == characterId) {
					return character;
					break;
				}
			}
		}
		return null;
	}

	static bool CanPerformAction(IEntity entity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if (!character) return false;
		if (character.IsRestrained()) return false;
		if (character.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE) return false;
		return true;
	}

	static bool IsDevServer()
	{

		//ServerInfo m_serverInfo = GetGame().GetServerInfo();
		//if (!m_serverInfo || m_serverInfo.GetName().Contains("DEV SERVER"))
		//	return true;
		
		return false;
	}
	
	static string GetRandomName()
	{
		
		// change this in future
		
		array<string> firstNames = {
			"Benjamin", "Kinga", "Borat", "Kevin", "Ben", "Shadows", "Blake", "Spooz", "Nathan", "Tung Tung"
		};
	
		array<string> lastNames = {
			"Sahur", "Decker", "Murphy", "Fritz", "Holder", "Wall", "Watkins", "Paul", "Flynn", "Kane", "Phelps", "Pierce", "Davidson", "Donovan", "Kaufman", "Kent", "Kirk", "Kramer", "Massey", "Whitehead", "Eaton", "Knox", "Tyler", "Lane", "Bray", "Griffin", "Larson", "Morales", "Powers", "Hodges", "Hall"
		};
	
		string randomFirst = firstNames.GetRandomElement();
		string randomLast = lastNames.GetRandomElement();
	
		return string.Format("%1 %2", randomFirst, randomLast);
	}

	static string FormatTimeDisplay(float seconds)
	{
		int timeMinutes = Math.Floor(seconds / 60);
		int timeSecondsInt = Math.Floor(Math.Mod(seconds, 60));
		
		if (timeMinutes > 0)
			return string.Format("%1m %2s", timeMinutes, timeSecondsInt);
		else
			return string.Format("%1s", timeSecondsInt);
	}

	static bool EjectAllPlayers(IEntity vehicle)
	{
		SCR_BaseCompartmentManagerComponent m_CompartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		if (!m_CompartmentManager)
			return false;

		bool ejected = false;
      	array<BaseCompartmentSlot> compartments = {};
		m_CompartmentManager.GetCompartments(compartments);

		SCR_ChimeraCharacter m_character;
		foreach (BaseCompartmentSlot compartment : compartments)
		{
			Print("[RL_Utils] EjectAllPlayers foreach loop");
			if (compartment)
			{
				m_character = SCR_ChimeraCharacter.Cast(compartment.GetOccupant());
				if (m_character)
				{
					m_character.EjectFromVehicle();
					ejected = true;
				}
			}
		}

		return ejected;
	}
	static string ShortenIdentityId(string identityId)
	{
		string shortIdentityId = "ERROR";
		if(identityId && identityId.Length() > 3)
		{
			shortIdentityId = (identityId.Substring((identityId.Length() - 3), 3));
			shortIdentityId.ToUpper()
		}
		return shortIdentityId;
	}
	
	static string GenerateRandomUid()
	{
		int timestamp = System.GetUnixTime();
		int randomNum = Math.RandomInt(10000, 99999);
		return string.Format("%1_%2", timestamp, randomNum);
	}

	static bool IsArrayTheSame(array<string> array1, array<string> array2)
	{
		if (!array1 && !array2)
			return true;
		if (!array1 || !array2)
			return false;
		
		if (array1.Count() != array2.Count())
			return false;
		
		for (int i = 0; i < array1.Count(); i++) {
			Print("[RL_Utils] CompareArray for");
			if (array1[i] != array2[i])
				return false;
		}
    
    	return true;
  	}	
	static string GenerateRandomID(int length = 8)
	{
		string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		string result = "";
		
		for (int i = 0; i < length; i++)
		{
			int randomIndex = Math.RandomInt(0, chars.Length());
			result += chars.Get(randomIndex);
		}
		
		return result;
	}

}
