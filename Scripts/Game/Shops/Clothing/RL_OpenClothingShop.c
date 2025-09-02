class RL_OpenClothingShop : RL_OpenShopBaseAction
{
    protected RL_ClothingShopUI m_shopMenu;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_ClothingShopMenu); 
        m_shopMenu = RL_ClothingShopUI.Cast(menuBase);
		m_shopMenu.SetEntities(pOwnerEntity, m_character);

		m_character.LoadClothingShop(m_shopMenu, m_shopId);
	}
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Clothing Shop";

		return true;
	}
}