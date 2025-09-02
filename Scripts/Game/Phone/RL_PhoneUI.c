class RL_PhoneUI : ChimeraMenuBase {
    
    // Core UI Elements
    protected Widget rootWidget;
    private Widget headerContainer;
    private Widget contentLayout;
    private Widget appGridsContainer;
    private GridLayoutWidget appGridContainer;
    private GridLayoutWidget appGridBottomContainer;
    private SCR_InputButtonComponent backButton;
    private Widget currentOpenApp;

    // Configuration Constants
	protected ResourceName RL_PHONE_APP_CONFIG_PATH = "{E62AEFC7D7AC22EA}Scripts/Game/Phone/RL_AppConfigs.conf";
    private const string PHONE_CALL_LAYOUT = "{0D3096C8A7A7C7FE}UI/Layouts/Phone/Apps/Call/Call.layout";
    private const string APP_GRID_ITEM_LAYOUT = "{A0F81AB763473238}UI/Layouts/Phone/App.layout";
    private const string PHONE_ICON_IMAGESET = "{F5A85B3176986520}UI/Imagesets/PhoneIcons/phoneAppIcons.imageset";
    private const int GRID_COLUMNS = 4;
    private const int GRID_ROWS = 4;

    // App Management
    private ref array<ref RL_AppConfig> availableApps;
    private ref array<ref RL_AppConfig> filteredApps = {};
    private ref array<ref SCR_ModularButtonComponent> appButtons = {};

    protected SCR_ChimeraCharacter playerCharacter;
    
    // Initialize available applications
    private void InitializeApplications() {
        RL_AppConfigList appConfigs = SCR_ConfigHelperT<RL_AppConfigList>.GetConfigObject(RL_PHONE_APP_CONFIG_PATH);
        availableApps = appConfigs.m_appConfigs;
    }
    
    override void OnMenuOpen() {
        InitializeUI();
        InitializeApplications();
        
        playerCharacter = RL_Utils.GetLocalCharacter();

        if (IsInPhoneCall()) {
            OpenCallInterface();
        } else {
            PopulateApplicationGrid();
        }
        
        RegisterInputHandlers();
    }

    // Initialize UI components
    private void InitializeUI() {
        rootWidget = GetRootWidget();
        super.OnMenuOpen();
        
        headerContainer = Widget.Cast(rootWidget.FindAnyWidget("PhoneHeader"));
        contentLayout = Widget.Cast(rootWidget.FindAnyWidget("PhoneContentLayout"));
        appGridsContainer = Widget.Cast(rootWidget.FindAnyWidget("PhoneAppGrids"));
        appGridContainer = GridLayoutWidget.Cast(rootWidget.FindAnyWidget("PhoneAppGrid"));
        appGridBottomContainer = GridLayoutWidget.Cast(rootWidget.FindAnyWidget("PhoneAppBottomGrid"));
        backButton = SCR_InputButtonComponent.GetInputButtonComponent("BackButton", rootWidget);
    }

    // Register input event handlers
    private void RegisterInputHandlers() {
        GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
    }

    // Check if character is currently in a phone call
    private bool IsInPhoneCall() {
        return playerCharacter && playerCharacter.GetPhoneStatus();
    }

    // Populate the main application grid
    void PopulateApplicationGrid() {
        ClearApplicationGrid();
        ResetFilteredApps();
        
        int currentRow = 4;
        int currentColumn = 0;
        
        foreach(RL_AppConfig app: availableApps) {
            if (!ShouldShowApp(app)) continue;
            
            Widget appWidget = CreateApplicationWidget(currentRow, currentColumn);
            ConfigureApplicationWidget(appWidget, app);
            
            filteredApps.Insert(app);
            
            UpdateGridPosition(currentRow, currentColumn);
        }
    }

    // Clear all existing apps from grid
    private void ClearApplicationGrid() {
        SCR_WidgetHelper.RemoveAllChildren(appGridContainer);
        SCR_WidgetHelper.RemoveAllChildren(appGridBottomContainer);
    }

    // Reset filtered apps and buttons arrays
    private void ResetFilteredApps() {
        filteredApps.Clear();
        appButtons.Clear();
    }

    // Determine if an app should be shown based on character type
    private bool ShouldShowApp(RL_AppConfig app) {
        switch(app.m_tPhoneAppType) {
            case RL_APP_TYPE.Civ:
                return !playerCharacter.IsPolice() && !playerCharacter.IsMedic();
            case RL_APP_TYPE.Police:
                return playerCharacter.IsPolice();
            case RL_APP_TYPE.Medic:
                return playerCharacter.IsMedic();
            case RL_APP_TYPE.Generic:
                return true;
        }
        return false;
    }

    // Create and position an application widget
    Widget CreateApplicationWidget(int row, int column) {
        Widget container;
        if(row == 4)
            container = GetGame().GetWorkspace().CreateWidgets(APP_GRID_ITEM_LAYOUT, appGridBottomContainer);
        else
            container = GetGame().GetWorkspace().CreateWidgets(APP_GRID_ITEM_LAYOUT, appGridContainer);
        GridSlot.SetColumn(container, column);
        GridSlot.SetRow(container, row);
        return container;
    }

    // Configure application widget with app data
    private void ConfigureApplicationWidget(Widget appWidget, RL_AppConfig app) {
        ImageWidget appIcon = ImageWidget.Cast(appWidget.FindAnyWidget("AppImage"));
        ButtonWidget appButton = ButtonWidget.Cast(appWidget.FindAnyWidget("AppButton"));
        TextWidget appLabel = TextWidget.Cast(appWidget.FindAnyWidget("AppLabel"));

        // Set app icon and label
        appIcon.LoadImageFromSet(0, PHONE_ICON_IMAGESET, app.m_sPhoneAppImagesetName);
        appLabel.SetText(app.m_sPhoneAppName);
        
        // Setup button interaction
        SCR_ModularButtonComponent buttonComponent = SCR_ModularButtonComponent.FindComponent(appButton);
        buttonComponent.m_OnClicked.Insert(HandleAppSelection);
        appButtons.Insert(buttonComponent);
    }

    // Update grid position for next app (bottom-to-top filling)
    private void UpdateGridPosition(inout int row, inout int column) {
        column++;
        
        // Special handling for row 4 - only allow GRID_COLUMNS-1 apps
        if (row == 4 && column >= GRID_COLUMNS - 2) {
            column = 0;
            row--;
        }
        // Normal handling for other rows
        else if (column >= GRID_COLUMNS) {
            column = 0;
            row--;
        }
    }


    // Handle application selection
    private void HandleAppSelection(SCR_ModularButtonComponent selectedButton) {
        if(!playerCharacter || playerCharacter.IsSpamming()) return;
        RL_AppConfig selectedApp = FindAppByButton(selectedButton);
        if (selectedApp) {
            SwitchApp(selectedApp.m_sPhoneAppName);
        }
    }

    // Find app configuration by button component
    private RL_AppConfig FindAppByButton(SCR_ModularButtonComponent targetButton) {
        foreach(int index, SCR_ModularButtonComponent button : appButtons) {
            Print("[RL_PhoneUI] FindAppByButton foreach loop");
            if (button == targetButton) {
                return filteredApps[index];
            }
        }
        return null;
    }

    // Open specific application by name
    Widget SwitchApp(string appName, bool skipInit = false) {
        RL_AppConfig appConfig = FindAppByName(appName);
        if (!appConfig) return null;

        CloseCurrentApp();
        ShowApplicationInterface();
        
        currentOpenApp = CreateAppWidget(appConfig);
        if(!skipInit)
            InitializeSpecialApps(appName);
        
        return currentOpenApp;
    }

    // Find app configuration by name
    private RL_AppConfig FindAppByName(string appName) {
        foreach(RL_AppConfig app : availableApps) {
            Print("[RL_PhoneUI] FindAppByName foreach loop");
            if (app.m_sPhoneAppName == appName) {
                return app;
            }
        }
        return null;
    }

    // Create application widget from configuration
    private Widget CreateAppWidget(RL_AppConfig appConfig) {
        Widget appWidget = GetGame().GetWorkspace().CreateWidgets(appConfig.m_sPhoneAppLayout, contentLayout);
        RL_AppBaseWidget appBase = RL_AppBaseWidget.Cast(appWidget.FindHandler(RL_AppBaseWidget));
        appBase.SetPhoneMenu(this);
        return appWidget;
    }

    // Initialize special applications that need extra setup
    private void InitializeSpecialApps(string appName) {
        switch(appName) {
            case "Contacts":
                InitializeContactsApp();
                break;
            case "Warrants":
                InitializeWarrantsApp();
                break;
            case "Keys":
                InitializeKeysApp();
                break;
            case "Messages":
                InitializeTextApp();
                break;
        }
    }

    // Initialize contacts application
    private void InitializeContactsApp() {
        RL_ContactsWidget contactsWidget = RL_ContactsWidget.Cast(currentOpenApp.FindHandler(RL_ContactsWidget));
        if (contactsWidget) {
            playerCharacter.GetPlayerList(contactsWidget, "FillContactList");
        }
    }

    // Initialize warrants application
    private void InitializeWarrantsApp() {
        RL_WarrantsWidget warrantsWidget = RL_WarrantsWidget.Cast(currentOpenApp.FindHandler(RL_WarrantsWidget));
        if (warrantsWidget) {
            playerCharacter.GetPlayerList(warrantsWidget, "FillSuspectList");
        }
    }

    // Initialize keys application
    private void InitializeKeysApp() {
        RL_KeysWidget keysWidget = RL_KeysWidget.Cast(currentOpenApp.FindHandler(RL_KeysWidget));
        if (keysWidget) {
            keysWidget.RefreshAllData();
        }
    }

    // Initialize keys application
    private void InitializeTextApp() {
        RL_TextWidget textWidget = RL_TextWidget.Cast(currentOpenApp.FindHandler(RL_TextWidget));
        if (textWidget) {
            textWidget.GetLastTexts();
        }
    }

    // Open phone call interface
    void OpenCallInterface() {
        ShowApplicationInterface();
        currentOpenApp = GetGame().GetWorkspace().CreateWidgets(PHONE_CALL_LAYOUT, contentLayout);
        
        RL_CallWidget callWidget = RL_CallWidget.Cast(currentOpenApp.FindHandler(RL_CallWidget));
        callWidget.SetPhoneMenu(this);
    }

    // Show application interface (hide grid, show header)
    private void ShowApplicationInterface() {
        appGridsContainer.SetVisible(false);
        headerContainer.SetVisible(true);
    }

    // Show main grid interface (show grid, hide header)
    private void ShowGridInterface() {
        appGridsContainer.SetVisible(true);
        headerContainer.SetVisible(false);
    }

    // Close current open application
    private void CloseCurrentApp() {
        if (currentOpenApp) {
            delete currentOpenApp;
            currentOpenApp = null;
        }
    }

    // Get back button component for external access
    SCR_InputButtonComponent GetBackButton() {
        return backButton;
    }

    // Return to main application grid
    void BackToHome() {
        CloseCurrentApp();
        PopulateApplicationGrid();
        ShowGridInterface();
    }

    // Close entire phone interface
    void CloseMenu() {
        CloseCurrentApp();
        GetGame().GetMenuManager().CloseMenu(this);
    }
}