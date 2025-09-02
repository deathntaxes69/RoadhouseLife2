modded class SCR_InventoryStorageBaseUI
{
	//------------------------------------------------------------------------------------------------
	// TODO Refactor later benjymin
	protected bool CanStackDrugItems(IEntity item1, IEntity item2)
	{
		RL_DrugComponent drugComp1 = RL_DrugComponent.Cast(item1.FindComponent(RL_DrugComponent));
		RL_DrugComponent drugComp2 = RL_DrugComponent.Cast(item2.FindComponent(RL_DrugComponent));
		
		if (drugComp1 && drugComp2)
		{
			if (drugComp1.GetQuality() != drugComp2.GetQuality())
				return false;
			if (drugComp1.GetName() != drugComp2.GetName())
				return false;
		}
		else if ((drugComp1 && !drugComp2) || (!drugComp1 && drugComp2))
		{
			return false;
		}
		
		RL_WeedSeedComponent weedComp1 = RL_WeedSeedComponent.Cast(item1.FindComponent(RL_WeedSeedComponent));
		RL_WeedSeedComponent weedComp2 = RL_WeedSeedComponent.Cast(item2.FindComponent(RL_WeedSeedComponent));
		
		if (weedComp1 && weedComp2)
		{
			if (weedComp1.GetStrainName() != weedComp2.GetStrainName())
				return false;
			if (weedComp1.GetQuality() != weedComp2.GetQuality())
				return false;
			if (weedComp1.GetYield() != weedComp2.GetYield())
				return false;
			if (weedComp1.GetGrowSpeedInterval() != weedComp2.GetGrowSpeedInterval())
				return false;
			if (weedComp1.IsIndoorOnly() != weedComp2.IsIndoorOnly())
				return false;
			if (weedComp1.GetColor() != weedComp2.GetColor())
				return false;
			
			array<string> soil1 = weedComp1.GetSuitableSoilTypes();
			array<string> soil2 = weedComp2.GetSuitableSoilTypes();
			if (soil1.Count() != soil2.Count())
				return false;
			
			for (int i = 0; i < soil1.Count(); i++)
			{
				Print("[RL_DrugStackFix] CanStackDrugItems for");
				if (soil1[i] != soil2[i])
					return false;
			}
		}
		else if ((weedComp1 && !weedComp2) || (!weedComp1 && weedComp2))
		{
			return false;
		}
		
		return true;
	}

	override protected void UpdateOwnedSlots(notnull array<IEntity> pItemsInStorage)
	{
		DeleteSlots();
		
		int iCompensationOffset = 0;
		array<SCR_InventorySlotUI> slotsToUpdate = {};
					
		for (int i = 0; i < pItemsInStorage.Count(); i++)
		{
			//Print("[RL_DrugStackFix] UpdateOwnedSlots for");
			InventoryItemComponent pComponent = GetItemComponentFromEntity( pItemsInStorage[i] );
			
			if ( !pComponent )
				continue;
			
			SCR_ItemAttributeCollection attributes = SCR_ItemAttributeCollection.Cast(pComponent.GetAttributes());
			bool stackable = (attributes && attributes.IsStackable());		
			int m = FindItem( pComponent );

			if ( m != -1 && stackable )
			{
				IEntity existingItem = m_aSlots[m].GetInventoryItemComponent().GetOwner();
				IEntity currentItem = pItemsInStorage[i];
				bool canStackDrugs = CanStackDrugItems(existingItem, currentItem);
				
				if (canStackDrugs)
				{
					RplComponent rplComp = RplComponent.Cast(pItemsInStorage[i].FindComponent(RplComponent));
					if (m_aSlots[m].IsInherited(SCR_SupplyInventorySlotUI) && rplComp)
					{
						m_aSlots[ m ].IncreaseStackNumberWithRplId(rplComp.Id()); 
						slotsToUpdate.Insert(m_aSlots[m]);
					}
					else
					{
						m_aSlots[ m ].IncreaseStackNumber();
					}

					iCompensationOffset++;
				}
				else
				{
					if ( i - iCompensationOffset > m_aSlots.Count()-1 )
						m_aSlots.Insert( CreateSlotUI(pComponent) );
					else
						m_aSlots[ i - iCompensationOffset++ ] = CreateSlotUI(pComponent);
				}
			}
			else
			{
				if ( i - iCompensationOffset > m_aSlots.Count()-1 )
					m_aSlots.Insert( CreateSlotUI(pComponent) );
				else
					m_aSlots[ i - iCompensationOffset++ ] = CreateSlotUI(pComponent);
			}
		}

		foreach (SCR_InventorySlotUI slot : slotsToUpdate)
		{
			//Print("[RL_DrugStackFix] UpdateOwnedSlots foreach loop");
			slot.OnOwnedSlotsUpdated();
		}
	}
}