[ComponentEditorProps(category: "RL/Drugs", description: "Component for weed seeds with specialized growing properties")]
class RL_WeedSeedComponentClass: ScriptComponentClass {}

class RL_WeedSeedComponent: ScriptComponent {

  [Attribute(defvalue: "Unidentified Strain", desc: "Strain Name"), RplProp()]
  protected string m_sStrainName;

  [Attribute(defvalue: "0.2", desc: "Quality of the weed seed"), RplProp()]
  protected float m_fQuality;

  [Attribute(defvalue: "1", desc: "Expected yield from this seed"), RplProp()]
  protected int m_iYield;

  [Attribute(defvalue: "420", desc: "Grow speed interval in seconds"), RplProp()]
  protected int m_iGrowSpeedInterval;

  [Attribute(defvalue: "{}", desc: "Suitable soil types for growing"), RplProp()]
  protected ref array<string> m_aSuitableSoil;

  [Attribute(defvalue: "0", desc: "Whether this seed can only be grown indoors"), RplProp()]
  protected bool m_bIndoorOnly;

  [Attribute(defvalue: "green", desc: "Color of the weed"), RplProp()]
  protected string m_sColor;

  //------------------------------------------------------------------------------------------------
  void RL_WeedSeedComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
    if (!m_aSuitableSoil)
      m_aSuitableSoil = {};
  }

  //------------------------------------------------------------------------------------------------
  float GetQuality() {
    return m_fQuality;
  }

   //------------------------------------------------------------------------------------------------
  void SetQuality(float quality) {
    // can delete on seed wipe and set to parameter without changes
    float maxQuality;
    if (m_bIndoorOnly) {
      maxQuality = 1.0;
    } else {
      maxQuality = 0.7;
    }
    m_fQuality = Math.Clamp(quality, 0.0, maxQuality);
  }

  //------------------------------------------------------------------------------------------------
  string GetStrainName() {
    return m_sStrainName;
  }

  //------------------------------------------------------------------------------------------------
  void SetStrainName(string strainName) {
    m_sStrainName = strainName;
  }

  //------------------------------------------------------------------------------------------------
  void ChangeStrainName(string strainName) {
    m_sStrainName = strainName;
    Replication.BumpMe();
  }

  //------------------------------------------------------------------------------------------------
  void ChangeUIInfo(string strainName, string description) {
    m_sStrainName = strainName;
    Replication.BumpMe();
  }

  //------------------------------------------------------------------------------------------------
  int GetYield() {
    return m_iYield;
  }

  //------------------------------------------------------------------------------------------------
  void SetYield(int yield) {
    // can delete on seed wipe and set to parameter without changes
    int maxYield;
    if (m_bIndoorOnly) {
      maxYield = 12;
    } else {
      maxYield = 6;
    }
    m_iYield = Math.ClampInt(yield, 1, maxYield);
  }

  //------------------------------------------------------------------------------------------------
  int GetGrowSpeedInterval() {
    return m_iGrowSpeedInterval;
  }

  //------------------------------------------------------------------------------------------------
  void SetGrowSpeedInterval(int interval) {
    // can delete on seed wipe and set to parameter without changes
    int minGrowSpeed;
    if (m_bIndoorOnly) {
    minGrowSpeed = 200;
    } else {
      minGrowSpeed = 260;
    }
    m_iGrowSpeedInterval = Math.ClampInt(interval, minGrowSpeed, 600);
  }

  //------------------------------------------------------------------------------------------------
  array<string> GetSuitableSoilTypes() {
    if (!m_aSuitableSoil)
      m_aSuitableSoil = {};
    return m_aSuitableSoil;
  }

  //------------------------------------------------------------------------------------------------
  void SetSuitableSoilTypes(array<string> soilTypes) {
    m_aSuitableSoil = soilTypes;
  }

  //------------------------------------------------------------------------------------------------
  bool IsIndoorOnly() {
    return m_bIndoorOnly;
  }

  //------------------------------------------------------------------------------------------------
  void SetIndoorOnly(bool indoorOnly) {
    m_bIndoorOnly = indoorOnly;
  }

  //------------------------------------------------------------------------------------------------
  string GetColor() {
    return m_sColor;
  }

  //------------------------------------------------------------------------------------------------
  void SetColor(string color) {
    if (color == "green" || color == "purple")
      m_sColor = color;
    else
      m_sColor = "green";
  }
} 