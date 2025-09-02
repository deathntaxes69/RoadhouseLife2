
class RL_PoliceChargeUI : ChimeraMenuBase
{
	protected Widget m_wRoot;
    protected SCR_EditBoxComponent m_ticketInput;
    protected SCR_EditBoxComponent m_jailInput;
    protected ItemPreviewManagerEntity m_characterPreviewManager;
	protected ItemPreviewWidget m_characterPreview;
    protected SCR_ComboBoxComponent m_crimeSelector;

    protected SCR_ChimeraCharacter m_suspectCharacter;
    protected SCR_ChimeraCharacter m_policeCharacter;

    protected ref array<ref RL_CrimeData> m_crimeOptions;
    protected string m_crimeConfig = "{D4CF1BBD194B8D4B}Configs/Crime/RL_CrimeConfig.conf";



	override void OnMenuOpen() 
	{
        m_wRoot = GetRootWidget();
        m_ticketInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("TicketEditBox").FindHandler(SCR_EditBoxComponent));
        m_jailInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("JailEditBox").FindHandler(SCR_EditBoxComponent));
        m_crimeSelector = SCR_ComboBoxComponent.GetComboBoxComponent("crimeSelector", m_wRoot);

        SCR_InputButtonComponent submitButton = SCR_InputButtonComponent.GetInputButtonComponent("SubmitButton", m_wRoot);
        if (submitButton)
            submitButton.m_OnActivated.Insert(OnSubmit);
        
        
        FillCrimeSelector();

        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);

    }
    void FillCrimeSelector()
    {
        Resource contHolder = BaseContainerTools.LoadContainer(m_crimeConfig);
        BaseContainer cont = contHolder.GetResource().ToBaseContainer();
        m_crimeOptions = RL_CrimeConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(cont)).GetCrimeList();

        m_crimeSelector.ClearAll();
        foreach (RL_CrimeData crime: m_crimeOptions)
        {
            Print("[RL_PoliceChargeUI] FillCrimeSelector foreach");
            m_crimeSelector.AddItem(crime.GetTitle(), false);
        }
        m_crimeSelector.SetCurrentItem(0);
    }
    void SetSuspectPreview()
    {
        if(!m_suspectCharacter)
            return;

        TextWidget suspectName = TextWidget.Cast(m_wRoot.FindAnyWidget("suspectName"));
        suspectName.SetText(m_suspectCharacter.GetCharacterName());
        		ResourceName prefabToPreview = RL_ShopUtils.ExtractEntityPrefabName(m_suspectCharacter);

        m_characterPreview = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("characterPreview"));
        if(!m_characterPreview) return;

        ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world) return;

        m_characterPreviewManager = world.GetItemPreviewManager();
        if(!m_characterPreviewManager) return;

        m_characterPreviewManager.SetPreviewItemFromPrefab(m_characterPreview, prefabToPreview, null, true);

    }
    void OnSubmit(SCR_InputButtonComponent w)
	{
        int ticketInputAmount = m_ticketInput.GetValue().ToInt();
        int jailInputAmount = m_jailInput.GetValue().ToInt();
        if((!ticketInputAmount || ticketInputAmount < 1) && (!jailInputAmount || jailInputAmount < 1))
        {
            RL_Utils.Notify("Ticket or jail must be greater than 0.", "CHARGE");
            return;
        }

        if(jailInputAmount > 30)
        {
            RL_Utils.Notify("Jail time cannot be more than 30.", "CHARGE");
            return;
        }
        RplId suspectRplId = EPF_NetworkUtils.GetRplId(m_suspectCharacter);       
        m_policeCharacter.ChargePlayer(this, suspectRplId, m_crimeSelector.GetCurrentItem(), ticketInputAmount, jailInputAmount);
        
        //CloseMenu();
        return;
    }
    void SetEntities(IEntity owner, IEntity player)
	{
        m_suspectCharacter = SCR_ChimeraCharacter.Cast(owner);
        m_policeCharacter = SCR_ChimeraCharacter.Cast(player);
        SetSuspectPreview();
	}
    void CloseMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
}