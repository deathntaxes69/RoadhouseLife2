class RL_GarageItem : JsonApiStruct
{
    int id;
    string characterId;
    string prefab;
    int active;
    int vehiclePrice;
    string vehicleColors;

    void RL_GarageItem()
    {
        RegV("id");
        RegV("characterId");
        RegV("prefab");
		RegV("active");
        RegV("vehiclePrice");
        RegV("vehicleColors");
    }
    int GetVehicleId()
    {
        return id;
    }
    string GetCharacterId()
    {
        return characterId;
    }
	string GetPrefab()
	{
		return prefab;
	}
    int GetStatus()
	{
		return active;
	}
    
    int GetVehiclePrice()
    {
        return vehiclePrice;
    }
    
    string GetVehicleColors()
    {
        return vehicleColors;
    }
    
    static RL_GarageItem Create(string characterId, string prefab)
	{
		RL_GarageItem vehicle();
        vehicle.characterId = characterId;
		vehicle.prefab = prefab;
        vehicle.active = 1;
        vehicle.vehicleColors = "";

		return vehicle;
	}
}
class RL_GarageArray : JsonApiStruct
{
    ref array<ref RL_GarageItem> data;

    void RL_GarageArray()
    {
        RegV("data");
    }
}
