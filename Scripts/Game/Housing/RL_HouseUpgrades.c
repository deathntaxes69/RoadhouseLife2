enum HouseUpgrades
{
	CRAFTING,
	STORAGE,
	PLAYER_SPAWN,
	VEHICLE_SPAWN,
	STORAGE_APARTMENT,
	CRAFTING_APARTMENT
}

[BaseContainerProps(configRoot: true)]
class RL_HouseUpgradeSpawnPoint
{
	[Attribute("0", UIWidgets.ComboBox, "Type of upgrade for this spawn point", "", ParamEnumArray.FromEnum(HouseUpgrades))]
	HouseUpgrades m_eUpgradeType;
	
	[Attribute("", UIWidgets.Object, "Spawn position")]
	ref PointInfo m_SpawnPoint;
	
	HouseUpgrades GetUpgradeType() { return m_eUpgradeType; }
	PointInfo GetSpawnPoint() { return m_SpawnPoint; }
} 