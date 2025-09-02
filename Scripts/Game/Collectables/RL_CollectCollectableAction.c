[BaseContainerProps]
class RL_CollectCollectableAction : ScriptedUserAction
{
	
	[Attribute("", desc: "Collectable Name")]
	protected string m_collectableName;
	
	[Attribute("", desc: "Collectable Value")]
	protected int m_collectableValue;
	
	[Attribute(defvalue: "1", category: "Reward", desc: "XP Category", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RL_SkillsCategory))]
	protected RL_SkillsCategory m_eXpCategory;
	
	[Attribute("0", desc: "XP Reward Amount")]
	protected int m_xpReward;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		character.CollectCollectable(m_collectableName, m_collectableName, m_collectableValue, m_eXpCategory, m_xpReward);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}