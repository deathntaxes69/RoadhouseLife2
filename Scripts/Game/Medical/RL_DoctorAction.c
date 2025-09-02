[BaseContainerProps]
class RL_DoctorAction : ScriptedUserAction
{
    protected int m_iPriceForHeal = 1000;
    
    protected IEntity m_ownerEntity;
    protected ref RL_CharacterDbHelper m_characterHelper;
    
    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
        m_ownerEntity = pOwnerEntity;
    }
    
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
        if (!character)
            return;

        if (!Replication.IsServer())
        {

            return;
        }    
        Tuple2<int, string> context(EPF_NetworkUtils.GetRplId(character), "IDK");
        
        if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
        m_characterHelper.TransactMoney(character.GetCharacterId(), (-m_iPriceForHeal), 0, this, "HealPurchaseCallback", context);
    }
    void HealPurchaseCallback(bool success, Managed context)
    {
        if(!success)
            return;
        // Prefab, quantity, total price, character ID
        Tuple2<int, string> typedContext = Tuple2<int, string>.Cast(context);
        IEntity targetEntity = EPF_NetworkUtils.FindEntityByRplId(typedContext.param1);
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(targetEntity);
        if(!character)
            return;

        SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(character.FindComponent(SCR_CharacterDamageManagerComponent));
        RL_MedicalUtils.Heal(damageMgr, 1, 1, true);
        
        character.Notify(string.Format("Healed for %1", RL_Utils.FormatMoney(m_iPriceForHeal)), "DOCTOR");

    }
    //------------------------------------------------------------------------------------------------
    override bool GetActionNameScript(out string outName)
    {
        outName = string.Format("Heal (%1)", RL_Utils.FormatMoney(m_iPriceForHeal));
        return true;
    }
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
        if (!character)
            return true;
        
        if (character.GetBank() < m_iPriceForHeal)
        {
            string reason = string.Format("Insufficient funds. Need %1", 
                RL_Utils.FormatMoney(m_iPriceForHeal));
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
    override bool CanBroadcastScript() 
	{ 
		return false; 
	}
} 