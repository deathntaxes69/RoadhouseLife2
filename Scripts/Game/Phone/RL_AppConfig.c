enum RL_APP_TYPE
{
	Generic,
    Civ,
    Police,
    Medic
}
[BaseContainerProps(configRoot: true)]
class RL_AppConfigList : Managed
{	
	[Attribute(defvalue: "", category: "", desc: "")]
	ref array<ref RL_AppConfig> m_appConfigs;
}

[BaseContainerProps(configRoot: true)]
class RL_AppConfig : Managed
{	

    [Attribute(defvalue: "App Name", category: "", desc: "")]
    string m_sPhoneAppName;
    

    [Attribute(defvalue: "1", category: "", desc: "", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RL_APP_TYPE))]
	RL_APP_TYPE m_tPhoneAppType;

    [Attribute(defvalue: "BANK", category: "", desc: "")]
    string m_sPhoneAppImagesetName;

    [Attribute(defvalue: "", category: "", desc: "")]
    ResourceName m_sPhoneAppLayout;
}

