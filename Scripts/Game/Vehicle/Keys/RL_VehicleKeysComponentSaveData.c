[EPF_ComponentSaveDataType(RL_VehicleKeysComponent), BaseContainerProps()]
class RL_VehicleKeysComponentSaveDataClass: EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_VehicleKeysComponentSaveData: EPF_ComponentSaveData {
  
  ref array<string> m_aOwnedVehicleKeys;

  override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    auto keysComp = RL_VehicleKeysComponent.Cast(component);
    if (!keysComp)
      return EPF_EReadResult.ERROR;
      
    m_aOwnedVehicleKeys = keysComp.GetOwnedVehicleKeyStrings();
    return EPF_EReadResult.OK;
  }

  override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    auto keysComp = RL_VehicleKeysComponent.Cast(component);
    if (!keysComp)
      return EPF_EApplyResult.ERROR;

    keysComp.SetVehicleKeys(m_aOwnedVehicleKeys);
    return EPF_EApplyResult.OK;
  }

  override bool Equals(notnull EPF_ComponentSaveData other) {
    RL_VehicleKeysComponentSaveData otherData = RL_VehicleKeysComponentSaveData.Cast(other);
    return RL_Utils.IsArrayTheSame(m_aOwnedVehicleKeys, otherData.m_aOwnedVehicleKeys);
  }
}