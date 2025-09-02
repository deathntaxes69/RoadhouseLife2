[BaseContainerProps(configRoot: true)]
class RL_CraftingRecipeList : Managed
{	
	[Attribute(defvalue: "", category: "All Recipes", desc: "All Recipes")]
	ref array<ref RL_CraftingRecipe> Recipes;
}
