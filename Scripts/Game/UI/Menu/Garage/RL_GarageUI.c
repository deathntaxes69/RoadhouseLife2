
class RL_GarageUI : ChimeraMenuBase
{

    protected RplId m_entityRplId;
    protected SCR_ChimeraCharacter m_character;
    protected RL_VehicleSystemComponent m_spawnComponent;

	protected Widget m_wRoot;
    protected TextWidget m_garageTitle;
    protected SCR_ListBoxComponent m_garageListBox;
    protected ButtonWidget m_garageRetrieveButton;
    
    [RplProp()]
    ref array<ref RL_GarageItem> m_garageData;
	
	override void OnMenuOpen() 
	{ 
	    m_character = RL_Utils.GetLocalCharacter();
        
        m_wRoot = GetRootWidget();
        m_garageTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("garageTitle"));
        Widget garageListWidget = Widget.Cast(m_wRoot.FindAnyWidget("garageList"));
        if(!garageListWidget) return;
        m_garageListBox = SCR_ListBoxComponent.Cast(garageListWidget.FindHandler(SCR_ListBoxComponent));
        ButtonWidget retrieveWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("RetrieveButton"));
		if (retrieveWidget)
		{
			SCR_InputButtonComponent retrieveButton = SCR_InputButtonComponent.Cast(retrieveWidget.FindHandler(SCR_InputButtonComponent));
			if (retrieveButton)
			{
				retrieveButton.m_OnActivated.Insert(RetrieveVehicle);
			}
		}
        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);

        m_garageData = {};


    }
    void OnGarageLoaded(bool success, string results)
    {
        if(!m_garageListBox)
            return;
        RL_GarageArray castedResults = new RL_GarageArray();
		castedResults.ExpandFromRAW(results);
        m_garageData = castedResults.data;
        // Add vehicles to list box
        foreach (RL_GarageItem garageItem : m_garageData)
		{
            Print("[RL_GarageUI] OnGarageLoaded foreach");
            string displayName = RL_ShopUtils.GetEntityDisplayName(garageItem.GetPrefab());
            if(!displayName)
                displayName = "ERROR";
            
            bool disabled = false;
            if(displayName == "ERROR")
            {
                disabled = true;
            }
            else if(garageItem.GetStatus() != 0)
            {
                if(garageItem.GetStatus() == 2)
                {
                    displayName = displayName + " (Impounded)";
                }
                else if(garageItem.GetStatus() == 3)
                {
                    displayName = displayName + " (Chopped)";
                }
                else
                {
                    disabled = true;
                    displayName = displayName + " (Unavailable)";
                }
            }

            int newItem = m_garageListBox.AddItem(displayName);
            SCR_ListBoxElementComponent newItemComp = m_garageListBox.GetElementComponent(newItem);
            // Disable vehicles marked as active in the world
            if(disabled)
                newItemComp.SetEnabled(false);
        }
    }
    void RetrieveVehicle(SCR_InputButtonComponent btn)
    {
        if(!m_character || m_character.IsSpamming()) return;

        int selectedRow =  m_garageListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_garageData.Count() ) return;
        RL_GarageItem garageItem = m_garageData[selectedRow];

        if(!m_spawnComponent)
        {
            RL_Utils.Notify("Something went wrong, try another garage", "GARAGE");
            Print("The garage needs a RL_VehicleSystemComponent component", LogLevel.ERROR);
            return;
        }
        m_character.RetrieveVehicle(m_entityRplId, garageItem, m_character.GetCharacterId(), m_character.GetCharacterName());
        

        CloseMenu();

    }
    void SetEntities(IEntity owner, IEntity player)
	{
		m_entityRplId = EPF_NetworkUtils.GetRplId(owner);
        m_spawnComponent = RL_VehicleSystemComponent.Cast(owner.FindComponent(RL_VehicleSystemComponent));
	}
    void CloseMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
}