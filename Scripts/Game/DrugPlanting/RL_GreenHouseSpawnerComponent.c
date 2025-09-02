class RL_PlantSpawnerComponentClass: ScriptComponentClass {}

class RL_PlantSpawnerComponent: ScriptComponent {
  [RplProp()]
  bool alreadyPlanted;

  [Attribute(desc: "Prefab to spawn")]
  ResourceName m_GreenhousePrefab;

  [RplRpc(RplChannel.Reliable, RplRcver.Server)]
  void RpcAsk_SpawnGreenhouse(vector spawnPos) {
    Print("RpcAsk_SpawnGreenhouse");
    if (alreadyPlanted)
      return;

    EntitySpawnParams params = new EntitySpawnParams();
    vector transform[4];
    Math3D.MatrixIdentity4(transform);
    transform[3] = spawnPos;
    params.Transform = transform;
    params.TransformMode = ETransformMode.WORLD;
    params.Parent = GetOwner();

    IEntity spawned = GetGame().SpawnEntityPrefab(Resource.Load(m_GreenhousePrefab), GetGame().GetWorld(), params);
    if (spawned) {
      RplComponent rpl = RplComponent.Cast(spawned.FindComponent(RplComponent));

      RL_OwnerSpawnerReferenceComponent refComp = RL_OwnerSpawnerReferenceComponent.Cast(spawned.FindComponent(RL_OwnerSpawnerReferenceComponent));
      if (refComp)
        refComp.SetSpawner(this);

      alreadyPlanted = true;
      Replication.BumpMe();
    }
  }

  void ResetPlanted() {
    alreadyPlanted = false;
    Replication.BumpMe();
  }
}