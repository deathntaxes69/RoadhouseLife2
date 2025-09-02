class RL_RefillWaterCanAction: SCR_ScriptedUserAction {
  [Attribute("25", desc: "Water to refill")]
  protected int m_iRefillAmount;

  override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) {
    if (!pUserEntity)
      return;

    CharacterControllerComponent controller = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
    if (!controller)
      return;

    IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
    if (!handItem)
      return;

    RL_WateringCanComponent can = RL_WateringCanComponent.Cast(handItem.FindComponent(RL_WateringCanComponent));
    if (!can)
      return;

    int current = can.GetWaterAmount();
    int max = can.GetMaxWaterAmount();

    if (current >= max)
      return;

    int refill = Math.Min(m_iRefillAmount, max - current);
    can.SetWaterAmount(current + refill);
  }

  override bool CanBePerformedScript(IEntity user) {
    if (!user)
      return false;

    CharacterControllerComponent controller = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
    if (!controller)
      return false;

    IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
    if (!handItem)
      return false;

    RL_WateringCanComponent can = RL_WateringCanComponent.Cast(handItem.FindComponent(RL_WateringCanComponent));
    if (!can) {
      SetCannotPerformReason("No watering can in hand");
      return false;
    }

    if (can.GetWaterAmount() >= can.GetMaxWaterAmount()) {
      SetCannotPerformReason("Already full");
      return false;
    }

    return true;
  }

  override bool CanBeShownScript(IEntity user) {
    return true;
  }

  override bool GetActionNameScript(out string outName) {
    outName = "Refill Watering Can";

    IEntity userEntity = GetGame().GetPlayerController().GetControlledEntity();
    if (!userEntity)
      return true;

    CharacterControllerComponent controller = CharacterControllerComponent.Cast(userEntity.FindComponent(CharacterControllerComponent));
    if (!controller)
      return true;

    IEntity handItem = controller.GetAttachedGadgetAtLeftHandSlot();
    if (!handItem)
      return true;

    RL_WateringCanComponent can = RL_WateringCanComponent.Cast(handItem.FindComponent(RL_WateringCanComponent));
    if (!can)
      return true;

    int current = can.GetWaterAmount();
    int max = can.GetMaxWaterAmount();

    outName = "Refill Can (" + current + "L / " + max + "L)";
    return true;
  }
}