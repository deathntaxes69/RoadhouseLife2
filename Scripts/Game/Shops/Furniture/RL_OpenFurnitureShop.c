class RL_OpenFurnitureShop : RL_OpenShopBaseAction
{
    protected RL_FurnitureShopUI m_shopMenu;

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_FurnitureShopMenu); 
        m_shopMenu = RL_FurnitureShopUI.Cast(menuBase);
		m_shopMenu.SetEntities(pOwnerEntity, pUserEntity);

		m_character.LoadFurnitureShop(m_shopMenu, m_shopId);
	}
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Furniture Shop";

		return true;
	}
}