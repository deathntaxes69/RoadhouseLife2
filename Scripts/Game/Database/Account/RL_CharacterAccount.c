class RL_CharacterAccount : Managed
{

	protected int id;
	protected string name;
	protected string prefab;
	protected string uid;
	protected int policeRank;
	protected int policeDept;
	protected int medicRank;
	protected int jailTime;
	protected string licenses;
	protected string recipes;
	protected int gangId;
	protected int gangRank;
	protected int age;
	protected int apartmentId;
	protected string apartment;
	protected int playTime;
	protected int connectedAt;
	protected bool hasWarrant;
	protected ref array<string> ownedPerks;
	protected string ownedHouseId;
	protected string ownedHousePrefab;
	protected ref array<ref RL_VehicleKeyData> ownedVehicles;

	string GetCharacterId()
	{
		return id.ToString();
	}
	string GetIdentityId()
	{
		return uid;
	}
	string GetName()
	{
		return name;
	}
	int GetAge()
	{
		return age;
	}
	string GetPrefab()
	{
		return prefab;
	}
	int GetPoliceRank()
	{
		return policeRank;
	}
	int GetPoliceDept()
	{
		return policeDept;
	}
	int GetMedicRank()
	{
		return medicRank;
	}
	int GetJailTime()
	{
		return jailTime;
	}
	void SetJailTime(int newJailTime)
	{
		jailTime = newJailTime;
	}

	string GetLicenses()
	{
		return licenses;
	}

	void SetLicenses(string newLicenses)
	{
		licenses = newLicenses;
	}

	string GetRecipes()
	{
		return recipes;
	}

	void SetRecipes(string newRecipes)
	{
		recipes = newRecipes;
	}

	int GetGangId()
	{
		return gangId;
	}

	int GetGangRank()
	{
		return gangRank;
	}

	int GetPlayTime()
	{
		return playTime;
	}

	void SetConnectedAt()
	{
		connectedAt = System.GetTickCount();
	}

	int GetConnectedAt()
	{
		return connectedAt;
	}

	bool HasLicense(string license)
	{
		if (!licenses || licenses.IsEmpty())
			return false;

		array<string> licenseArray = {};
		licenses.Split(",", licenseArray, true);

		foreach (string lic : licenseArray)
		{
			Print("[RL_CharacterAccount] HasLicense foreach loop");
			if (lic == license)
				return true;
		}

		return false;
	}

	int GetApartmentLocationId()
	{
		return apartmentId;
	}

	string GetApartment()
	{
		return apartment;
	}

	bool GetHasWarrant()
	{
		return hasWarrant;
	}

	void SetHasWarrant(bool newHasWarrant)
	{
		hasWarrant = newHasWarrant;
	}

	array<string> GetOwnedPerks()
	{
		return ownedPerks;
	}

	void SetOwnedPerks(array<string> newOwnedPerks)
	{
		ownedPerks = newOwnedPerks;
	}

	string GetOwnedHouseId()
	{
		return ownedHouseId;
	}

	void SetOwnedHouseId(string newOwnedHouseId)
	{
		ownedHouseId = newOwnedHouseId;
	}

	string GetOwnedHousePrefab()
	{
		return ownedHousePrefab;
	}

	void SetOwnedHousePrefab(string newOwnedHousePrefab)
	{
		ownedHousePrefab = newOwnedHousePrefab;
	}

	array<ref RL_VehicleKeyData> GetOwnedVehicles()
	{
		return ownedVehicles;
	}

	void SetOwnedVehicles(array<ref RL_VehicleKeyData> newOwnedVehicles)
	{
		ownedVehicles = newOwnedVehicles;
	}

	array<string> GetOwnedVehiclesAsStrings()
	{
		array<string> vehicleStrings = {};
		
		if (ownedVehicles)
		{
			foreach (RL_VehicleKeyData vehicleKey : ownedVehicles)
			{
				Print("[RL_CharacterAccount] GetOwnedVehiclesAsStrings foreach loop");
				if (vehicleKey)
					vehicleStrings.Insert(vehicleKey.ToReplicationString());
			}
		}
		
		return vehicleStrings;
	}

	static RL_CharacterAccount Create(string uid, string characterName, int age, string prefab)
	{
		RL_CharacterAccount character();
		character.uid = uid;
		character.name = characterName;
		character.age = age;
		character.prefab = prefab;
		return character;
	}

}
