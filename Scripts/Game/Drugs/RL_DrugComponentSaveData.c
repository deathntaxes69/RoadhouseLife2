[EPF_ComponentSaveDataType(RL_DrugComponent), BaseContainerProps()]
class RL_DrugComponentSaveDataClass: EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_DrugComponentSaveData: EPF_ComponentSaveData {
    
  float m_fQuality;
  string m_sName;

  //------------------------------------------------------------------------------------------------
  override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_DrugComponent drugComponent = RL_DrugComponent.Cast(component);
    if (!drugComponent)
      return EPF_EReadResult.ERROR;

    m_fQuality = drugComponent.GetQuality();
    m_sName = drugComponent.GetName();
    return EPF_EReadResult.OK;
  }

  //------------------------------------------------------------------------------------------------
  override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_DrugComponent drugComponent = RL_DrugComponent.Cast(component);
    if (!drugComponent)
      return EPF_EApplyResult.ERROR;

    if (m_sName != "")
      drugComponent.SetDrugInfo(m_sName, m_fQuality);
    else
      drugComponent.SetQuality(m_fQuality);
    
    return EPF_EApplyResult.OK;
  }

  //------------------------------------------------------------------------------------------------
  override bool Equals(notnull EPF_ComponentSaveData other) {
    RL_DrugComponentSaveData otherData = RL_DrugComponentSaveData.Cast(other);
    return m_fQuality == otherData.m_fQuality && 
           m_sName == otherData.m_sName;
  }
} 