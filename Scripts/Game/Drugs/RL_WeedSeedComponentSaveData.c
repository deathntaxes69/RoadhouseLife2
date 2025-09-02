[EPF_ComponentSaveDataType(RL_WeedSeedComponent), BaseContainerProps()]
class RL_WeedSeedComponentSaveDataClass: EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_WeedSeedComponentSaveData: EPF_ComponentSaveData {

  string m_sStrainName;
  float m_fQuality;
  int m_iYield;
  int m_iGrowSpeedInterval;
  ref array<string> m_aSuitableSoil;
  bool m_bIndoorOnly;
  string m_sColor;

  //------------------------------------------------------------------------------------------------
  override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_WeedSeedComponent weedSeed = RL_WeedSeedComponent.Cast(component);
    if (!weedSeed)
      return EPF_EReadResult.ERROR;

    m_sStrainName = weedSeed.GetStrainName();
    m_fQuality = weedSeed.GetQuality();
    m_iYield = weedSeed.GetYield();
    m_iGrowSpeedInterval = weedSeed.GetGrowSpeedInterval();
    m_aSuitableSoil = weedSeed.GetSuitableSoilTypes();
    m_bIndoorOnly = weedSeed.IsIndoorOnly();
    m_sColor = weedSeed.GetColor();
    
    return EPF_EReadResult.OK;
  }

  //------------------------------------------------------------------------------------------------
  override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes) {
    RL_WeedSeedComponent weedSeed = RL_WeedSeedComponent.Cast(component);
    if (!weedSeed)
      return EPF_EApplyResult.ERROR;
	
	  weedSeed.SetIndoorOnly(m_bIndoorOnly);
    weedSeed.SetQuality(m_fQuality);
    weedSeed.SetYield(m_iYield);
    weedSeed.SetGrowSpeedInterval(m_iGrowSpeedInterval);
    weedSeed.SetSuitableSoilTypes(m_aSuitableSoil);
    weedSeed.SetColor(m_sColor);
    
    if (!m_sStrainName.IsEmpty())
      weedSeed.SetStrainName(m_sStrainName);
    
    return EPF_EApplyResult.OK;
  }

  
  //------------------------------------------------------------------------------------------------
  override bool Equals(notnull EPF_ComponentSaveData other) {
    RL_WeedSeedComponentSaveData otherData = RL_WeedSeedComponentSaveData.Cast(other);
    if (!otherData)
      return false;
      
    return m_sStrainName == otherData.m_sStrainName &&
           m_fQuality == otherData.m_fQuality &&
           m_iYield == otherData.m_iYield &&
           m_iGrowSpeedInterval == otherData.m_iGrowSpeedInterval &&
           RL_Utils.IsArrayTheSame(m_aSuitableSoil, otherData.m_aSuitableSoil) &&
           m_bIndoorOnly == otherData.m_bIndoorOnly &&
           m_sColor == otherData.m_sColor;
  }
} 