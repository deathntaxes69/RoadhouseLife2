[BaseContainerProps(configRoot: true)]
class RL_HouseUpgradesConfig : Managed
{	
	[Attribute(defvalue: "", category: "All House Upgrades", desc: "All House Upgrades")]
	ref array<ref RL_HouseUpgradeDefinition> Upgrades;
	
	RL_HouseUpgradeDefinition FindUpgradeByType(HouseUpgrades upgradeType)
	{
		foreach (RL_HouseUpgradeDefinition upgrade : Upgrades)
		{
			Print("[RL_HouseUpgradesConfig] FindUpgradeByType foreach loop");
			if (upgrade.GetUpgradeType() == upgradeType)
				return upgrade;
		}
		return null;
	}
	
	array<ref RL_HouseUpgradeDefinition> GetUpgradesByType(HouseUpgrades upgradeType)
	{
		array<ref RL_HouseUpgradeDefinition> result = {};
		foreach (RL_HouseUpgradeDefinition upgrade : Upgrades)
		{
			Print("[RL_HouseUpgradesConfig] GetUpgradesByType foreach loop");
			if (upgrade.GetUpgradeType() == upgradeType)
				result.Insert(upgrade);
		}
		return result;
	}
	
	array<ref RL_HouseUpgradeDefinition> GetAllUpgrades()
	{
		return Upgrades;
	}
}