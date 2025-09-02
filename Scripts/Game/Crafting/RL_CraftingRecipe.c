[BaseContainerProps(configRoot: true)]
class RL_CraftingRecipe
{	
	
	[Attribute(defvalue: "", category: "Output", desc: "Name")];
	string name;
	
	[Attribute(defvalue: "", category: "Output", desc: "Output Item")]
    ResourceName outputItem;
	
	[Attribute(defvalue: "", category: "Output", desc: "Output Quantity")]
    int outputQuantity;
	
	[Attribute(defvalue: "", category: "Recipe", desc: "Ingredients")]
    ref array<ref RL_CraftingIngredient> ingredients;
	
	[Attribute(defvalue: "", category: "Recipe", desc: "Crafting Stations")]
    ref array<string> craftingStations;
	
	[Attribute(defvalue: "", category: "Recipe", desc: "Required Skills")]
    ref array<ref RL_CraftingSkillRequirement> skills;
	
	[Attribute(defvalue: "", category: "Recipe", desc: "XP Gain")]
    int xpGain;
	
	[Attribute(defvalue: "", category: "Crafting Time (seconds)", desc: "Crafting Time (seconds)")]
    int craftingTime;
	
	[Attribute(defvalue: "", category: "Category", desc: "Category")];
	string category;
	
	[Attribute(defvalue: "-1", category: "Recipe Requirements", desc: "Recipe ID (-1 = no recipe required)")];
	int recipeId;
	
	[Attribute(defvalue: "", category: "Recipe Requirements", desc: "Recipe Name")]
	string recipeName;

}