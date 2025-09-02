class RL_FishingZoneRAREClass : BaseGameTriggerEntityClass {}

class RL_FishingZoneRARE : BaseGameTriggerEntity {
  override void OnActivate(IEntity ent) {
    super.OnActivate(ent);

    SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(ent);
    if (player) {
      RL_FishingZoneStateComponent zoneStateComponent = RL_FishingZoneStateComponent.Cast(player.FindComponent(RL_FishingZoneStateComponent));
      if (zoneStateComponent) {
        zoneStateComponent.SetIsInRareZone(true);
      }
    }
  }

  override void OnDeactivate(IEntity ent) {
    super.OnDeactivate(ent);

    SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(ent);
    if (player) {
      RL_FishingZoneStateComponent zoneStateComponent = RL_FishingZoneStateComponent.Cast(player.FindComponent(RL_FishingZoneStateComponent));
      if (zoneStateComponent) {
        zoneStateComponent.SetIsInRareZone(false);
      }
    }
  }
}
