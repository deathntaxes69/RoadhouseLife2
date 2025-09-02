[EPF_ComponentSaveDataType(RL_MetabolismComponent), BaseContainerProps()]
class RL_MetabolismComponentSaveDataClass: EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_MetabolismComponentSaveData: EPF_ComponentSaveData {
  float m_fFoodLevel;
  float m_fThirstLevel;
  float m_fHealthLevel;

  override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    auto metabolismComp = RL_MetabolismComponent.Cast(component);
    if (!metabolismComp)
      return EPF_EReadResult.ERROR;

    auto damageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
    if (!damageManager)
      return EPF_EReadResult.ERROR;

    m_fFoodLevel = metabolismComp.GetFoodLevel();
    m_fThirstLevel = metabolismComp.GetThirstLevel();
    m_fHealthLevel = damageManager.GetHealth() / damageManager.GetMaxHealth();
    return EPF_EReadResult.OK;
  }

  override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    auto metabolismComp = RL_MetabolismComponent.Cast(component);
    if (!metabolismComp)
      return EPF_EApplyResult.ERROR;

    auto damageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
    if (!damageManager)
      return EPF_EApplyResult.ERROR;

    metabolismComp.SetFoodLevel(m_fFoodLevel);
    metabolismComp.SetThirstLevel(m_fThirstLevel);
    damageManager.SetHealthScaled(m_fHealthLevel);
    return EPF_EApplyResult.OK;
  }

  override bool Equals(notnull EPF_ComponentSaveData other) {
    RL_MetabolismComponentSaveData otherData = RL_MetabolismComponentSaveData.Cast(other);
    return m_fFoodLevel == otherData.m_fFoodLevel && m_fThirstLevel == otherData.m_fThirstLevel && m_fHealthLevel == otherData.m_fHealthLevel;
  }
}