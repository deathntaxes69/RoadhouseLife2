[BaseContainerProps(configRoot: true)]
class RL_CraftingCategoryList : Managed
{	
	[Attribute(defvalue: "", category: "All Categories", desc: "All Categories")]
	ref array<ref string> Categories;
}