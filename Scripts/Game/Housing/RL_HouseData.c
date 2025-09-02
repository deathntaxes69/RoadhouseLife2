class RL_HouseData : JsonApiStruct
{
	protected string uniqueId;
	protected string prefab;
	protected int buyPrice;
	protected int sellPrice;
	
	//------------------------------------------------------------------------------------------------
	void RL_HouseData()
	{
		RegV("uniqueId");
		RegV("prefab");
		RegV("buyPrice"); 
		RegV("sellPrice");
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUniqueId()
	{
		return uniqueId;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUniqueId(string value)
	{
		uniqueId = value;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetBuyPrice()
	{
		return buyPrice;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetBuyPrice(int value)
	{
		buyPrice = value;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetSellPrice()
	{
		return sellPrice;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSellPrice(int value)
	{
		sellPrice = value;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPrefab()
	{
		return prefab;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPrefab(string value)
	{
		prefab = value;
	}
}

class RL_OwnedHouseData : JsonApiStruct
{
	protected string uniqueId;
	protected int ownerCid;
	protected ref array<ref RL_BaseUpgradeJsonData> upgrades;
	protected ref array<ref RL_DynamicUpgradeJsonData> dynamicUpgrades;
	
	//------------------------------------------------------------------------------------------------
	void RL_OwnedHouseData()
	{
		RegV("uniqueId");
		RegV("ownerCid");
		RegV("upgrades");
		RegV("dynamicUpgrades");
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUniqueId()
	{
		return uniqueId;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerCid()
	{
		return ownerCid;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_BaseUpgradeJsonData> GetUpgrades()
	{
		return upgrades;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_DynamicUpgradeJsonData> GetDynamicUpgrades()
	{
		return dynamicUpgrades;
	}
}



class RL_HouseSyncResponse : JsonApiStruct
{
	protected bool success;
	protected string message;
	protected ref array<ref RL_OwnedHouseData> ownedHouses;
	
	//------------------------------------------------------------------------------------------------
	void RL_HouseSyncResponse()
	{
		RegV("success");
		RegV("message");
		RegV("ownedHouses");
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSuccess()
	{
		return success;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetMessage()
	{
		return message;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_OwnedHouseData> GetOwnedHouses()
	{
		return ownedHouses;
	}
}

class RL_HouseUpgradeRequestData
{
	int upgradeType;
	string upgradeName;
	int upgradePrice;
	string resourceNamePrefab;
}

class RL_HouseUpgradePurchaseResponse : RL_BaseUpgradePurchaseResponse
{
	//------------------------------------------------------------------------------------------------
	void RL_HouseUpgradePurchaseResponse()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUniqueId()
	{
		return GetPropertyId();
	}
}

class RL_DynamicUpgradeData
{
	string m_sUpgradeUid;
	string m_sPrefabPath;
	vector m_vPosition;
	vector m_vRotation;
	string m_sPlacedAt;
	
	void RL_DynamicUpgradeData()
	{
	}
	
	void SetData(string upgradeUid, string prefabPath, vector position, vector rotation, string placedAt)
	{
		m_sUpgradeUid = upgradeUid;
		m_sPrefabPath = prefabPath;
		m_vPosition = position;
		m_vRotation = rotation;
		m_sPlacedAt = placedAt;
	}
	
	string GetUpgradeUid() { return m_sUpgradeUid; }
	string GetPrefabPath() { return m_sPrefabPath; }
	vector GetPosition() { return m_vPosition; }
	vector GetRotation() { return m_vRotation; }
	string GetPlacedAt() { return m_sPlacedAt; }
}

class RL_DynamicUpgradeJsonData : JsonApiStruct
{
	protected string upgradeUid;
	protected string prefabPath;
	protected ref array<float> position;
	protected ref array<float> rotation;
	protected string placedAt;
	
	//------------------------------------------------------------------------------------------------
	void RL_DynamicUpgradeJsonData()
	{
		RegV("upgradeUid");
		RegV("prefabPath");
		RegV("position");
		RegV("rotation");
		RegV("placedAt");
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUpgradeUid()
	{
		return upgradeUid;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPrefabPath()
	{
		return prefabPath;
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetPosition()
	{
		if (!position || position.Count() < 3)
			return Vector(0, 0, 0);
		return Vector(position[0], position[1], position[2]);
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetRotation()
	{
		if (!rotation || rotation.Count() < 3)
			return Vector(0, 0, 0);
		return Vector(rotation[0], rotation[1], rotation[2]);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPlacedAt()
	{
		return placedAt;
	}
} 