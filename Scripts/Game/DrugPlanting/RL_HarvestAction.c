class RL_HarvestAction: SCR_ScriptedUserAction {
  [Attribute(desc: "How many should be harvested")]
  private int amount;

  [Attribute(desc: "Item that will be given", category: "General")]
  private ResourceName m_givenItem;

  override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) {
    RL_GrowingComponent grow = RL_GrowingComponent.Cast(pOwnerEntity.FindComponent(RL_GrowingComponent));
    if (!grow || !grow.CanHarvest())
      return;

    InventoryStorageManagerComponent inv = EL_Component<InventoryStorageManagerComponent>.Find(pUserEntity);
    if (!inv)
      return;

    int harvestAmount = amount;
    float quality = 0.2;
    string strainName = "";
    RL_WeedSeedComponent seedComponent = RL_WeedSeedComponent.Cast(pOwnerEntity.FindComponent(RL_WeedSeedComponent));
    if (seedComponent) {
      harvestAmount = seedComponent.GetYield();
      quality = seedComponent.GetQuality();
      strainName = seedComponent.GetStrainName();
    }

    array<RplId> addedItemIds = {};
    int itemsAdded = RL_InventoryUtils.AddAmountWithIds(inv, m_givenItem, harvestAmount, addedItemIds, true);
    
    foreach (RplId itemId : addedItemIds) {
      Print("[RL_HarvestAction] PerformAction foreach loop");
      IEntity harvested = EPF_NetworkUtils.FindEntityByRplId(itemId);
      if (!harvested) continue;
  
      RL_DrugComponent drugComponent = RL_DrugComponent.Cast(harvested.FindComponent(RL_DrugComponent));
      if (drugComponent) {
        drugComponent.SetDrugInfo(strainName, quality);
        //Replication.BumpMe();
      }

      RL_WeedSeedComponent harvestedSeedComponent = RL_WeedSeedComponent.Cast(harvested.FindComponent(RL_WeedSeedComponent));
      if (harvestedSeedComponent && seedComponent) {
        harvestedSeedComponent.SetIndoorOnly(seedComponent.IsIndoorOnly());
        harvestedSeedComponent.SetQuality(quality);
        harvestedSeedComponent.SetYield(seedComponent.GetYield());
        harvestedSeedComponent.SetGrowSpeedInterval(seedComponent.GetGrowSpeedInterval());
        harvestedSeedComponent.SetColor(seedComponent.GetColor());
        harvestedSeedComponent.ChangeStrainName(strainName);
        //Replication.BumpMe();
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

  override bool CanBeShownScript(IEntity user) {
    return true;
  }

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

  override bool GetActionNameScript(out string outName) {
    int harvestAmount = amount;
    RL_WeedSeedComponent seedComponent = RL_WeedSeedComponent.Cast(GetOwner().FindComponent(RL_WeedSeedComponent));
    if (seedComponent) {
      harvestAmount = seedComponent.GetYield();
    }

    outName = string.Format("Harvest %1x %2", harvestAmount, RL_ResourceUtils.GetDisplayNameFromResource(m_givenItem));
    return true;
  }
}