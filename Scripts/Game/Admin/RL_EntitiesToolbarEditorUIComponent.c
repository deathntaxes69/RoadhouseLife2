
//! @ingroup Editor_UI Editor_UI_Components

modded class SCR_EntitiesToolbarEditorUIComponent : SCR_BaseToolbarEditorUIComponent
{
	protected int m_iUpdateInterval = 1;
	protected int m_iLastUpdateTime = 0;	
	
	//------------------------------------------------------------------------------------------------
	override protected void Refresh()
	{
		int currentTime = System.GetUnixTime();

		// throttle updates
		if ((currentTime - m_iLastUpdateTime) < m_iUpdateInterval)
			return;
		
		m_iLastUpdateTime = currentTime;
		Print("SCR_EntitiesToolbarEditorUIComponent");
		if (m_State != -1)
		{
			SCR_BaseEditableEntityFilter filter = SCR_BaseEditableEntityFilter.GetInstance(m_State, true);
			
			set<SCR_EditableEntityComponent> entities = new set<SCR_EditableEntityComponent>();
			int count = filter.GetEntities(entities);
			
			m_bHasEntities = false;
			m_Entities.Clear();
			foreach (SCR_EditableEntityComponent entity: entities)
			{
				//Print("[SCR_EntitiesToolbarEditorUIComponent] Refresh foreach loop");
				if (entity
					&& (!m_aTypeBlackList.Contains(entity.GetEntityType()))
					&& (m_FlagsBlacklist == 0 || (entity.GetEntityFlags() & m_FlagsBlacklist) == 0)
				)
				{					
					//--- Add only entities of desired type
					if (m_Type == -1 || entity.GetEntityType() == m_Type)
					{
						int order = m_aTypeOrder.Find(entity.GetEntityType());
						if (order == -1)
							order = 100 + entity.GetEntityType();
						
						foreach (SCR_EntityToolbarStateOffset stateOffset: m_aStateOffsets)
						{
							//Print("[SCR_EntitiesToolbarEditorUIComponent] Refresh foreach loop 2");
							if (entity.HasEntityState(stateOffset.m_State))
								order += stateOffset.m_iOrderOffset;
						}

                        //ONLY CUSTOM PART
                        if(!ShouldShowInSearch(entity))
							continue;
                        //END CUSTOM PART

						m_Entities.Insert(order, entity);
						m_bHasEntities = true;
					}
					else
					{
						//--- Has entities even if tabs prevent them from being shown
						m_bHasEntities |= m_bHasTabs;
					}
				}
			}
			
			if (m_Pagination)
				m_Pagination.SetEntryCount(m_Entities.Count());
		}
		
		//CUSTOM 
        bool hasContent;
		if (m_Pagination)
		{
			hasContent = m_Pagination.RefreshPage();
		}
		else
		{
			DeleteAllItems();
			ShowEntries(m_ItemsWidget, 0, int.MAX);
			hasContent = m_ItemsWidget.GetChildren() != null;
		}
		
		SetToolbarVisible(hasContent);
        // END CUSTOM
		m_queuedRefresh = false;
	}
	SCR_PlayersManagerEditorComponent m_playersManager;
	bool ShouldShowInSearch(SCR_EditableEntityComponent entity)
	{
		if(!m_playersManager)
        	m_playersManager = SCR_PlayersManagerEditorComponent.Cast(SCR_PlayersManagerEditorComponent.GetInstance(SCR_PlayersManagerEditorComponent));

		string searchInput = "";
		if(m_customSearchEditbox)
			searchInput = m_customSearchEditbox.GetValue();

		GenericEntity ownerEntity = entity.GetOwnerScripted();
		if(searchInput.IsEmpty())
			return true;
		
		if(m_playersManager && ownerEntity && SCR_ChimeraCharacter.Cast(ownerEntity))
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ownerEntity);
			if(!character) return false;
			string characterId = character.GetCharacterId();
			string characterName = character.GetCharacterName();
			int playerID = m_playersManager.GetPlayerID(entity);
			string playerName = SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(playerID);
			characterName.ToLower();
			playerName.ToLower();
			searchInput.ToLower();
			if(!playerName.Contains(searchInput) && !characterName.Contains(searchInput) && !characterId.Contains(searchInput))
				return false;
			//SCR_PlayerEditableEntityUIComponent determines the name when in the hint
		}
		if(ownerEntity && Vehicle.Cast(ownerEntity))
		{
			Vehicle vehicle = Vehicle.Cast(ownerEntity);
			if(!vehicle) return false;
			auto vehicleManagerComponent = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
			if(!vehicleManagerComponent) return false;

			string ownerName = vehicleManagerComponent.GetOwnerName();
			string ownerId = vehicleManagerComponent.GetOwnerId();
			ownerName.ToLower();
			ownerId.ToLower();

			if(!ownerName.Contains(searchInput) && !ownerId.Contains(searchInput))
				return false;
		}
		return true;
	}
    void HandleSearchInputChange() {
        Refresh();
    }
    protected SCR_EditBoxComponent m_customSearchEditbox;
    override void HandlerAttachedScripted(Widget w)
	{
        super.HandlerAttachedScripted(w);
        Widget searchWidget = w.FindAnyWidget("CustomSearchInput");
        if(!searchWidget) return;
        m_customSearchEditbox = SCR_EditBoxComponent.Cast(searchWidget.FindHandler(SCR_EditBoxComponent));
        if(!m_customSearchEditbox) return;
        m_customSearchEditbox.m_OnChanged.Insert(HandleSearchInputChange);

    }
    


}
