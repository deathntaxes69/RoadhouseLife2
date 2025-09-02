class RL_WellDrinkAction: SCR_ScriptedUserAction {
  [Attribute("0.05", UIWidgets.Slider, params: "0 1 0.01")]
  protected float m_fThirstAmount;

  override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) {
    RL_MetabolismComponent metabolism = RL_MetabolismComponent.Cast(pUserEntity.FindComponent(RL_MetabolismComponent));
    if (!metabolism)
      return;

    float currentThirst = metabolism.GetThirstLevel();
    float allowedDrink = Math.Min(m_fThirstAmount, 1.0 - currentThirst);

    if (allowedDrink > 0) {
      metabolism.IncreaseThirstLevel(allowedDrink);
    }
  }

  override bool CanBePerformedScript(IEntity user) {
    RL_MetabolismComponent metabolism = RL_MetabolismComponent.Cast(user.FindComponent(RL_MetabolismComponent));
    if (!metabolism)
      return false;

    if (metabolism.GetThirstLevel() >= 1.0) {
      SetCannotPerformReason("You are already fully hydrated.");
      return false;
    }

    return true;
  }

  override bool GetActionNameScript(out string outName) {
    outName = "Drink from Well";
    return true;
  }
}