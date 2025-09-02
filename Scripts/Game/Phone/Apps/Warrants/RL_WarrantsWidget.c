sealed class RL_WarrantsWidget : RL_AppBaseWidget
{

    protected Widget m_warrantsContainer;
    protected SCR_ListBoxComponent m_warrantsListBox;
    protected Widget m_formContainer;
    protected SCR_ComboBoxComponent m_suspectSelector;
    protected SCR_ComboBoxComponent m_crimeSelector;
    protected SCR_InputButtonComponent m_newButton;
    protected SCR_InputButtonComponent m_submitButton;
	protected SCR_InputButtonComponent m_deleteButton;

    protected ref array<ref RL_CrimeData> m_crimeOptions;
    protected string m_crimeConfig = "{D4CF1BBD194B8D4B}Configs/Crime/RL_CrimeConfig.conf";

    protected ref array<ref RL_WarrantData> m_warrantsData;
    protected ref array<int> m_playerList;
    protected ref array<string> m_nameList;


    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

        if(!m_character) return;
		
        m_warrantsContainer = Widget.Cast(m_wRoot.FindAnyWidget("warrantsList"));
        if(!m_warrantsContainer) return;
        m_warrantsListBox = SCR_ListBoxComponent.Cast(m_warrantsContainer.FindHandler(SCR_ListBoxComponent));

        m_formContainer = Widget.Cast(m_wRoot.FindAnyWidget("formContainer"));
        if(!m_formContainer) return;
        m_formContainer.SetVisible(false);

        m_suspectSelector = SCR_ComboBoxComponent.GetComboBoxComponent("suspectSelector", m_wRoot);
        m_crimeSelector = SCR_ComboBoxComponent.GetComboBoxComponent("crimeSelector", m_wRoot);

        m_newButton = SCR_InputButtonComponent.GetInputButtonComponent("NewButton", m_wRoot);
        if (!m_newButton) return;
        m_newButton.m_OnActivated.Insert(OnSwitchToCreate);
        
        m_submitButton = SCR_InputButtonComponent.GetInputButtonComponent("SubmitButton", m_wRoot);
        if (!m_submitButton) return;
        m_submitButton.SetVisible(false);
        m_submitButton.m_OnActivated.Insert(OnSubmit);

        m_deleteButton = SCR_InputButtonComponent.GetInputButtonComponent("DeleteButton", m_wRoot);
        if (!m_deleteButton) return;
        m_deleteButton.m_OnActivated.Insert(OnDelete);

        Resource contHolder = BaseContainerTools.LoadContainer(m_crimeConfig);
        BaseContainer cont = contHolder.GetResource().ToBaseContainer();
        m_crimeOptions = RL_CrimeConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(cont)).GetCrimeList();

        m_character.GetAllWarrants(this);
        FillCrimeOptions();
		
	}
    void FillWarrantsList(bool success, array<ref RL_WarrantData> results)
    {
        if(!success || !results)
            return;
        
        m_warrantsData = results;

        m_warrantsListBox.Clear();
        foreach (RL_WarrantData warrant : m_warrantsData)
		{
            Print("[RL_WarrantsWidget] FillWarrantsList foreach");
            RL_CrimeData crimeData;
            foreach (RL_CrimeData crime: m_crimeOptions)
            {
                Print("[RL_WarrantsWidget] FillWarrantsList foreach 2");
                if(warrant.GetCrimeId() == crime.GetCrimeId())
                {
                    crimeData = crime;
                }
            }
            if(!crimeData) continue;
            //Print(text.GetPartyName());
            //Print(text.GetContent());
            m_warrantsListBox.AddItem(
                string.Format("%1 -\n%2 \nIssued by %3", warrant.GetCharacterName(), crimeData.GetTitle(), warrant.GetIssuerName())
            );
        }
    }
    void FillSuspectList(array<int> playerList, array<string> nameList)
    {
        Print("FillSuspectList");
        Print(playerList);
        m_playerList = playerList;
        m_nameList = nameList;

        foreach(string fullName : nameList) {
            Print("[RL_WarrantsWidget] FillSuspectList foreach");
            m_suspectSelector.AddItem(fullName);
        }
        
    }
    void FillCrimeOptions()
    {
        m_crimeSelector.ClearAll();
        foreach (RL_CrimeData crime: m_crimeOptions)
        {
            Print("[RL_WarrantsWidget] FillCrimeOptions foreach");
            m_crimeSelector.AddItem(crime.GetTitle(), false, crime);
        }
        m_crimeSelector.SetCurrentItem(0);
    }
    void OnSwitchToCreate(SCR_InputButtonComponent btn)
    {
        SetWarrantsVisible(false);

    }
    void OnSubmit(SCR_InputButtonComponent btn)
    {
        if (m_character.IsSpamming())
            return;

        int selectedRow = m_suspectSelector.GetCurrentIndex();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_playerList.Count() ) return;
        int suspectId = m_playerList[selectedRow];
        RL_CrimeData selectedCrime = RL_CrimeData.Cast(m_crimeSelector.GetCurrentItemData());
        if(!selectedCrime) return;

        m_character.IssueWarrant(this, suspectId, selectedCrime.GetCrimeId());
        
        SetWarrantsVisible(true);

    }
    void OnDelete(SCR_InputButtonComponent btn)
    {
		if (m_character.IsSpamming())
			return;

        int selectedRow = m_warrantsListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow >= m_warrantsData.Count()) return;
        
        RL_WarrantData selectedWarrant = m_warrantsData[selectedRow];
        if(!selectedWarrant) return;

        m_character.DeleteWarrant(this, selectedWarrant.GetWarrantId(), selectedWarrant.GetCharacterId());
    }
    void SetWarrantsVisible(bool warrantsVisible)
    {
        m_warrantsContainer.SetVisible(warrantsVisible);
        m_newButton.SetVisible(warrantsVisible);
        m_deleteButton.SetVisible(warrantsVisible);
        m_formContainer.SetVisible(!warrantsVisible);
        m_submitButton.SetVisible(!warrantsVisible);
    }
}