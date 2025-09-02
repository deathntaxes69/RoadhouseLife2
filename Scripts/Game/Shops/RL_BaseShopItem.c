class RL_BaseShopItem : JsonApiStruct
{
    string prefab;
    int price;
    int sellPrice;
    int category = 1;
    int reqRank = 0;
    int reqDept = 0;

    void RL_BaseShopItem()
    {
        RegV("prefab");
        RegV("price");
        RegV("sellPrice");
        RegV("category");
        RegV("reqRank");
        RegV("reqDept");
    }
	string GetPrefab()
	{
		return prefab;
	}
    int GetPrice()
	{
		return price;
	}
    int GetSellPrice()
	{
		return sellPrice;
	}
    int GetCategory()
	{
        if(!category)
            return 1;
            
		return category;
	}
    int GetRequiredRank()
	{
		return reqRank;
	}
    int GetRequiredDept()
	{
		return reqDept;
	}
}
class RL_BaseShopArray : JsonApiStruct
{
    ref array<ref RL_BaseShopItem> data;

    void RL_BaseShopArray()
    {
        RegV("data");
    }
}
