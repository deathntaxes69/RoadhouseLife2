sealed enum RL_SHOP_TYPE {
    Generic,
    Police = 1,
    Medic = 2,
	Admin = 3
}
class RL_OpenShopBaseAction : ScriptedUserAction
{
	protected IEntity m_ownerEntity;
	protected SCR_ChimeraCharacter m_character;

	[Attribute(defvalue:"1", desc: "Shop ID", category: "General")]
	protected int m_shopId;

	[Attribute("0", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(RL_SHOP_TYPE))]
	private RL_SHOP_TYPE m_shopType;

	protected bool m_bAccessChecked = false;
	protected bool m_bAccessAllowed = false;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_ownerEntity = pOwnerEntity;
		m_character = SCR_ChimeraCharacter.Cast(pUserEntity);
	}
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Shop";
		return true;
	}
	override bool CanBeShownScript(IEntity user)
 	{
		return true;
	}
	override bool CanBePerformedScript(IEntity user)
	{
        if(m_shopType && m_shopType > 0) {
			
			if(AllowedShopAccess(user))
				return true;

            SetCannotPerformReason("Restricted"); 
            return false; 
        }
		
		return true;
	}
	bool AllowedShopAccess(IEntity user)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if(!character) return false;

		// MEDIC /COP
		if((m_shopType == RL_SHOP_TYPE.Police && character.IsPolice()) || (m_shopType == RL_SHOP_TYPE.Medic && character.IsMedic()))
		{
			return true;
		}
		// Admin
		if(m_shopType == RL_SHOP_TYPE.Admin && SCR_Global.IsAdmin(SCR_PlayerController.GetLocalPlayerId()))
		{
			return true;
		}

		return false;
	}
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
	
}