class RL_SeizeTrunkAction : ScriptedUserAction
{

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
        {
            RL_Utils.Notify("Seized items in vehicle", "POLICE");
			return;
        }		

        InventoryStorageManagerComponent storageManager = RL_InventoryUtils.GetResponsibleStorageManager(pOwnerEntity);
        if (!storageManager)
            return;
        
        array<BaseInventoryStorageComponent> storages = {};
        storageManager.GetStorages(storages);
        
		int deletedItemCount = 0;
        foreach (BaseInventoryStorageComponent storage : storages)
        {
			Print("[RL_SeizeTrunkAction] PerformAction foreach");
            if (storage.GetPurpose() != EStoragePurpose.PURPOSE_DEPOSIT)
                continue;
                
            array<IEntity> storageItems = {};
            storage.GetAll(storageItems);
            
            foreach (IEntity item : storageItems)
            {
				Print("[RL_SeizeTrunkAction] PerformAction foreach 2");
                //ResourceName prefab = RL_ShopUtils.ExtractEntityPrefabName(item);
                //if (prefab.IsEmpty())
                //    continue;
                SCR_EntityHelper.DeleteEntityAndChildren(item);
				deletedItemCount++;
            }
        }
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(pUserEntity);
		if(!character) return;
		if(deletedItemCount > 0)
			character.Notify(string.Format("Seized %1 items from the vehicle", deletedItemCount), "SEIZE");
		else
			character.Notify("No items in the vehicle to seize", "SEIZE");

	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Seize Trunk";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		
        SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		
		return userCharacter && userCharacter.IsPolice();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
	}
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
}