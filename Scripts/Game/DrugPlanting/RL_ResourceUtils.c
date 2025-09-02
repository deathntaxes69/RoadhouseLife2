class RL_ResourceUtils {
  static string GetDisplayNameFromResource(ResourceName res) {
    string path = res;
    int lastSlash = path.LastIndexOf("/");
    int dot = path.LastIndexOf(".");

    if (lastSlash == -1 || dot == -1 || dot <= lastSlash)
      return path;

    string rawName = path.Substring(lastSlash + 1, dot - lastSlash - 1);
    rawName.Replace("_", " ");

    string cap = rawName.Substring(0, 1);
    cap.ToUpper();

    string displayName = cap + rawName.Substring(1, rawName.Length() - 1);
    return displayName;
  }

  static bool HasCorrectGadgetInHand(IEntity user, ResourceName expectedPrefab) {
    if (!user || expectedPrefab.IsEmpty())
      return false;

    CharacterControllerComponent controller = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
    if (!controller || !controller.IsGadgetInHands())
      return false;

    IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
    if (!handItem)
      return false;

    return handItem.GetPrefabData().GetPrefabName() == expectedPrefab;
  }
}