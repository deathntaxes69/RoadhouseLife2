class RL_WateringCanComponentClass: ScriptComponentClass {}

class RL_WateringCanComponent: ScriptComponent {
  [Attribute(defvalue: "100", desc: "Max water capacity")]
  protected int m_iMaxWater;

  [Attribute(defvalue: "100", desc: "Initial water amount"), RplProp()]
  protected int m_iCurrentWater;

  override void OnPostInit(IEntity owner) {
    super.OnPostInit(owner);
    SetEventMask(owner, EntityEvent.INIT);
  }

  override void EOnInit(IEntity owner) {}

  bool UseWater(int amount = 1) {
    if (m_iCurrentWater < amount)
      return false;

    m_iCurrentWater -= amount;
    Replication.BumpMe();
    return true;
  }

  int GetWaterAmount() {
    return m_iCurrentWater;
  }

  void SetWaterAmount(int amount) {
    m_iCurrentWater = Math.Clamp(amount, 0, m_iMaxWater);
    Replication.BumpMe();
  }

  int GetMaxWaterAmount() {
    return m_iMaxWater;
  }
}