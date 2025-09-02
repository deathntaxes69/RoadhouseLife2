class RL_VehicleKeyData : JsonApiStruct
{
    int vehicleId;
    string vehicleName;

    void RL_VehicleKeyData()
    {
        RegV("vehicleId");
        RegV("vehicleName");
    }

    int GetVehicleId()
    {
        return vehicleId;
    }

    string GetVehicleName()
    {
        return vehicleName;
    }

    void SetVehicleId(int id)
    {
        vehicleId = id;
    }

    void SetVehicleName(string name)
    {
        vehicleName = name;
    }

    static RL_VehicleKeyData Create(int id, string name)
    {
        RL_VehicleKeyData vehicleKey();
        vehicleKey.vehicleId = id;
        vehicleKey.vehicleName = name;
        return vehicleKey;
    }

    string ToReplicationString()
    {
        return vehicleId.ToString() + ":" + vehicleName;
    }

    static RL_VehicleKeyData FromReplicationString(string data)
    {
        array<string> parts = {};
        data.Split(":", parts, true);
        
        if (parts.Count() >= 2)
        {
            int id = parts[0].ToInt();
            string name = parts[1];
            return Create(id, name);
        }
        
        return null;
    }
}