class RL_RespawnUI: ChimeraMenuBase
{
    protected int m_iMaxCharacters = 2;
    protected string m_sPlayerUid;
    protected RL_RespawnComponent m_respawnComponent;
    protected ref array<ref RL_CharacterAccount> m_aCharacters = {};
	
    protected Widget m_wRoot;
    protected Widget m_background;

    // Player Banned Frame
    protected Widget m_playerBannedFrame;
    protected TextWidget m_playerBannedTimeText;
    protected TextWidget m_playerBannedNoteText;

    // Player Not Whitelisted Frame
    protected Widget m_playerNotWhitelistedFrame;
	protected TextWidget m_playerNotWhitelistedAccountWidget;

    // Player Not Linked Frame
    protected Widget m_playerNotLinkedFrame;
    protected TextWidget m_playerNotLinkedUIDWidget;

	// MOTD Frame
    protected Widget m_playerMOTDFrame;
	protected Widget m_closeMOTDButtonWidget;

    // Select Character Frame
    protected Widget m_selectCharacterFrame;
    protected SCR_ListBoxComponent m_characterListBox;
    protected Widget m_createButtonWidget;

    // Create Character Frame
    protected Widget m_createCharacterFrame;
    protected SCR_EditBoxComponent m_firstNameInput;
    protected SCR_EditBoxComponent m_lastNameInput;
    protected SCR_EditBoxComponent m_ageInput;
    protected TextWidget m_warningText;
    protected ItemPreviewManagerEntity m_characterPreviewManager;
	protected ItemPreviewWidget m_characterPreview;
    protected SCR_ComboBoxComponent m_characterModelSelector;
    protected ref array<ref Tuple2<string, string>> m_characterOptions;

    // Select Spawn Frame
    protected Widget m_selectSpawnFrame;
    protected SCR_ListBoxComponent m_spawnListBox;
    protected ref array<string> m_aSpawnTitles = {};
    protected ref array<vector> m_aSpawnPositions = {};
	protected ref array<vector> m_aSpawnRotations = {};
    protected vector m_vSelectedSpawnVector;
	protected vector m_vSelectedSpawnVectorRot;
 
    // Camera shizz
    protected CameraManager m_cameraManager;
    protected CameraBase m_spawnCamera;
    protected CameraBase m_orginalCamera;
    protected bool m_bEnableCameraMoves;

	override void OnMenuOpen() 
	{ 
        Print("RL_RespawnUI OnMenuOpen Start");
        m_wRoot = GetRootWidget();

        // Background
        m_background = Widget.Cast(m_wRoot.FindAnyWidget("MenuBackground"));

        m_playerBannedFrame = Widget.Cast(m_wRoot.FindAnyWidget("playerBannedFrame"));
        m_playerNotWhitelistedFrame = Widget.Cast(m_wRoot.FindAnyWidget("playerNotWhitelistedFrame"));
		m_playerNotWhitelistedAccountWidget = TextWidget.Cast(m_wRoot.FindAnyWidget("accountInfoNoWhitelistTitle"));
        m_playerNotLinkedFrame = Widget.Cast(m_wRoot.FindAnyWidget("playerNotLinkedFrame"));
        m_playerNotLinkedUIDWidget = TextWidget.Cast(m_wRoot.FindAnyWidget("notLinkedUIDTitle"));
        m_playerMOTDFrame = Widget.Cast(m_wRoot.FindAnyWidget("playerMOTDFrame"));
        m_closeMOTDButtonWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("CloseMOTD"));
		if (m_closeMOTDButtonWidget)
		{
			SCR_InputButtonComponent closeMOTDButton = SCR_InputButtonComponent.Cast(m_closeMOTDButtonWidget.FindHandler(SCR_InputButtonComponent));
			if (closeMOTDButton)
			{
				closeMOTDButton.m_OnActivated.Insert(OnCloseMOTD);
			}
		}

