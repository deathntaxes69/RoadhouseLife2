class RL_FishingSenderComponentClass extends ScriptComponentClass {}

class RL_FishingSenderComponent extends ScriptComponent {
	private bool m_IsFishing = false;

	void SetFishing(bool state) {
		m_IsFishing = state;
	}

	bool IsFishing() {
		return m_IsFishing;
	}

	void AddToInventory(ResourceName resource) {
		RplComponent rplComp = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		Rpc(RpcDo_AddToInventory, rplComp.Id(), resource);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcDo_AddToInventory(RplId repId, ResourceName resourceName) {
		Print("[RL_FishingSenderComponent] RpcDo_AddToInventory");
		RplComponent ownerRplComp = RplComponent.Cast(Replication.FindItem(repId));
		IEntity owner = ownerRplComp.GetEntity();
		SCR_InventoryStorageManagerComponent storageManager = SCR_InventoryStorageManagerComponent.Cast(owner.FindComponent(SCR_InventoryStorageManagerComponent));

		Resource resource = Resource.Load(resourceName);
		if (!resource) return;

		EntitySpawnParams spawnParams();
		spawnParams.Transform[3] = storageManager.GetOwner().GetOrigin();

		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), spawnParams);
		if (!entity) return;

		bool insertResult = storageManager.TryInsertItem(entity);
		if (!insertResult)
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
	}
}
