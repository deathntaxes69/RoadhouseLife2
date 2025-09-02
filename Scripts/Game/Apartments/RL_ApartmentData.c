class RL_ApartmentData : JsonApiStruct
{
	protected string apartment;
	protected int ownerCid;
	protected int buyPrice;
	protected int sellPrice;
	protected ref array<ref RL_BaseUpgradeJsonData> upgrades;
	
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentData()
	{
		RegV("apartment");
		RegV("ownerCid");
		RegV("buyPrice"); 
		RegV("sellPrice");
		RegV("upgrades");
	}
	
	//------------------------------------------------------------------------------------------------
	string GetApartment()
	{
		return apartment;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetApartment(string value)
	{
		apartment = value;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerCid()
	{
		return ownerCid;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetOwnerCid(int value)
	{
		ownerCid = value;
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
	array<ref RL_BaseUpgradeJsonData> GetUpgrades()
	{
		return upgrades;
	}
}



class RL_ApartmentGetResponse : JsonApiStruct
{
	protected bool success;
	protected string message;
	protected string apartment;
	protected int buyPrice;
	protected int sellPrice;
	protected int ownerCid;
	protected ref array<ref RL_BaseUpgradeJsonData> upgrades;
	
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentGetResponse()
	{
		RegV("success");
		RegV("message");
		RegV("apartment");
		RegV("buyPrice");
		RegV("sellPrice");
		RegV("ownerCid");
		RegV("upgrades");
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
	string GetApartment()
	{
		return apartment;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetBuyPrice()
	{
		return buyPrice;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetSellPrice()
	{
		return sellPrice;
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
}

class RL_ApartmentUpgradeRequestData
{
	int upgradeType;
	string upgradeName;
	int upgradePrice;
	string resourceNamePrefab;
}

class RL_ApartmentUpgradePurchaseResponse : RL_BaseUpgradePurchaseResponse
{
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentUpgradePurchaseResponse()
	{
		// base class handles all the common fields
	}
	
	//------------------------------------------------------------------------------------------------
	string GetApartment()
	{
		return GetPropertyId();
	}
}

class RL_ApartmentBuyResponse : RL_BasePropertyPurchaseResponse
{
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentBuyResponse()
	{
		// baseD class handles all the common fields
	}
	
	//------------------------------------------------------------------------------------------------
	string GetApartment()
	{
		return GetPropertyId();
	}
}

// Apartment sell response - extends base class
class RL_ApartmentSellResponse : RL_BasePropertyPurchaseResponse
{
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentSellResponse()
	{
		// borat handles all the common fields
	}
	
	//------------------------------------------------------------------------------------------------
	string GetApartment()
	{
		return GetPropertyId();
	}
}

class RL_ApartmentOwnershipResponse : JsonApiStruct
{
	protected bool success;
	protected bool ownsApartment;
	protected string apartment;
	protected string error;
	
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentOwnershipResponse()
	{
		RegV("success");
		RegV("ownsApartment");
		RegV("apartment");
		RegV("error");
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSuccess()
	{
		return success;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetOwnsApartment()
	{
		return ownsApartment;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetApartment()
	{
		return apartment;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetError()
	{
		return error;
	}
} 