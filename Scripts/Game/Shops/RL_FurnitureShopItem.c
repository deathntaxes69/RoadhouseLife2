class RL_FurnitureShopItem : RL_BaseShopItem
{
    string previewPrefab;

    void RL_FurnitureShopItem()
    {
        RegV("previewPrefab");
    }
    
    string GetPreviewPrefab()
	{
		return previewPrefab;
	}
}

class RL_FurnitureShopArray : JsonApiStruct
{
    ref array<ref RL_FurnitureShopItem> data;

    void RL_FurnitureShopArray()
    {
        RegV("data");
    }
}