        // Select Character Frame
        m_selectCharacterFrame = Widget.Cast(m_wRoot.FindAnyWidget("selectCharacterFrame"));
        Widget characterListWidget = Widget.Cast(m_wRoot.FindAnyWidget("characterList"));
        if(!characterListWidget) return;
        m_characterListBox = SCR_ListBoxComponent.Cast(characterListWidget.FindHandler(SCR_ListBoxComponent));
        m_createButtonWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("CreateNewCharacter"));
		if (m_createButtonWidget)
		{
			SCR_InputButtonComponent createNewButton = SCR_InputButtonComponent.Cast(m_createButtonWidget.FindHandler(SCR_InputButtonComponent));
			if (createNewButton)
			{
				createNewButton.m_OnActivated.Insert(OpenCreateCharaterFrame);
			}
		}
        Widget selectCharacterWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("SelectCharacter"));
		if (selectCharacterWidget)
		{
			SCR_InputButtonComponent selectCharacterButton = SCR_InputButtonComponent.Cast(selectCharacterWidget.FindHandler(SCR_InputButtonComponent));
			if (selectCharacterButton)
			{
				selectCharacterButton.m_OnActivated.Insert(OnSelectCharacter);
			}
		}

        // Create Character Frame
        m_createCharacterFrame = Widget.Cast(m_wRoot.FindAnyWidget("createCharacterFrame"));
        m_firstNameInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("FirstNameEditBox").FindHandler(SCR_EditBoxComponent));
        m_lastNameInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("LastNameEditBox").FindHandler(SCR_EditBoxComponent));
        m_ageInput = SCR_EditBoxComponent.Cast(m_wRoot.FindAnyWidget("AgeEditBox").FindHandler(SCR_EditBoxComponent));
        m_warningText = TextWidget.Cast(m_wRoot.FindAnyWidget("WarningText"));
        m_characterModelSelector = SCR_ComboBoxComponent.GetComboBoxComponent("modelSelector", m_wRoot);
        m_characterModelSelector.m_OnChanged.Insert(OnChangeCharacterModel);
        Widget cancelNewWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("CancelCharacter"));
        if (cancelNewWidget)
		{
			SCR_InputButtonComponent cancelNewButton = SCR_InputButtonComponent.Cast(cancelNewWidget.FindHandler(SCR_InputButtonComponent));
			if (cancelNewButton)
			{
				cancelNewButton.m_OnActivated.Insert(OnCancelNewCharacter);
			}
		}
        Widget saveCharacterWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("SaveCharacter"));
		if (saveCharacterWidget)
		{
			SCR_InputButtonComponent saveCharacterButton = SCR_InputButtonComponent.Cast(saveCharacterWidget.FindHandler(SCR_InputButtonComponent));
			if (saveCharacterButton)
			{
				saveCharacterButton.m_OnActivated.Insert(OnSaveNewCharacter);
			}
		}

        // Select Spawn Frame
        m_selectSpawnFrame = Widget.Cast(m_wRoot.FindAnyWidget("selectSpawnFrame"));
        Widget spawnListWidget = Widget.Cast(m_wRoot.FindAnyWidget("spawnList"));
        if(!spawnListWidget) return;
        m_spawnListBox = SCR_ListBoxComponent.Cast(spawnListWidget.FindHandler(SCR_ListBoxComponent));
        if(!m_spawnListBox) return;
        m_spawnListBox.m_OnChanged.Insert(OnSelectedSpawnChanged);

        Widget spawnWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Spawn"));
		if (spawnWidget)
		{
			SCR_InputButtonComponent spawnButton = SCR_InputButtonComponent.Cast(spawnWidget.FindHandler(SCR_InputButtonComponent));
			if (spawnButton)
			{
				spawnButton.m_OnActivated.Insert(OnSelectSpawn);
			}
		}

        // Hide it all
        ToggleWidget(m_selectCharacterFrame, false);
        ToggleWidget(m_createCharacterFrame, false);
        ToggleWidget(m_selectSpawnFrame, false);
        ToggleWidget(m_playerBannedFrame, false);
        ToggleWidget(m_playerNotWhitelistedFrame, false);
        ToggleWidget(m_playerNotLinkedFrame, false);
        ToggleWidget(m_playerMOTDFrame, false);

        // Get spawn points and call it again later after whitelists loaded (hopefully)
        GetSpawnPoints();
        //GetGame().GetCallqueue().CallLaterByName(this, "GetSpawnPoints", 2000);
        
        Print("RL_RespawnUI OnMenuOpen Done");
        
    }
    
    void GetSpawnPoints()
    {
        if (!m_aSpawnTitles)
            m_aSpawnTitles = {};
        FillSpawnListBox();
    }
    
    void SetSpawnPoints(array<string> titles, array<vector> positions, array<vector> rotations)
    {
        m_aSpawnTitles = titles;
        m_aSpawnPositions = positions;
		m_aSpawnRotations = rotations;
        FillSpawnListBox();
    }
    void OnChangeCharacterModel()
    {
        Tuple2<string, string> selectedModel = Tuple2<string, string>.Cast(m_characterModelSelector.GetCurrentItemData());
        if (!selectedModel) return;
        SetCharacterModelPreview(selectedModel.param2)
    }
    void SetCharacterModelPreview(string prefabToPreview)
    {
        m_characterPreview = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("characterPreview"));
        if(!m_characterPreview) return;

        ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world) return;

        m_characterPreviewManager = world.GetItemPreviewManager();
        if(!m_characterPreviewManager) return;

        m_characterPreviewManager.SetPreviewItemFromPrefab(m_characterPreview, prefabToPreview, null, true);

    }
    void OnSaveNewCharacter(SCR_InputButtonComponent btn)
    {
		if (m_respawnComponent.IsSpamming())
			return;

        ClearWarningText();
        string firstName = m_firstNameInput.GetValue();
        string lastName = m_lastNameInput.GetValue();
        
        if (firstName.Length() < 2) 
        {
            ShowWarningText("First name must be at least 2 characters long");
            return;
        }
        
        if (lastName.Length() < 2) 
        {
            ShowWarningText("Last name must be at least 2 characters long");
            return;
        }

        firstName = CapitalizeFirstLetter(firstName);
        lastName = CapitalizeFirstLetter(lastName);
        
        string fullName = string.Format("%1 %2", firstName, lastName);

        if (m_ageInput.GetValue().Length() == 0) 
        {
            ShowWarningText("Age is required");
            return;
        }
        
        int age = m_ageInput.GetValue().ToInt();
        if(!age || age == 0) 
        {
            ShowWarningText("Please enter a valid age");
            return;
        }
        
        if (age < 18 || age > 80) 
        {
            ShowWarningText("Age must be between 18 and 80");
            return;
        }

        Tuple2<string, string> selectedModel = Tuple2<string, string>.Cast(m_characterModelSelector.GetCurrentItemData());
        if (!selectedModel) 
        {
            ShowWarningText("Please select a character model");
            return;
        }

        foreach (RL_CharacterAccount character : m_aCharacters)
        {
            Print("[RL_RespawnUI] OnSaveNewCharacter foreach");
            if (character.GetName() == fullName)
            {
                ShowWarningText("A character with this name already exists");
                return;
            }
        }

        m_respawnComponent.AddCharacter(m_sPlayerUid, fullName, age, selectedModel.param2);
        // Switch back to select character
        OnCancelNewCharacter(null);
    }
    void OnAccountAdded(RL_CharacterAccount characterAccount)
    {
        Print("OnAccountAdded 1");
        if(characterAccount)
        {
            Print("OnAccountAdded 2");
            m_aCharacters.Insert(characterAccount);
            FillCharactersListBox();
        }
    }
    void OnCancelNewCharacter(SCR_InputButtonComponent btn)
    {
        ClearWarningText();
        // Switch back to select character
        ToggleWidget(m_selectCharacterFrame, true);
        ToggleWidget(m_createCharacterFrame, false);
    }
    void OpenSelectCharacterFrame(RL_RespawnComponent respawnComponent, string uid)
    {
        Print("OpenSelectCharacterFrame");
        m_respawnComponent = respawnComponent;
        m_sPlayerUid = uid;

        //StartCameraMovement("ZoomCamera");
        ToggleWidget(m_selectCharacterFrame, true);
    }

    void ShowMOTD()
    {
        ToggleWidget(m_selectCharacterFrame, false);
        ToggleWidget(m_playerMOTDFrame, true);

		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, OnCloseMOTD);
    }

    void OnCloseMOTD()
    {
		ToggleWidget(m_playerMOTDFrame, false);

		// Switch to create character if none exist
        if(m_aCharacters.Count() == 0)
        {
            OpenCreateCharaterFrame();
        } else {
        	ToggleWidget(m_selectCharacterFrame, true);
		}
        GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnCloseMOTD);
    }

    void ShowNotLinked(int accountid)
    {
        ToggleWidget(m_selectCharacterFrame, false);
        ToggleWidget(m_playerNotLinkedFrame, true);

		//PlayerController m_playerController = GetGame().GetPlayerController();
		//if (m_playerController)
			//m_playerNotLinkedUIDWidget.SetText(string.Format("Account ID: %1\nUID: %2", accountid, EPF_Utils.GetPlayerUID(m_playerController.GetPlayerId())));
		m_playerNotLinkedUIDWidget.SetText(string.Format("Account ID: %1", accountid));
		
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseGameButton);
        GetGame().GetCallqueue().CallLater(Kick, 20000, false, 30);
    }

    void ShowNotWhitelisted(int accountid)
    {
        ToggleWidget(m_selectCharacterFrame, false);

		//PlayerController m_playerController = GetGame().GetPlayerController();
		//if (m_playerController)
			//m_playerNotWhitelistedAccountWidget.SetText(string.Format("Account ID: %1\nUID: %2", accountid, EPF_Utils.GetPlayerUID(m_playerController.GetPlayerId())));
		m_playerNotWhitelistedAccountWidget.SetText(string.Format("Account ID: %1", accountid));

        ToggleWidget(m_playerNotWhitelistedFrame, true);

		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseGameButton);
        GetGame().GetCallqueue().CallLater(Kick, 20000, false, 30);
    }

    void ShowNameInUse()
    {
		ToggleWidget(m_selectCharacterFrame, false);
		ToggleWidget(m_createCharacterFrame, true);
		ShowWarningText("A character with this name already exists");
    }

    void ShowBanned(int ban_expire_seconds, string ban_note, int accountid)
    {
        int ban_seconds = 0;
        int ban_minutes = 0;
        int ban_hours = 0;
        int ban_days = 0;
        SCR_DateTimeHelper.GetDayHourMinuteSecondFromSeconds(ban_expire_seconds, ban_days, ban_hours, ban_minutes, ban_seconds);
		
        ToggleWidget(m_selectCharacterFrame, false);
        ToggleWidget(m_playerBannedFrame, true);
		
        m_playerBannedTimeText = TextWidget.Cast(m_wRoot.FindAnyWidget("banTimeTitle"));
        m_playerBannedNoteText = TextWidget.Cast(m_wRoot.FindAnyWidget("banNoteTitle"));

        m_playerBannedTimeText.SetText(string.Format("Ban Time Remaining: %1 Days, %2 Hours %3 Minutes %4 Seconds", ban_days, ban_hours, ban_minutes, ban_seconds));
        m_playerBannedNoteText.SetText(string.Format("Account ID: %1\nBan Note: %2", accountid, ban_note));

		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseGameButton);
        GetGame().GetCallqueue().CallLater(Kick, 20000, false, ban_expire_seconds);
    }
	void CloseGameButton()
	{
		GameStateTransitions.RequestGameplayEndTransition();
        GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, CloseGameButton);
	}
    void Kick(int kickTime)
    {
        int playerId = GetGame().GetPlayerController().GetPlayerId();
        // this doesnt seem to work?
        GetGame().GetPlayerManager().KickPlayer(playerId, PlayerManagerKickReason.TEMP_BAN, kickTime);
		CloseGameButton();
    }
    void OnAccountsLoaded(bool success, array<ref RL_CharacterAccount> results)
    {
        Print("OnAccountsLoaded");
        
        if(success && results)
            m_aCharacters = results;

        FillCharactersListBox();

    }
    void FillCharactersListBox()
    {
        m_characterListBox.Clear();

        if (m_aCharacters.Count() == 0)
			return;

        // Disable create character button if greater or equal to max
        if(m_aCharacters.Count() >= m_iMaxCharacters)
            ToggleWidget(m_createButtonWidget, false);

        foreach (RL_CharacterAccount character : m_aCharacters)
		{
            Print(character);
            Print(character.GetName());
            string characterName = character.GetName();
            m_characterListBox.AddItem(characterName);
        }
        // Set first item selected
        m_characterListBox.SetFocusOnFirstItem();
        m_characterListBox.SetItemSelected(0, true);

    }
    void OnSelectCharacter(SCR_InputButtonComponent btn)
    {
        Print("OnSelectCharacter");
        int selectedRow =  m_characterListBox.GetSelectedItem();
        
        if (selectedRow == -1) return;
        if(selectedRow+1 > m_aCharacters.Count() ) return;
        RL_CharacterAccount selectedAccount = m_aCharacters[selectedRow];
        // Pass selected character back to respawn comp.
        m_respawnComponent.OnAccountSelected(selectedAccount);
        CloseMenu();
    }
    void OpenCreateCharaterFrame()
    {
        ClearWarningText();
        
        ToggleWidget(m_selectCharacterFrame, false);
        ToggleWidget(m_createCharacterFrame, true);

        m_characterOptions = {
            new Tuple2<string, string>("Asian Male", "{C465F15DDF56A6D8}Prefabs/Characters/Presets/Asian_Male_02.et"),
            new Tuple2<string, string>("Black Male", "{BFD6AFB8A1D8E2F9}Prefabs/Characters/Presets/Black_Male_02.et"),
            new Tuple2<string, string>("White Male 1", "{CE23D4366B47E9B9}Prefabs/Characters/Presets/White_Male_01.et")
        };
        m_characterModelSelector.ClearAll();
        foreach (Tuple2<string, string> model: m_characterOptions)
        {
            Print("[RL_RespawnUI] OpenCreateCharaterFrame foreach");
            m_characterModelSelector.AddItem(model.param1, false, model);
        }
        m_characterModelSelector.SetCurrentItem(0);
        OnChangeCharacterModel();
    }
    void OpenSelectSpawnFrame(RL_RespawnComponent respawnComponent)
    {
        m_respawnComponent = respawnComponent;
        StartCameraMovement("RotateCamera");
        ToggleWidget(m_selectSpawnFrame, true);

    }
    void StartCameraMovement(string moveFunction) 
    {   
        Print("StartCameraMovement");
        ScriptModule scriptModule = GetGame().GetScriptModule();
        if(!scriptModule) return;
        m_respawnComponent.SetRespawnCamera();
        m_bEnableCameraMoves = true;
		scriptModule.Call(this, moveFunction, true, null);
    }
    void FillSpawnListBox()
    {
        if (!m_spawnListBox) return;
        
        m_spawnListBox.Clear();
        
        if (!m_aSpawnTitles)
            m_aSpawnTitles = {};
        
        foreach (string spawnTitle : m_aSpawnTitles)
        {
            Print("[RL_RespawnUI] FillSpawnListBox foreach");
            m_spawnListBox.AddItem(spawnTitle);
        }
        
        if (m_spawnListBox.GetItemCount() > 0)
        {
            m_spawnListBox.SetItemSelected(0, true);
            m_spawnListBox.SetFocusOnFirstItem();
        }
    }
    void SetSelectedSpawnVector(int index)
    {
        if (index >= 0 && index < m_aSpawnPositions.Count())
            m_vSelectedSpawnVector = m_aSpawnPositions[index];
			m_vSelectedSpawnVectorRot = m_aSpawnRotations[index];
    }
    void OnSelectedSpawnChanged(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
    {
        if (selectedRow >= 0 && selectedRow < m_aSpawnTitles.Count())
            SetSelectedSpawnVector(selectedRow);
    }
    void OnSelectSpawn(SCR_InputButtonComponent btn)
    {
        int selectedRow = m_spawnListBox.GetSelectedItem();
        if (selectedRow == -1 || selectedRow >= m_aSpawnTitles.Count()) return;
        
        SetSelectedSpawnVector(selectedRow);

        SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!playerController) return;
        
        IEntity player = playerController.GetControlledEntity();
        if (!player) return;

        m_respawnComponent.OnSpawnSelected(player, m_vSelectedSpawnVector, m_vSelectedSpawnVectorRot);
        CloseMenu();
    }
    void ToggleWidget(Widget w, bool enable)
    {
        if(!w)
        {
            Print("Why is ToggleWidget being called there is no widget");
            return;
        }
        w.SetVisible(enable);
        w.SetEnabled(enable);
    }
    
    string CapitalizeFirstLetter(string input)
    {
        if (input.Length() == 0) return input;
        
        string firstChar = input.Substring(0, 1);
        firstChar.ToUpper();
        
        if (input.Length() == 1) return firstChar;
        
        string restOfString = input.Substring(1, input.Length() - 1);
        restOfString.ToLower();
        
        return string.Format("%1%2", firstChar, restOfString);
    }
    void ZoomCamera() {

        float distance = 600.0;
        float speed = 0.008;
        float height = 100.0;
        int loopCount = 0;
        while (m_respawnComponent && m_bEnableCameraMoves && distance > 150) {
            Print("[RL_RespawnUI] ZoomCamera while loop");

            CameraBase spawnCamera = m_respawnComponent.GetSpawnCamera();
            if(!spawnCamera || !m_vSelectedSpawnVector)
                continue;
            
            if(loopCount > 5)
                AnimateWidget.Opacity(m_background, 0, UIConstants.FADE_RATE_DEFAULT, true);

            distance -= 5;

            float x = m_vSelectedSpawnVector[0] + distance;
            float z = m_vSelectedSpawnVector[2] + distance;

            SetCameraPos(spawnCamera, x, z, height);

            Sleep(20);

            loopCount++;
        }
    }
    void RotateCamera() {

        float radius = 150.0;
        float speed = 0.008;
        float angle = 0.0;
        float height = 100.0;
        int loopCount = 0;
        
        while (m_respawnComponent && m_bEnableCameraMoves && loopCount < 5000) {
            //Print("[RL_RespawnUI] RotateCamera while loop");

            CameraBase spawnCamera = m_respawnComponent.GetSpawnCamera();

            if(!spawnCamera || !m_vSelectedSpawnVector)
                continue;

            if(loopCount > 5)
                AnimateWidget.Opacity(m_background, 0, UIConstants.FADE_RATE_DEFAULT, true);

            angle += speed;

            float x = m_vSelectedSpawnVector[0] + radius * Math.Cos(angle);
            float z = m_vSelectedSpawnVector[2] + radius * Math.Sin(angle);           
            
            SetCameraPos(spawnCamera, x, z, height);

            Sleep(20);

            loopCount++;
        }
    }
    void SetCameraPos(CameraBase spawnCamera, float x, float z, float height)
    {
        vector newMat[4];
        spawnCamera.GetWorldTransform(newMat);

        vector newCameraPosition = Vector(x, m_vSelectedSpawnVector[1] + height, z);
        newMat[3] = newCameraPosition;

        vector direction = vector.Direction(newCameraPosition, m_vSelectedSpawnVector);
        newMat[2] = direction;

        newMat[1] = Vector(0, 1, 0);

        spawnCamera.SetWorldTransform(newMat);
    }

    void ShowWarningText(string message)
    {
        if (m_warningText)
            m_warningText.SetText(message);
    }

    void ClearWarningText()
    {
        if (m_warningText)
            m_warningText.SetText("");
    }

    void OnCharacterCreationError(string errorMessage)
    {
        ShowWarningText(errorMessage);
    }
    void CloseMenu()
	{
        if(m_respawnComponent && m_bEnableCameraMoves)
        {
            m_bEnableCameraMoves = false;
            m_respawnComponent.RemoveRespawnCamera();
        }
		GetGame().GetMenuManager().CloseMenu(this);
	}
    void ~RL_RespawnUI()
	{
        Print("~RL_RespawnUI");

		m_bEnableCameraMoves = false;
        if (m_characterModelSelector)
			m_characterModelSelector.m_OnChanged.Remove(OnChangeCharacterModel);
	}
}