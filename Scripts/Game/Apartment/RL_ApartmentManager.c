[ComponentEditorProps(category: "", description: "")]
class RL_ApartmentManagerComponentClass : ScriptComponentClass {}

class RL_ApartmentManagerComponent : ScriptComponent
{
	protected ResourceName m_sApartmentPrefab = "{2B3E7FB7276E3F3A}Assets/Narcos_Apartment/Prefab/NLR_Apartment.et";
	ref map<IEntity, string> m_aApartments = new map<IEntity, string>();

	bool CreateApartment(int characterId, string playerPersistenceID, ResourceName apartmentPrefab = "")
	{
		if (!m_aApartments)
			m_aApartments = new map<IEntity, string>();

		if (GetSpawnedApartment(playerPersistenceID))
			RemoveApartment(characterId, playerPersistenceID);
			
		CreateApartmentForPlayer(characterId, playerPersistenceID, apartmentPrefab);
		
		return true;
	}

	void RemoveApartment(int characterId, string playerPersistenceID)
	{
		Print("RemoveApartment");
		if (!m_aApartments)
			return;

		DeleteApartment(playerPersistenceID);
	}

	private void CreateApartmentForPlayer(int characterId, string playerPersistenceID, ResourceName apartmentPrefab = "")
	{
		SCR_ChimeraCharacter character = RL_Utils.GetCharacterByCharacterIdLocal(characterId.ToString());
		if (!character)
			return;
		
		FinishApartmentLoad(characterId, playerPersistenceID, character, apartmentPrefab);
	}

	void FinishApartmentLoad(int characterId, string playerPersistenceID, SCR_ChimeraCharacter character, ResourceName apartmentPrefab = "")
	{
		Resource resource = Resource.Load(m_sApartmentPrefab);
		if (!resource || !resource.IsValid())
		{
			Print("FinishApartmentLoad Apartment prefab is not valid: " + m_sApartmentPrefab, LogLevel.ERROR);
			return;
		}
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character);
		EntitySpawnParams params = GetApartmentSpawnParams(playerId);
		if (!params) return;
		
		RL_ApartmentEntity apartmentEnt = RL_ApartmentEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params));
		if (!apartmentEnt)
		{
			Print("FinishApartmentLoad Failed to spawn apartment entity", LogLevel.ERROR);
			return;
		}	
		
		RL_ApartmentComponent apartmentComponent = RL_ApartmentComponent.Cast(apartmentEnt.FindComponent(RL_ApartmentComponent));
		if (apartmentComponent)
			apartmentComponent.SetOwnerCid(characterId);
		
		m_aApartments.Insert(apartmentEnt, playerPersistenceID);

		if (!apartmentEnt.GetPlayerSpawnPoint())
		{
			m_aApartments.Remove(apartmentEnt);
			SCR_EntityHelper.DeleteEntityAndChildren(apartmentEnt);
			return;
		}

		if (apartmentComponent)
			GetGame().GetCallqueue().CallLater(apartmentComponent.LoadAndSpawnUpgrades, 500, false);

		vector worldLocation = apartmentEnt.GetPlayerSpawnPoint();
		character.SpawnApartment(worldLocation);
	}

	void DeleteApartment(string persistinceID)
	{
		IEntity apartmentEntity = GetSpawnedApartment(persistinceID);
		Print("DeleteApartment Going for delete");
		if (!apartmentEntity)
			return;

		Print("DeleteApartment Found removing");
		m_aApartments.Remove(apartmentEntity);
		SCR_EntityHelper.DeleteEntityAndChildren(apartmentEntity);

	}
	IEntity GetSpawnedApartment(string persistinceID)
	{
		if(!m_aApartments || m_aApartments.Count() == 0)
			return null;
		return m_aApartments.GetKeyByValue(persistinceID);
	}
	// Generates spawn parameters for placing an apartment based on the character ID
	// this makes sure apartments for different players dont overlap
	private EntitySpawnParams GetApartmentSpawnParams(int playerId)
	{
		// Find the base apartment spawn location entity in the world
		IEntity baseSpawnPoint = GetGame().GetWorld().FindEntityByName("ApartmentsSpawnPoint");
		if (!baseSpawnPoint) return null;

		// Get the world transform matrix of the spawn point (position and orientation)
		vector baseTransform[4];
		baseSpawnPoint.GetWorldTransform(baseTransform);

		// Create a new spawn parameter object
		EntitySpawnParams spawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;

		RL_ApartmentUtils.OffsetApartmentByPlayerId(playerId, baseTransform);

		// Set the updated transform
		spawnParams.Transform = baseTransform;

		return spawnParams;
	}
}

