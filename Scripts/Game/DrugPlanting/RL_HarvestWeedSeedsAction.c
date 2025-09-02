class RL_HarvestWeedSeedsAction: SCR_ScriptedUserAction {

  //------------------------------------------------------------------------------------------------
  override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) {
    RL_GrowingComponent grow = RL_GrowingComponent.Cast(pOwnerEntity.FindComponent(RL_GrowingComponent));
    if (!grow || !grow.CanHarvest())
      return;

    InventoryStorageManagerComponent inv = EL_Component<InventoryStorageManagerComponent>.Find(pUserEntity);
    if (!inv)
      return;

    RL_WeedSeedComponent plantSeedComponent = RL_WeedSeedComponent.Cast(pOwnerEntity.FindComponent(RL_WeedSeedComponent));
    if (!plantSeedComponent)
      return;

    ResourceName seedBagPrefab = "{6A7E0BA984422860}Prefabs/DrugPlanting/Weed/Weed_Seed_Bag.et";
    array<RplId> addedItemIds = {};
    int itemsAdded = RL_InventoryUtils.AddAmountWithIds(inv, seedBagPrefab, 1, addedItemIds, true);
    
    if (itemsAdded > 0) {
      foreach (RplId itemId : addedItemIds) {
        Print("[RL_HarvestWeedSeedsAction] PerformAction foreach loop");
        IEntity seedBag = EPF_NetworkUtils.FindEntityByRplId(itemId);
        if (!seedBag) continue;
        
        RL_WeedSeedComponent seedBagComponent = RL_WeedSeedComponent.Cast(seedBag.FindComponent(RL_WeedSeedComponent));
        if (seedBagComponent) {
		      seedBagComponent.SetIndoorOnly(plantSeedComponent.IsIndoorOnly());
          seedBagComponent.SetQuality(plantSeedComponent.GetQuality());
          seedBagComponent.SetYield(plantSeedComponent.GetYield());
          seedBagComponent.SetGrowSpeedInterval(plantSeedComponent.GetGrowSpeedInterval());
          seedBagComponent.SetColor(plantSeedComponent.GetColor());
          seedBagComponent.ChangeStrainName(plantSeedComponent.GetStrainName());
        }

        RL_SeedBagGadgetComponent gadgetComponent = RL_SeedBagGadgetComponent.Cast(seedBag.FindComponent(RL_SeedBagGadgetComponent));
        if (gadgetComponent) {
          gadgetComponent.SetSeedCount(2);
        }
      }
    }

    RL_OwnerSpawnerReferenceComponent refComp = RL_OwnerSpawnerReferenceComponent.Cast(pOwnerEntity.FindComponent(RL_OwnerSpawnerReferenceComponent));
    if (refComp) {
      RL_PlantSpawnerComponent spawner = refComp.GetSpawner();
      if (spawner) {
        spawner.ResetPlanted();
      }
    }
    SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
  }

  //------------------------------------------------------------------------------------------------
  override bool CanBeShownScript(IEntity user) {
    return true;
  }

  //------------------------------------------------------------------------------------------------
  override bool CanBePerformedScript(IEntity user) {
    if (!GetOwner())
      return false;

    RL_GrowingComponent grow = RL_GrowingComponent.Cast(GetOwner().FindComponent(RL_GrowingComponent));
    if (!grow)
      return false;

    if (grow.CanHarvest())
      return true;

    int currentStage = grow.GetGrowthStage();
    int maxStage = grow.GetMaxGrowthStage();
    string progressText = string.Format("Not ready - Stage %1/%2", currentStage + 1, maxStage);
    
    if (grow.IsWatered())
      progressText += " [Watered]";
    
    SetCannotPerformReason(progressText);
    return false;
  }

  //------------------------------------------------------------------------------------------------
  override bool GetActionNameScript(out string outName) {
    outName = "Harvest Seeds";
    return true;
  }
} 