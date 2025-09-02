class RL_FishingZoneStateComponentOLDClass: ScriptComponentClass {}

class RL_FishingZoneStateComponentOLD: ScriptComponent {
  private bool isInZone = false;
  private bool isInResZone = false;
  private bool isInRareZone = false;

  void SetIsInZone(bool state) {
    isInZone = state;
  }

  bool IsInZone() {
    return isInZone;
  }

  void SetIsInRestZone(bool state) {
    isInResZone = state;
  }

  bool IsInRestZone() {
    return isInResZone;
  }

  //---------------//

  void SetIsInRareZone(bool state) {
    isInRareZone = state;
  }

  bool IsInRareZone() {
    return isInRareZone;
  }

}