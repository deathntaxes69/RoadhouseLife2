[BaseContainerProps()]
class RL_PerkDefinition
{
    [Attribute(defvalue: "", category: "Basic", desc: "Perk ID")]
    string perkId;
    
    [Attribute(defvalue: "", category: "Basic", desc: "Name")]
    string name;
    
    [Attribute(defvalue: "", category: "Basic", desc: "Description")]
    string description;
    
    [Attribute(defvalue: "1", category: "Requirements", desc: "Perk Category", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RL_SkillsCategory))]
    RL_SkillsCategory category;
    
    [Attribute(defvalue: "1", category: "Requirements", desc: "Required Skill Level")]
    int requiredSkillLevel;
    
    [Attribute(defvalue: "1", category: "Cost", desc: "Perk Point Cost")]
    int perkPointCost;
    
    [Attribute(defvalue: "", category: "Visual", desc: "Icon name from imageset")]
    string icon;
    
    [Attribute(defvalue: "", category: "Prerequisites", desc: "Required Perks")]
    ref array<string> prerequisitePerks;
}
