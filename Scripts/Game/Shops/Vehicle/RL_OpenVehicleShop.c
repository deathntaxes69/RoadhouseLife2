class RL_OpenVehicleShop : RL_OpenShopBaseAction
{
	protected RL_VehicleSystemComponent m_spawnComponent;
    protected RL_VehicleShopUI m_shopMenu;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_VehicleShopMenu); 
        m_shopMenu = RL_VehicleShopUI.Cast(menuBase);
		m_shopMenu.SetEntities(pOwnerEntity, pUserEntity);

		m_character.LoadVehicleShop(m_shopMenu, m_shopId);
	}
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Vehicle Shop";

		return true;
	}
}