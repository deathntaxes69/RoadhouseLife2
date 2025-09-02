[BaseContainerProps()]
class RL_CraftingIngredient
{	
	[Attribute(defvalue: "", category: "Item", desc: "Name")]
    protected string name;
	
	[Attribute(defvalue: "", category: "Item", desc: "Item")]
    protected ResourceName prefab;
	
	[Attribute(defvalue: "", category: "Item", desc: "Quantity")]
    protected int quantity;

    string GetName()
    {
        return name;
    }
    string GetPrefab()
    {
        return prefab;
    }
    int GetQuantity()
    {
        return quantity;
    }
    static RL_CraftingIngredient Create(string ingredientName, string prefab, int quantity)
    {
        RL_CraftingIngredient ingredient();
		ingredient.name = ingredientName;
        ingredient.prefab = prefab;
        ingredient.quantity = quantity;
        return ingredient;
    }
}