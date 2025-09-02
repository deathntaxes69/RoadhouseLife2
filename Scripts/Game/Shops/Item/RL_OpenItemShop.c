class RL_OpenItemShop : RL_OpenShopBaseAction
{
    protected RL_ItemShopUI m_shopMenu;

	[Attribute(defvalue: "0", desc: "Enforce license requirement")]
	protected bool m_bEnforceLicenseRequirement;

	[Attribute(defvalue: "1", desc: "Required license type", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ELicenseType))]
	protected ELicenseType m_eRequiredLicense;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (m_bEnforceLicenseRequirement)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (!character)
				return;
			
			string licenseString = m_eRequiredLicense.ToString();
			if (!character.HasLicense(licenseString))
			{
				string licenseName = GetLicenseName(m_eRequiredLicense);
				RL_Utils.Notify(string.Format("Required license: %1", licenseName), "ACTION");
				return;
			}
		}

		super.PerformAction(pOwnerEntity, pUserEntity);
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_ItemShopMenu); 
        m_shopMenu = RL_ItemShopUI.Cast(menuBase);
		m_shopMenu.SetEntities(pOwnerEntity, m_character);

		m_character.LoadItemShop(m_shopMenu, m_shopId);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (m_bEnforceLicenseRequirement)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
			if (!character)
			{
				SetCannotPerformReason("Invalid character");
				return false;
			}
			
			string licenseString = m_eRequiredLicense.ToString();
			if (!character.HasLicense(licenseString))
			{
				string licenseName = GetLicenseName(m_eRequiredLicense);
				SetCannotPerformReason(string.Format("Required license: %1", licenseName));
				return false;
			}
		}
		
		return super.CanBePerformedScript(user);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
        outName = "Open Item Shop";

		return true;
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