[EPF_ComponentSaveDataType(RL_SeedBagGadgetComponent), BaseContainerProps()]
class RL_SeedBagComponentSaveDataClass: EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_SeedBagComponentSaveData: EPF_ComponentSaveData {
  int m_iSeedCount;

  override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_SeedBagGadgetComponent seedBag = RL_SeedBagGadgetComponent.Cast(component);
    if (!seedBag)
      return EPF_EReadResult.ERROR;

    m_iSeedCount = seedBag.GetSeedCount();
    return EPF_EReadResult.OK;
  }

  override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_SeedBagGadgetComponent seedBag = RL_SeedBagGadgetComponent.Cast(component);
    if (!seedBag)
      return EPF_EApplyResult.ERROR;

    seedBag.SetSeedCount(m_iSeedCount);
    return EPF_EApplyResult.OK;
  }

  override bool Equals(notnull EPF_ComponentSaveData other) {
    RL_SeedBagComponentSaveData otherData = RL_SeedBagComponentSaveData.Cast(other);
    return m_iSeedCount == otherData.m_iSeedCount;
  }
}