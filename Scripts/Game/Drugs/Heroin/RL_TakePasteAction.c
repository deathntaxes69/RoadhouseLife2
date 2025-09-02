class RL_TakePasteAction : ScriptedUserAction
{
	protected static const ResourceName HEROIN_PASTE_PREFAB = "{D223921CF3719B65}Prefabs/Supplies/Heroin_Paste.et";
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
			
		if (!pOwnerEntity || !pUserEntity)
			return;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(pOwnerEntity.FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp || !campfireComp.CanTakePaste())
			return;
			
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(pUserEntity);
		if (!inventoryManager)
			return;
			
		int pasteCount = campfireComp.GetPoppyCount();
		float outputQuality = campfireComp.GetOutputQuality();
		int actualPasteCount = campfireComp.TakeAllPaste();
		
		if (actualPasteCount > 0)
		{
			array<RplId> addedItemIds = {};
			int addedCount = RL_InventoryUtils.AddAmountWithIds(inventoryManager, HEROIN_PASTE_PREFAB, actualPasteCount, addedItemIds, true);
			ApplyQualityToPaste(addedItemIds, outputQuality);
			
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
			if (character)
			{
				int qualityPercent = Math.Floor(outputQuality * 100);
				string qualityText = qualityPercent.ToString() + "%";
				string message;
				
				int excessCount = actualPasteCount - addedCount;
				if (excessCount > 0)
				{
					message = string.Format("Took %1 heroin paste (%2 quality), %3 dropped on ground", 
						addedCount, qualityText, excessCount);
				}
				else
				{
					message = string.Format("Took %1 heroin paste (%2 quality)", actualPasteCount, qualityText);
				}
				
				character.Notify(message, "HEROIN");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ApplyQualityToPaste(array<RplId> itemIds, float quality)
	{
		foreach (RplId itemId : itemIds)
		{
			Print("[RL_TakePasteAction] ApplyQualityToPaste foreach loop");
			IEntity pasteItem = EPF_NetworkUtils.FindEntityByRplId(itemId);
			if (!pasteItem)
				continue;
				
			RL_DrugComponent drugComp = RL_DrugComponent.Cast(pasteItem.FindComponent(RL_DrugComponent));
			if (!drugComp)
				continue;
				
			drugComp.SetDrugInfo("Heroin", quality);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!user || !GetOwner())
			return false;
			
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
			return false;
			
		return campfireComp.IsCooking() || campfireComp.CanTakePaste();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!user || !GetOwner())
		{
			SetCannotPerformReason("Invalid entity");
			return false;
		}
		
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (!campfireComp)
		{
			SetCannotPerformReason("No campfire component");
			return false;
		}
		
		if (!campfireComp.CanTakePaste())
		{
			if (!campfireComp.IsCookingComplete())
			{
				if (campfireComp.IsCooking())
				{
					int progress = Math.Floor(campfireComp.GetCookingProgress() * 100);
					SetCannotPerformReason(string.Format("Still cooking... %1%", progress));
				}
				else
					SetCannotPerformReason("Nothing to take");
			}
			else
				SetCannotPerformReason("Cannot take paste");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		RL_HeroinCampfireComponent campfireComp = RL_HeroinCampfireComponent.Cast(GetOwner().FindComponent(RL_HeroinCampfireComponent));
		if (campfireComp)
		{
			if (campfireComp.IsCooking())
			{
				int progress = Math.Floor(campfireComp.GetCookingProgress() * 100);
				int pasteCount = campfireComp.GetPoppyCount();
				outName = string.Format("Cooking... %1%% (%2x paste)", progress, pasteCount);
			}
			else if (campfireComp.IsCookingComplete())
			{
				int pasteCount = campfireComp.GetPoppyCount();
				float quality = campfireComp.GetOutputQuality();
				int qualityPercent = Math.Floor(quality * 100);
				outName = string.Format("Take Paste (%1x @ %2%%)", pasteCount, qualityPercent);
			}
			else
			{
				outName = "Take Heroin Paste";
			}
		}
		else
		{
			outName = "Take Heroin Paste";
		}
		return true;
	}
}
