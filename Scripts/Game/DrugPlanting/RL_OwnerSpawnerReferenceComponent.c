class RL_OwnerSpawnerReferenceComponentClass: ScriptComponentClass {}

class RL_OwnerSpawnerReferenceComponent: ScriptComponent {
  protected RL_PlantSpawnerComponent m_Spawner;

  void SetSpawner(RL_PlantSpawnerComponent spawner) {
    m_Spawner = spawner;
  }

  RL_PlantSpawnerComponent GetSpawner() {
    return m_Spawner;
  }
}