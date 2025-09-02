// DEPRECATED/ORPHANED
class RL_SeedBagComponentClass: ScriptComponentClass {}

class RL_SeedBagComponent: ScriptComponent {
  [Attribute(defvalue: "5", desc: "Initial seed count"), RplProp()]
  protected int m_iSeedCount;

  override void OnPostInit(IEntity owner) {
    super.OnPostInit(owner);
    SetEventMask(owner, EntityEvent.INIT);
  }

  override void EOnInit(IEntity owner) {}

  bool UseSeed() {
    if (m_iSeedCount <= 0)
      return false;

    m_iSeedCount--;
    Replication.BumpMe();
    return true;
  }

  bool IsEmpty() {
    return m_iSeedCount <= 0;
  }

  int GetSeedCount() {
    return m_iSeedCount;
  }

  void SetSeedCount(int count) {
    m_iSeedCount = count;
    Replication.BumpMe();
  }
}