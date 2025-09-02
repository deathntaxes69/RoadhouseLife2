enum EDrugs
{
	WEED = 1,
	COCAINE = 2,
	HEROIN = 3
};

class RL_DrugUtils
{
	static void GetDrugProperties(EDrugs drug, out string name, out int pricePerUnit, out string prefab)
	{
		switch (drug)
		{
			case EDrugs.WEED:
				name = "Weed";
				pricePerUnit = 215;
				prefab = "{3FE46DD7F49E69E4}Prefabs/Supplies/ProcessedWeed.et";
				break;
			case EDrugs.COCAINE:
				name = "Cocaine";
				pricePerUnit = 450;
				prefab = "{58E47FAA721D5EEC}Prefabs/Supplies/Cocaine_Brick.et";
				break;
			case EDrugs.HEROIN:
				name = "Heroin";
				pricePerUnit = 300;
				prefab = "{D0EAC6344C6CCAC8}Prefabs/Supplies/Heroin_Brick.et";
				break;
			default:
				name = "Unknown";
				pricePerUnit = 0;
				prefab = "";
				break;
		}
	}
}

// Change where this is used to use the class above later :p
string GetDrugName(EDrugs drug)
{
	string name, prefab;
	int price;
	RL_DrugUtils.GetDrugProperties(drug, name, price, prefab);
	return name;
}

// Change where this is used to use the class above later :p
int GetDrugPricePerUnit(EDrugs drug)
{
	string name, prefab;
	int price;
	RL_DrugUtils.GetDrugProperties(drug, name, price, prefab);
	return price;
}

// Change where this is used to use the class above later :p
string GetDrugPrefab(EDrugs drug)
{
	string name, prefab;
	int price;
	RL_DrugUtils.GetDrugProperties(drug, name, price, prefab);
	return prefab;
}