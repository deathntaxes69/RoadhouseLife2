sealed class RL_TextWidget : RL_AppBaseWidget
{

    protected TextWidget m_titleText;
    protected Widget m_conversationsListWidget;
    protected RL_TextListBoxComponent m_conversationsListBox;

    protected Widget m_textListWidget;
    protected RL_TextListBoxComponent m_textListBox;
    protected ScrollLayoutWidget m_textScrollLayout;

    protected SCR_EditBoxComponent m_editBox;
    protected SCR_InputButtonComponent m_sendButton;

    protected string m_selectedPartyId = string.Empty;
    protected ref array<ref RL_TextData> m_conversationData;
    protected ref array<ref RL_TextData> m_lastTextsData;
    

    override protected void HandlerAttached(Widget w)
	{
        super.HandlerAttached(w);

        if(!m_character || !m_character.GetCharacterId())
			return;
		

        m_titleText = TextWidget.Cast(m_wRoot.FindAnyWidget("titleText"));
        if(!m_titleText)
            return;

        m_conversationsListWidget = Widget.Cast(m_wRoot.FindAnyWidget("conversationsList"));
        if(!m_conversationsListWidget)
            return;

        m_conversationsListBox = RL_TextListBoxComponent.Cast(m_conversationsListWidget.FindHandler(RL_TextListBoxComponent));
        if(!m_conversationsListBox) return;
        m_conversationsListBox.m_OnChanged.Insert(OnConversationChange);

        m_textListWidget = Widget.Cast(m_wRoot.FindAnyWidget("textList"));
        if(!m_textListWidget)
            return;

        m_textListBox = RL_TextListBoxComponent.Cast(m_textListWidget.FindHandler(RL_TextListBoxComponent));
        
        m_textScrollLayout = ScrollLayoutWidget.Cast(m_textListWidget.FindAnyWidget("ScrollLayout0"));
        if(!m_textScrollLayout)
            return;
        Print(m_textScrollLayout);

        Widget editBox = Widget.Cast(m_wRoot.FindAnyWidget("EditBox"));
        m_editBox = SCR_EditBoxComponent.Cast(editBox.FindHandler(SCR_EditBoxComponent));
        
        m_sendButton = SCR_InputButtonComponent.GetInputButtonComponent("SendButton", m_wRoot);
        if (m_sendButton)
            m_sendButton.m_OnActivated.Insert(OnSend);
        
		
	}
    void GetLastTexts()
    {
        m_editBox.SetVisible(false);
        m_sendButton.SetVisible(false);
        m_character.GetLastTexts(this);
    }
    void FillLastTexts(bool success, array<ref RL_TextData> results)
    {
        Print("FillLastTexts");
        m_titleText.SetText("TEXTS");
        
        if(!success || !results)
            return;
        
        m_lastTextsData = results;

        m_conversationsListBox.Clear();
        foreach (RL_TextData text : results)
		{
            m_conversationsListBox.AddConversation(text.GetPartyName(), text.GetContent());
        }
    }
    void OnConversationChange(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {
        if(m_lastTextsData && selectedRow >= m_lastTextsData.Count() ) return;
        RL_TextData lastText = m_lastTextsData[selectedRow];

        string textPartyId;
        if(m_character.GetCharacterId() == lastText.GetSender())
            textPartyId = lastText.GetRecipient();
        else
            textPartyId = lastText.GetSender();
        
        if(textPartyId == string.Empty) return;
        
        GetConversation(textPartyId);
        
        //Reset selection so they can click it again later
        //listBox.SetItemSelected(selectedRow, false);
    }
    void GetConversation(string textPartyId)
    {
        m_selectedPartyId = textPartyId;
        m_character.GetConversation(this, textPartyId);
        
        
    }
    void FillConversation(bool success, array<ref RL_TextData> results)
    {
        Print("FillConversation");
        if(!m_textListBox || !m_textScrollLayout) return;
        m_textListBox.Clear();

        m_conversationsListWidget.SetVisible(false);
        m_textListWidget.SetVisible(true);
        m_editBox.SetVisible(true);
        m_sendButton.SetVisible(true);
        //GetPartyName
        
        m_editBox.ActivateWriteMode(true);

        if(!success || !results) return;
        
        m_conversationData = results;

        bool titleSet = false;
        foreach (RL_TextData text : results)
		{
            HorizontalFillOrigin fillOrgin = HorizontalFillOrigin.RIGHT;

            if(m_character.GetCharacterId() == text.GetSender())
            {
                fillOrgin = HorizontalFillOrigin.LEFT;
            }
            string wrappedTextContent = WrapText(text.GetContent());
            m_textListBox.CreateTextEntry(wrappedTextContent, fillOrgin);
            
            if(titleSet) continue;
            titleSet = true;
            m_titleText.SetText(text.GetPartyName());
            
        }
        GetGame().GetCallqueue().CallLater(m_textScrollLayout.SetSliderPos, 500, false, 0 , 1);
        
        //m_textScrollLayout.SetSliderPos(0, 0);
    }
    
    // IDK BRO SEEMS A LITTLE BROKE BUT SORTA WORKS
    string WrapText(string textContent)
    {
        int maxCharactersPerLine = 25;
        array<string> lines = {};

        array<string> splitWords = {};
        textContent.Split(" ", splitWords, true); // Split by space

        string currentLine = "";

        foreach (string word : splitWords)
        {
            string remainingWord = word;
            
            // Handle long words by breaking them
            while (remainingWord.Length() > maxCharactersPerLine)
            {
                // If we have content on current line, finish it first
                if (currentLine != "")
                {
                    lines.Insert(currentLine);
                    currentLine = "";
                }
                
                // Add the chunk that fits on one line
                lines.Insert(remainingWord.Substring(0, maxCharactersPerLine));
                remainingWord = remainingWord.Substring(maxCharactersPerLine, remainingWord.Length() - maxCharactersPerLine);
            }

            // Now handle the remaining word (or original word if it wasn't too long)
            if (remainingWord != "")
            {
                // Check if adding this word would exceed the line limit
                int spaceNeeded;
                if (currentLine == "")
                    spaceNeeded = 0;
                else
                    spaceNeeded = 1;
                    
                if ((currentLine.Length() + spaceNeeded + remainingWord.Length()) > maxCharactersPerLine)
                {
                    // Start a new line
                    if (currentLine != "")
                    {
                        lines.Insert(currentLine);
                    }
                    currentLine = remainingWord;
                }
                else
                {
                    // Add word to current line
                    if (currentLine == "")
                        currentLine = remainingWord;
                    else
                        currentLine += " " + remainingWord;
                }
            }
        }

        // Add the last line if it has content
        if (currentLine != "")
            lines.Insert(currentLine);

        // Join all lines with single newlines
        string result = "";
        for (int i = 0; i < lines.Count(); i++)
        {
            if (i > 0)
                result += "\n";
            result += lines[i];
        }
        return result;

    }
    void BackToConversations()
    {
        m_selectedPartyId = string.Empty;

        m_conversationsListWidget.SetVisible(true);
        m_textListWidget.SetVisible(false);
        m_editBox.SetVisible(false);
        m_sendButton.SetVisible(false);
        m_character.GetLastTexts(this);
    }
    void OnSend(SCR_InputButtonComponent btn)
    {
        if(m_selectedPartyId == string.Empty) return;
        if(m_character.IsSpamming()) return;
        string content = m_editBox.GetValue();
        if(content == "" || content == " ") return;
        m_character.SendText(this, m_selectedPartyId, content);
    }
    void OnSendCallback()
    {
        if(m_selectedPartyId == string.Empty) 
        {
            m_character.GetLastTexts(this);
        } else {
            m_character.GetConversation(this, m_selectedPartyId);
        }

    }
}