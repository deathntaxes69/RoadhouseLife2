[BaseContainerProps()]
class RL_HouseUpgradeDefinition
{
    [Attribute(defvalue: "0", category: "Info", desc: "Upgrade Type", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(HouseUpgrades))]
    HouseUpgrades upgradeType;
    
    [Attribute(defvalue: "", category: "Info", desc: "Upgrade Name")]
    string name;
    
    [Attribute(defvalue: "", category: "Info", desc: "Description")]
    string description;
    
    [Attribute(defvalue: "1000", category: "Cost", desc: "Buy Price")]
    int buyPrice;
    
    [Attribute(defvalue: "", category: "Prefab", desc: "Resource Name Prefab")]
    string resourceNamePrefab;
    
    HouseUpgrades GetUpgradeType() { return upgradeType; }
    string GetName() { return name; }
    string GetDescription() { return description; }
    int GetBuyPrice() { return buyPrice; }
    string GetResourceNamePrefab() { return resourceNamePrefab; }
} 