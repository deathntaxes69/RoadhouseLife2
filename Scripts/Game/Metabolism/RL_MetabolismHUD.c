class RL_MetabolismHUD: SCR_InfoDisplay {
  protected RL_MetabolismComponent m_MetabolismComponent;
  protected SCR_CharacterDamageManagerComponent m_DamageManager;

  protected SCR_WLibProgressBarComponent m_barFood;
  protected SCR_WLibProgressBarComponent m_barThirst;
  protected SCR_WLibProgressBarComponent m_barHealth;

  protected TextWidget m_TextFood;
  protected TextWidget m_TextThirst;
  protected TextWidget m_TextHealth;

  override event void OnStartDraw(IEntity owner) {
    super.OnStartDraw(owner);

    m_MetabolismComponent = RL_MetabolismComponent.Cast(owner.FindComponent(RL_MetabolismComponent));
    m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));

    m_barFood = SCR_WLibProgressBarComponent.GetProgressBar("ProgressBarFood", m_wRoot);
    m_barThirst = SCR_WLibProgressBarComponent.GetProgressBar("ProgressBarThirst", m_wRoot);
    m_barHealth = SCR_WLibProgressBarComponent.GetProgressBar("ProgressBarHealth", m_wRoot);

    m_TextFood = TextWidget.Cast(m_wRoot.FindAnyWidget("TextFood"));
    m_TextThirst = TextWidget.Cast(m_wRoot.FindAnyWidget("TextThirst"));
    m_TextHealth = TextWidget.Cast(m_wRoot.FindAnyWidget("TextHealth"));
  }

  override event void UpdateValues(IEntity owner, float timeSlice) {
    if (!m_MetabolismComponent || !m_DamageManager)
      return;

    float foodLevel = m_MetabolismComponent.GetFoodLevel();
    float thirstLevel = m_MetabolismComponent.GetThirstLevel();
    float healthLevel = 1.0;

    if (m_DamageManager.GetMaxHealth() > 0)
      healthLevel = m_DamageManager.GetHealth() / m_DamageManager.GetMaxHealth();

    if (m_barFood) m_barFood.SetValue(foodLevel);
    if (m_barThirst) m_barThirst.SetValue(thirstLevel);
    if (m_barHealth) m_barHealth.SetValue(healthLevel);

    if (m_TextFood) m_TextFood.SetText(string.Format("%1%", Math.Round(foodLevel * 100)));
    if (m_TextThirst) m_TextThirst.SetText(string.Format("%1%", Math.Round(thirstLevel * 100)));
    if (m_TextHealth) m_TextHealth.SetText(string.Format("%1%", Math.Round(healthLevel * 100)));
  }
}