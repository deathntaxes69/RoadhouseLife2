[EPF_ComponentSaveDataType(RL_WateringCanComponent), BaseContainerProps()]
class RL_WateringCanSaveDataClass: EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_WateringCanSaveData: EPF_ComponentSaveData {
  int m_iCurrentWater;

  override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_WateringCanComponent can = RL_WateringCanComponent.Cast(component);
    if (!can)
      return EPF_EReadResult.ERROR;

    m_iCurrentWater = can.GetWaterAmount();
    return EPF_EReadResult.OK;
  }

  override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_WateringCanComponent can = RL_WateringCanComponent.Cast(component);
    if (!can)
      return EPF_EApplyResult.ERROR;

    can.SetWaterAmount(m_iCurrentWater);
    return EPF_EApplyResult.OK;
  }

  override bool Equals(notnull EPF_ComponentSaveData other) {
    RL_WateringCanSaveData otherData = RL_WateringCanSaveData.Cast(other);
    return m_iCurrentWater == otherData.m_iCurrentWater;
  }
}