[EntityEditorProps(category: "NarcosLife/Core/Components", description:"Component that registers entity location with the location manager")]
class RL_RegisterEntityLocationClass : ScriptComponentClass
{
	[Attribute(defvalue: "1", desc: "Entity Type", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ELocationEntityType))]
	ELocationEntityType m_eEntityType;
}

class RL_RegisterEntityLocation : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	ELocationEntityType GetEntityType()
	{
		RL_RegisterEntityLocationClass componentData = RL_RegisterEntityLocationClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_eEntityType;
		else
			return ELocationEntityType.GENERIC;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		RL_EntityLocationManager manager = RL_EntityLocationManager.GetInstance();
		if (manager)
			manager.RegisterEntity(owner, GetEntityType());
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		RL_EntityLocationManager manager = RL_EntityLocationManager.GetInstance();
		if (manager)
			manager.UnregisterEntity(owner);
			
		super.OnDelete(owner);
	}
} 