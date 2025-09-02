sealed class RL_ContactsWidget : RL_AppBaseWidget
{
    protected SCR_ListBoxComponent m_contactsListBox;

    protected ref array<int> m_playerList;
    protected ref array<string> m_nameList;

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

        Widget contactsListWidget = Widget.Cast(m_wRoot.FindAnyWidget("contactsList"));
        if(!contactsListWidget)
            return;
        m_contactsListBox = SCR_ListBoxComponent.Cast(contactsListWidget.FindHandler(SCR_ListBoxComponent));

        SCR_InputButtonComponent callButton = SCR_InputButtonComponent.GetInputButtonComponent("CallButton", m_wRoot);
        if (callButton)
            callButton.m_OnActivated.Insert(OnCall);
        SCR_InputButtonComponent textButton = SCR_InputButtonComponent.GetInputButtonComponent("TextButton", m_wRoot);
        if (textButton)
            textButton.m_OnActivated.Insert(OnText);
		
	}
    void FillContactList(array<int> playerList, array<string> nameList)
    {
        m_playerList = playerList;
        m_nameList = nameList;
        
        if(playerList.Count() < 1)
            return;
            
        foreach(string fullName : nameList) {
            Print("[RL_ContactsWidget] FillContactList foreach loop");
            m_contactsListBox.AddItem(fullName);
        }
        m_contactsListBox.SetItemSelected(0, true);
        m_contactsListBox.SetFocusOnFirstItem();
    }
    void OnCall(SCR_InputButtonComponent btn)
    {
        Print("OnCall");
        Print(m_playerList);
        if (m_character.IsSpamming())
            return;

        int selectedRow = m_contactsListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_playerList.Count() ) return;
        int recipient = m_playerList[selectedRow];
        string recipientName = m_nameList[selectedRow];

        BaseRadioComponent radioComp = RL_RadioUtils.GetRadioComponent(m_character);
        if(!radioComp)
        {
            RL_Utils.Notify("You need a radio to call", "PHONE");
            return;
        }
        m_character.PlaceCall(recipient, recipientName);
        m_phoneMenu.CloseMenu();
    }
    void OnText(SCR_InputButtonComponent btn)
    {
        Print("OnText");
        Print(m_playerList);
        if (m_character.IsSpamming())
            return;

        int selectedRow = m_contactsListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_playerList.Count() ) return;
        int recipient = m_playerList[selectedRow];
        string recipientName = m_nameList[selectedRow];

        BaseRadioComponent radioComp = RL_RadioUtils.GetRadioComponent(m_character);
        if(!radioComp)
        {
            RL_Utils.Notify("You need a radio to text", "PHONE");
            return;
        }
        Widget appWidget = m_phoneMenu.SwitchApp("Messages", true);
        if(!appWidget) return;
        RL_TextWidget textWidget = RL_TextWidget.Cast(appWidget.FindHandler(RL_TextWidget));
        string recipientId = RL_Utils.GetCharacterIdByPlayerId(recipient);
        if(!recipientId || recipientId.IsEmpty()) return;
        textWidget.GetConversation(recipientId);
    }
}