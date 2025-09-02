[BaseContainerProps(configRoot: true)]
class RL_PerksConfig : Managed
{	
	[Attribute(defvalue: "", category: "All Perks", desc: "All Perks")]
	ref array<ref RL_PerkDefinition> Perks;
} 