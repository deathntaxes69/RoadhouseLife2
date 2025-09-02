[BaseContainerProps()]
class RL_CraftingSkillRequirement
{
	[Attribute(defvalue: "1", category: "Skill", desc: "Skill Category", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RL_SkillsCategory))]
	RL_SkillsCategory skillCategory;
	
	[Attribute(defvalue: "0", category: "Skill", desc: "Required Level")]
	int skillLevel;
}