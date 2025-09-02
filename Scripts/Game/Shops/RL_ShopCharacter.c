modded class SCR_ChimeraCharacter
{
	protected ref RL_CharacterDbHelper m_characterHelper;
	protected ref map<string, ref array<string>> m_contextCache = new map<string, ref array<string>>();
	protected ref array<string> m_test = {};

	void PurchaseItems(string prefab, int quantity, int totalPrice, string characterId)
	{
		int currentCash = this.GetCash();
		if (totalPrice > currentCash)
		{
			RL_Utils.Notify("You don't have enough cash.", "SHOP");
			return;
		}
		Rpc(RpcAsk_PurchaseItems, prefab, quantity, totalPrice, characterId);
	}

	void PurchaseFurniture(string prefab, int totalPrice, string characterId)
	{
		int currentCash = this.GetCash();
		if (totalPrice > currentCash)
		{
			RL_Utils.Notify("You don't have enough cash.", "SHOP");
			return;
		}
		Rpc(RpcAsk_PurchaseFurniture, prefab, totalPrice, characterId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_PurchaseItems(string prefab, int quantity, int totalPrice, string characterId)
	{
		Print("RpcAsk_PurchaseItems");
		Tuple4<string, int, int, string> context(prefab, quantity, totalPrice, characterId);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, (-totalPrice), 0, this, "PurchaseCallbackItems", context);

		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("location", RL_Utils.GetCharacterByCharacterIdLocal(characterId).GetOrigin().ToString());
		jsonData.WriteValue("prefab", prefab);
		jsonData.WriteValue("quantity", quantity);
		jsonData.WriteValue("totalPrice", totalPrice);
		jsonData.WriteValue("shop", "Items");

		m_characterHelper.PlayerLog(characterId, "Shop Purchase", jsonData.ExportToString());
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_PurchaseFurniture(string prefab, int totalPrice, string characterId)
	{
		Print("RpcAsk_PurchaseFurniture");
		Tuple3<string, int, string> context(prefab, totalPrice, characterId);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, (-totalPrice), 0, this, "PurchaseCallbackFurniture", context);

		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("location", RL_Utils.GetCharacterByCharacterIdLocal(characterId).GetOrigin().ToString());
		jsonData.WriteValue("prefab", prefab);
		jsonData.WriteValue("quantity", 1);
		jsonData.WriteValue("totalPrice", totalPrice);
		jsonData.WriteValue("shop", "Furniture");

		m_characterHelper.PlayerLog(characterId, "Shop Purchase", jsonData.ExportToString());
	}

	void PurchaseCallbackItems(bool success, Managed context)
	{
		// Prefab, quantity, total price, character ID
		Tuple4<string, int, int, string> typedContext = Tuple4<string, int, int, string>.Cast(context);
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(typedContext.param4);
		//Rpc(RpcDo_PurchaseCallback, success);
		Rpc(RpcDo_TransactCallbackItems, success, typedContext.param1, typedContext.param2, typedContext.param3, false);
		if (success && typedContext.param1)
		{
			RL_InventoryUtils.AddAmount(character, typedContext.param1, typedContext.param2, true);
		}
	}

	void PurchaseCallbackFurniture(bool success, Managed context)
	{
		Tuple3<string, int, string> typedContext = Tuple3<string, int, string>.Cast(context);
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(typedContext.param3);
		Rpc(RpcDo_TransactCallbackFurniture, success, typedContext.param1, typedContext.param2);
		if (success && typedContext.param1)
		{
			RL_InventoryUtils.AddAmount(character, typedContext.param1, 1, true);
		}
	}

	void PurchaseClothing(array<string> prefabs, int totalPrice, string characterId)
	{

		Rpc(RpcAsk_PurchaseClothing, prefabs, totalPrice, characterId);
	}

	void PurchaseClothingWithItemTransfer(array<string> prefabs, int totalPrice, string characterId, map<ResourceName, int> storedItems)
	{
		int mapSize = storedItems.Count();
		array<string> itemPrefabs = {};
		array<int> itemQuantities = {};
		itemPrefabs.Reserve(mapSize);
		itemQuantities.Reserve(mapSize);
		
		foreach (ResourceName prefab, int quantity : storedItems)
		{
			Print("[RL_ShopCharacter] PurchaseClothingWithItemTransfer foreach");
			itemPrefabs.Insert(prefab);
			itemQuantities.Insert(quantity);
		}
		
		Rpc(RpcAsk_PurchaseClothingWithItemTransfer, prefabs, totalPrice, characterId, itemPrefabs, itemQuantities);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_PurchaseClothing(array<string> prefabs, int totalPrice, string characterId)
	{
		PrintFormat("RpcAsk_PurchaseClothing m_contextCache count %1", m_contextCache.Count());
		// Cache purchase context
		string contextKey = RL_Utils.GenerateRandomUid();
		m_contextCache.Insert(contextKey, prefabs);

		Tuple2<string, string> context(contextKey, characterId);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, (-totalPrice), 0, this, "PurchaseCallbackClothing", context);

		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("location", RL_Utils.GetCharacterByCharacterIdLocal(characterId).GetOrigin().ToString());
		jsonData.WriteValue("prefabs", prefabs);
		jsonData.WriteValue("totalPrice", totalPrice);
		jsonData.WriteValue("shop", "Clothing");

		m_characterHelper.PlayerLog(characterId, "Shop Purchase", jsonData.ExportToString());
		//Print("TEST BEFORE CALLBACK");
		//Print(context.param1);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_PurchaseClothingWithItemTransfer(array<string> prefabs, int totalPrice, string characterId, array<string> itemPrefabs, array<int> itemQuantities)
	{
		PrintFormat("RpcAsk_PurchaseClothingWithItemTransfer m_contextCache count %1", m_contextCache.Count());
		string clothingIndex = RL_Utils.GenerateRandomUid();
		m_contextCache.Insert(clothingIndex, prefabs);
		
		string itemsIndex = RL_Utils.GenerateRandomUid();
		m_contextCache.Insert(itemsIndex, itemPrefabs);

		array<string> quantityStrings = {};
		int quantitiesCount = itemQuantities.Count();
		quantityStrings.Reserve(quantitiesCount);
		
		for (int i = 0; i < quantitiesCount; i++)
		{
			Print("[RL_ShopCharacter] RpcAsk_PurchaseClothingWithItemTransfer foreach");
			quantityStrings.Insert(itemQuantities[i].ToString());
		}
		string quantitiesIndex = RL_Utils.GenerateRandomUid();
		m_contextCache.Insert(quantitiesIndex, quantityStrings);

		Tuple4<string, string, string, string> context(clothingIndex, itemsIndex, quantitiesIndex, characterId);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, (-totalPrice), 0, this, "PurchaseCallbackClothingWithItemTransfer", context);

		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("location", RL_Utils.GetCharacterByCharacterIdLocal(characterId).GetOrigin().ToString());
		jsonData.WriteValue("prefabs", prefabs);
		jsonData.WriteValue("totalPrice", totalPrice);
		jsonData.WriteValue("shop", "Clothing");
		m_characterHelper.PlayerLog(characterId, "Shop Purchase", jsonData.ExportToString());
	}

	void PurchaseCallbackClothing(bool success, Managed context)
	{
		// m_contextCache index, character ID
		Tuple2<string, string> typedContext = Tuple2<string, string>.Cast(context);
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(typedContext.param2);
		Rpc(RpcDo_PurchaseCallback, success);

		if (success && m_contextCache.Get(typedContext.param1))
		{
			foreach (string prefab : m_contextCache.Get(typedContext.param1))
			{
				Print("[RL_ShopCharacter] PurchaseCallbackClothing foreach");
				RL_ShopUtils.SwapEntityClothing(character, prefab, false);
			}

		}
		m_contextCache.Remove(typedContext.param1)
	}

    void PurchaseCallbackClothingWithItemTransfer(bool success, Managed context)
	{
		Tuple4<string, string, string, string> typedContext = Tuple4<string, string, string, string>.Cast(context);
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(typedContext.param4);
		Rpc(RpcDo_PurchaseCallback, success);


		if(success)
		{
			array<string> clothingPrefabs = m_contextCache.Get(typedContext.param1);
			array<string> itemPrefabs = m_contextCache.Get(typedContext.param2);
			array<string> quantityStrings = m_contextCache.Get(typedContext.param3);
			
			InventoryStorageManagerComponent storageManager = RL_InventoryUtils.GetResponsibleStorageManager(character);
			bool hasValidItems = itemPrefabs && quantityStrings && itemPrefabs.Count() == quantityStrings.Count() && storageManager;
			
			array<IEntity> removedItems = {};
			if (hasValidItems)
			{
				RemoveStoredItems(storageManager, itemPrefabs, quantityStrings, removedItems);
			}
			
			if (clothingPrefabs)
			{
				foreach (string prefab : clothingPrefabs)
				{
					Print("[RL_ShopCharacter] PurchaseCallbackClothingWithItemTransfer foreach");
					RL_ShopUtils.SwapEntityClothing(character, prefab, false);
				}
			}

			if (hasValidItems)
			{
				RestoreStoredItems(storageManager, removedItems);
			}
		}
		m_contextCache.Remove(typedContext.param1);
		m_contextCache.Remove(typedContext.param2);
		m_contextCache.Remove(typedContext.param3);

	}

    protected void RemoveStoredItems(InventoryStorageManagerComponent storageManager, array<string> itemPrefabs, array<string> quantityStrings, out array<IEntity> outRemovedItems)
	{
        outRemovedItems = {};
        if (!storageManager || itemPrefabs.IsEmpty() || quantityStrings.IsEmpty())
            return;

        int prefabCount = itemPrefabs.Count();
        int quantityCount = quantityStrings.Count();
        int processCount = Math.Min(prefabCount, quantityCount);
        array<int> quantities = {};
        quantities.Reserve(processCount);
        int totalRequested = 0;
        
        for (int i = 0; i < processCount; i++)
        {
            int quantity = Math.Max(0, quantityStrings[i].ToInt());
            quantities.Insert(quantity);
            totalRequested += quantity;
        }
        
        if (totalRequested > 0)
            outRemovedItems.Reserve(totalRequested);

		for (int i = 0; i < processCount; i++)
		{
			Print("[RL_ShopCharacter] RemoveStoredItems for");
			string prefab = itemPrefabs[i];
			int quantity = quantities[i];
            if (quantity == 0)
                continue;

            array<IEntity> foundItems = RL_InventoryUtils.FindItemsByPrefab(storageManager, prefab);
            if (foundItems.IsEmpty())
                continue;

            int removedCount = 0;
            int foundCount = foundItems.Count();
            int maxIterations = Math.Min(foundCount, quantity);
            
            for (int j = 0; j < maxIterations && removedCount < quantity; j++)
            {
                IEntity item = foundItems[j];
                InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
                if (!itemComponent)
                    continue;

                InventoryStorageSlot parentSlot = itemComponent.GetParentSlot();
                if (!parentSlot)
                    continue;

                BaseInventoryStorageComponent fromStorage = parentSlot.GetStorage();
                if (!fromStorage)
                    continue;

                if (storageManager.TryRemoveItemFromStorage(item, fromStorage))
                {
                    outRemovedItems.Insert(item);
                    removedCount++;
                }
            }
		}
	}

    protected void RestoreStoredItems(InventoryStorageManagerComponent storageManager, array<IEntity> removedItems)
	{
        if (!storageManager || removedItems.IsEmpty())
            return;

        ref map<ResourceName, int> failedInsertCountByPrefab = new map<ResourceName, int>();
        int itemCount = removedItems.Count();

        for (int i = 0; i < itemCount; i++)
        {
            Print("[RL_ShopCharacter] RestoreStoredItems for");
            IEntity item = removedItems[i];
            if (!item)
                continue;

            if (!storageManager.TryInsertItem(item))
            {
                ResourceName prefabName = RL_ShopUtils.ExtractEntityPrefabName(item);
                int current = failedInsertCountByPrefab.Get(prefabName);
                failedInsertCountByPrefab.Set(prefabName, current + 1);
            }
        }

        foreach (ResourceName prefabKey, int droppedCount : failedInsertCountByPrefab)
        {
            if (droppedCount > 0)
            {
                string itemName = RL_ShopUtils.GetEntityDisplayName(prefabKey);
                RL_Utils.Notify(string.Format("Dropped %1x %2 on ground (no space)", droppedCount, itemName));
            }
        }
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_PurchaseCallback(bool success)
	{
		if (!success)
		{
			RL_Utils.Notify("Transaction Failed.", "SHOP");
			return;
		}

		RL_Utils.Notify("Purchase Successful.", "SHOP");
	}

	void SellAllItems(string prefab, int sellPriceEach, string characterId)
	{
		int totalQuantity = RL_InventoryUtils.GetAmount(this, prefab);
		PrintFormat("SellAllItems characterId=%1 totalQuantity=%2 prefab=%3", characterId, totalQuantity, prefab);
		if (!totalQuantity || totalQuantity < 1)
		{
			RL_Utils.Notify("You don't have any of these items to sell.", "SHOP");
			return;
		}
		Rpc(RpcAsk_SellItems, prefab, totalQuantity, sellPriceEach, characterId);
	}

	void SellItems(string prefab, int quantity, int sellPriceEach, string characterId)
	{
		int totalQuantity = RL_InventoryUtils.GetAmount(this, prefab);
		if (!totalQuantity || totalQuantity < quantity)
		{
			RL_Utils.Notify("You don't have this many items to sell.", "SHOP");
			return;
		}
		Rpc(RpcAsk_SellItems, prefab, quantity, sellPriceEach, characterId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_SellItems(string prefab, int quantity, int sellPriceEach, string characterId)
	{
		SCR_ChimeraCharacter character = RL_Utils.FindCharacterById(characterId);
		
		int availableQuantity = RL_InventoryUtils.GetAmount(character, prefab);
		if(quantity != availableQuantity)
		{
			PrintFormat("RpcAsk_SellItems - Quantity mismatch, Character ID: %1 , Requested: %2 , Available: %3 , Prefab: %4", characterId, quantity, availableQuantity, prefab);
		}
		int actualQuantityToSell = Math.Min(quantity, availableQuantity);
		int totalSellPrice = RL_ShopUtils.CalculateTotalQualityAdjustedSellPrice(character, prefab, actualQuantityToSell, sellPriceEach);
		int quantityRemoved = RL_InventoryUtils.RemoveAmount(character, prefab, actualQuantityToSell);

		PrintFormat("RpcAsk_SellItems Character ID: %1 , Total: %2 , Quantity: %3 , Prefab: %4", characterId, totalSellPrice, quantity, prefab);

		Tuple3<string, int, int> context(prefab, quantityRemoved, totalSellPrice);
		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, totalSellPrice, 0, this, "SellCallbackItems", context);

		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		jsonData.WriteValue("location", character.GetOrigin().ToString());
		jsonData.WriteValue("prefab", prefab);
		jsonData.WriteValue("sellPriceEach", sellPriceEach);
		jsonData.WriteValue("quantity", quantity);
		jsonData.WriteValue("totalSellPrice", totalSellPrice);
		jsonData.WriteValue("shop", "Items");

		m_characterHelper.PlayerLog(characterId, "Shop Sell", jsonData.ExportToString());
	}

	void SellCallbackItems(bool success, Managed context)
	{
		// Prefab, quantity removed, total sell price
		Tuple3<string, int, int> typedContext = Tuple3<string, int, int>.Cast(context);
		Rpc(RpcDo_TransactCallbackItems, success, typedContext.param1, typedContext.param2, typedContext.param3, true);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_TransactCallbackItems(bool success, string prefab, int quantity, int totalPrice, bool selling)
	{
		string displayName = RL_ShopUtils.GetEntityDisplayName(prefab);
		if (!displayName)
			displayName = "ERROR";

		if (!success)
		{
			RL_Utils.Notify("Transaction Failed.", "SHOP");
			// Selling shouldn't fail so we still want to notify the amount returned
			if (!selling)
				return;
		}
		if (selling)
			RL_Utils.Notify(string.Format("Sold %1 x %2 for %3", quantity, displayName, RL_Utils.FormatMoney(totalPrice)), "SHOP");
		else
			RL_Utils.Notify(string.Format("Purchased %1 x %2 for %3", quantity, displayName, RL_Utils.FormatMoney(totalPrice)), "SHOP");
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_TransactCallbackFurniture(bool success, string prefab, int totalPrice)
	{
		string displayName = RL_ShopUtils.GetEntityDisplayName(prefab);
		if (!displayName)
			displayName = "ERROR";

		if (!success)
		{
			RL_Utils.Notify("Transaction Failed.", "SHOP");
			return;
		}
		
		RL_Utils.Notify(string.Format("Purchased %1 for %2", displayName, RL_Utils.FormatMoney(totalPrice)), "SHOP");
	}

}
