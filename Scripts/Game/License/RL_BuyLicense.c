[BaseContainerProps]
class RL_BuyLicense : ScriptedUserAction
{
    [Attribute(defvalue:"1", desc: "License type to purchase", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ELicenseType))]
    protected ELicenseType m_eLicenseType;
    
    [Attribute(defvalue:"1000", desc: "Price for this license")]
    protected int m_iPrice;
    
    protected IEntity m_ownerEntity;
    
    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
        m_ownerEntity = pOwnerEntity;
    }
    
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        if (!Replication.IsServer())
            return;
            
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
        if (!character)
            return;
        
        character.BuyLicense(m_eLicenseType, m_iPrice);
    }
    
    //------------------------------------------------------------------------------------------------
    override bool GetActionNameScript(out string outName)
    {
        string licenseName = GetLicenseName(m_eLicenseType);
        outName = string.Format("Buy %1 - %2", licenseName, RL_Utils.FormatMoney(m_iPrice));
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
        if (!character)
            return false;
            
        string licenseString = m_eLicenseType.ToString();
        return !character.HasLicense(licenseString);
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
        if (!character)
        {
            SetCannotPerformReason("Invalid character");
            return false;
        }
        
        string licenseString = m_eLicenseType.ToString();
        if (character.HasLicense(licenseString))
        {
            SetCannotPerformReason("You already have this license");
            return false;
        }
        
        int playerBank = character.GetBank();
        if (playerBank < m_iPrice)
        {
            string reason = string.Format("Insufficient bank funds. Need %1, have %2", 
                RL_Utils.FormatMoney(m_iPrice), RL_Utils.FormatMoney(playerBank));
            SetCannotPerformReason(reason);
            return false;
        }
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool HasLocalEffectOnlyScript()
    {
        return false;
    }
} 