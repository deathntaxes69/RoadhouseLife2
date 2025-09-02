[EntityEditorProps(category: "NarcosLife/Building", description: "Pickup component for building blueprints")]
class RL_PickupComponentClass : ScriptComponentClass
{
	[Attribute("{65FE06414CE0B126}Prefabs/Building/RL_BasePlacementBlueprint.et", desc: "Blueprint prefab to create", category: "Blueprint")]
	ResourceName m_sBlueprintPrefab;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "Allow anyone to pickup", category: "Pickup Permissions")]
	bool m_bAllowAnyone;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "Allow only police to pickup", category: "Pickup Permissions")]
	bool m_bAllowOnlyPolice;
}

class RL_PickupComponent : ScriptComponent
{
	[RplProp()]
	protected string m_sOwnerUid = "";
	
	[RplProp()]
	protected string m_sUpgradeUid = "";
	
	protected bool m_bBeingPlaced = false;
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetBlueprintPrefab()
	{
		RL_PickupComponentClass componentData = RL_PickupComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_sBlueprintPrefab;
		return "{65FE06414CE0B126}Prefabs/Building/RL_BasePlacementBlueprint.et";
	}
	
	//------------------------------------------------------------------------------------------------
	string GetOwnerUid()
	{
		return m_sOwnerUid;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetOwnerUid(string ownerUid)
	{
		m_sOwnerUid = ownerUid;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUpgradeUid()
	{
		return m_sUpgradeUid;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpgradeUid(string upgradeUid)
	{
		m_sUpgradeUid = upgradeUid;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsDynamicUpgrade()
	{
		return !m_sUpgradeUid.IsEmpty();
	}
	
	//------------------------------------------------------------------------------------------------
	bool AllowAnyone()
	{
		RL_PickupComponentClass componentData = RL_PickupComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_bAllowAnyone;
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool AllowOnlyPolice()
	{
		RL_PickupComponentClass componentData = RL_PickupComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_bAllowOnlyPolice;
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsBeingPlaced()
	{
		return m_bBeingPlaced;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetBeingPlaced(bool beingPlaced)
	{
		m_bBeingPlaced = beingPlaced;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnPickedUp()
	{
		
		if (IsDynamicUpgrade() && !m_sOwnerUid.IsEmpty())
		{
			SCR_ChimeraCharacter ownerCharacter = RL_Utils.FindCharacterById(m_sOwnerUid);
			if (ownerCharacter)
				ownerCharacter.RemoveDynamicUpgrade(m_sUpgradeUid);
		}
	}
}
