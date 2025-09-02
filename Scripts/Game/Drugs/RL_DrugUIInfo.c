[BaseContainerProps()]
class RL_DrugUIInfo : SCR_InventoryUIInfo
{
	// TODO Refactor later benjymin
	
	//------------------------------------------------------------------------------------------------
	override string GetInventoryItemName(InventoryItemComponent item)
	{
		if (!item)
			return GetName();
			
		IEntity owner = item.GetOwner();
		if (!owner)
			return GetName();
		
		RL_WeedSeedComponent weedComp = RL_WeedSeedComponent.Cast(owner.FindComponent(RL_WeedSeedComponent));
		if (weedComp)
		{
			string strainName = weedComp.GetStrainName();
			if (strainName != "" && strainName != "Unidentified")
			{
				string suffix = "Bud";
				EntityPrefabData prefabData = owner.GetPrefabData();
				if (prefabData)
				{
					ResourceName prefabName = prefabData.GetPrefabName();
					string prefabPath = prefabName.GetPath();
					if (prefabPath.Contains("Seed_Bag"))
					{
						suffix = "Seeds";
					}
				}
				
				return string.Format("%1 %2", strainName, suffix);
			}
		}
		
		RL_DrugComponent drugComp = RL_DrugComponent.Cast(owner.FindComponent(RL_DrugComponent));
		if (drugComp)
		{
			string drugName = drugComp.GetName();
			if (drugName != "" && drugName != "Unidentified Drug")
			{
				return drugName;
			}
		}
		
		return GetName();
	}
	
	//------------------------------------------------------------------------------------------------
	override string GetInventoryItemDescription(InventoryItemComponent item)
	{
		if (!item)
			return GetDescription();
			
		IEntity owner = item.GetOwner();
		if (!owner)
			return GetDescription();
		
		string description = "";
		
		RL_WeedSeedComponent weedComp = RL_WeedSeedComponent.Cast(owner.FindComponent(RL_WeedSeedComponent));
		if (weedComp)
		{
			string strainName = weedComp.GetStrainName();
			if (strainName != "" && strainName != "Unidentified")
			{
				description += string.Format("Quality: %1", weedComp.GetQuality());
				description += string.Format("\nYield: %1", weedComp.GetYield());
				
				float growTimeSeconds = weedComp.GetGrowSpeedInterval() * 7;
				description += string.Format("\nSpeed: %1", RL_Utils.FormatTimeDisplay(growTimeSeconds));
				
				description += "\nSuitable Soil: ";
				array<string> soilTypes = weedComp.GetSuitableSoilTypes();
				if (soilTypes && soilTypes.Count() > 0)
				{
					string soilString = "";
					for (int i = 0; i < soilTypes.Count(); i++)
					{
						Print("[RL_DrugUIInfo] GetInventoryItemDescription for");
						if (i > 0) 
							soilString += ", ";
						soilString += soilTypes[i];
					}
					description += soilString;
				}
				else
				{
					description += "Any";
				}
				
				description += "\n";
				if (weedComp.IsIndoorOnly())
				{
					description += "Indoor";
				}
				else
				{
					description += "Outdoor";
				}
				
				return description;
			}
		}
		
		RL_DrugComponent drugComp = RL_DrugComponent.Cast(owner.FindComponent(RL_DrugComponent));
		if (drugComp)
		{
			float qualityPercent = drugComp.GetQuality() * 100;
			description += string.Format("Quality: %1%%", qualityPercent.ToString(-1, 1));
			return description;
		}
		
		return GetDescription();
	}
} 