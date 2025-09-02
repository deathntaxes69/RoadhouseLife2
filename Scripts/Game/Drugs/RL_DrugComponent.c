[ComponentEditorProps(category: "RL/Drugs", description: "Base component for all drugs that tracks quality")]
class RL_DrugComponentClass: ScriptComponentClass {}

class RL_DrugComponent: ScriptComponent {

  [Attribute(defvalue: "1.0", desc: "Quality of the drug (0.0 - 1.0)"), RplProp()]
  protected float m_fQuality;

  [Attribute(defvalue: "", desc: "Drug Name"), RplProp()]
  protected string m_sName;



  //------------------------------------------------------------------------------------------------
  float GetQuality() {
    return m_fQuality;
  }

  //------------------------------------------------------------------------------------------------
  void SetQuality(float quality) {
    m_fQuality = Math.Clamp(quality, 0.0, 1.0);
    Replication.BumpMe();
  }

  //------------------------------------------------------------------------------------------------
  string GetName() {
    return m_sName;
  }

  //------------------------------------------------------------------------------------------------
  void SetName(string name) {
    m_sName = name;
    Replication.BumpMe();
  }

  //------------------------------------------------------------------------------------------------
  void SetDrugInfo(string name, float quality) {
    m_sName = name;
    m_fQuality = Math.Clamp(quality, 0.0, 1.0);
    Replication.BumpMe();
  }
} 