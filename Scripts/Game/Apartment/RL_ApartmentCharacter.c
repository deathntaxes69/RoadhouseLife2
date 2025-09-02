modded class SCR_ChimeraCharacter
{
	
	protected vector m_vEnterLocation = vector.Zero;
	protected ref RL_ApartmentDbHelper m_apartmentDbHelper;
	protected ResourceName m_sApartmentPrefab;
	protected int m_iApartmentPrice;

	RL_ApartmentManagerComponent m_apartmentManager;
	RL_ApartmentManagerComponent GetApartmentManager()
	{
		if(m_apartmentManager)
			return m_apartmentManager;

		BaseGameMode gameMode = GetGame().GetGameMode();
		m_apartmentManager =  RL_ApartmentManagerComponent.Cast(gameMode.FindComponent(RL_ApartmentManagerComponent));
		return m_apartmentManager;
	}

	//------------------------------------------------------------------------------------------------
	void SaveEnterLocation()
	{
		m_vEnterLocation = this.GetOrigin();
	}

	//------------------------------------------------------------------------------------------------
	void TryEnterApartment(ResourceName apartmentPrefab, int apartmentPrice)
	{
		m_sApartmentPrefab = apartmentPrefab;
		m_iApartmentPrice = apartmentPrice;
		
		if (!m_apartmentDbHelper)
			m_apartmentDbHelper = new RL_ApartmentDbHelper();
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return;
		
		int playerCid = character.GetCharacterId().ToInt();
		m_apartmentDbHelper.CheckApartmentOwnership(playerCid, this, "OnCheckApartmentOwnership");
	}

	//------------------------------------------------------------------------------------------------
	void BuyApartmentUpgrade(string propertyId, int upgradeType, string upgradeName, int upgradePrice, string resourceNamePrefab)
	{
		Rpc(RpcAsk_BuyApartmentUpgrade, propertyId, upgradeType, upgradeName, upgradePrice, resourceNamePrefab);
	}

	//------------------------------------------------------------------------------------------------
	void OnCheckApartmentOwnership(bool success, string response)
	{
		
		if (!success)
			return;
		
		RL_ApartmentOwnershipResponse responseData = new RL_ApartmentOwnershipResponse();
		responseData.ExpandFromRAW(response);
		
		if (!responseData.GetSuccess())
			return;
		
		if (responseData.GetOwnsApartment())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
			if (character && character.HasApartment())
			{
				CreateApartment(character.GetApartment());
			}
			else
			{
				CreateApartment(m_sApartmentPrefab);
			}
		}
		else
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
			if (!character)
				return;
			
			int playerCid = character.GetCharacterId().ToInt();
			string apartmentPrefab = m_sApartmentPrefab;
			
			m_apartmentDbHelper.BuyApartment(playerCid, apartmentPrefab, m_iApartmentPrice, this, "OnBuyApartmentResponse");
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnBuyApartmentResponse(bool success, string response)
	{
		
		if (!success)
			return;
		
		RL_ApartmentBuyResponse responseData = new RL_ApartmentBuyResponse();
		responseData.ExpandFromRAW(response);
		
		if (responseData.GetSuccess())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
			if (character)
			{
				character.SetApartment(m_sApartmentPrefab);
			}
			CreateApartment(m_sApartmentPrefab);
		}
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyApartmentUpgrade(string propertyId, int upgradeType, string upgradeName, int upgradePrice, string resourceNamePrefab)
	{
		Print("RpcAsk_BuyApartmentUpgrade");
		if (!m_apartmentDbHelper)
			m_apartmentDbHelper = new RL_ApartmentDbHelper();
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return;
		
		int cid = character.GetCharacterId().ToInt();
		
		ref RL_ApartmentUpgradeRequestData upgradeData = new RL_ApartmentUpgradeRequestData();
		upgradeData.upgradeType = upgradeType;
		upgradeData.upgradeName = upgradeName;
		upgradeData.upgradePrice = upgradePrice;
		upgradeData.resourceNamePrefab = resourceNamePrefab;
		
		m_apartmentDbHelper.BuyApartmentUpgrade(cid, upgradeData, this, "BuyApartmentUpgradeCallback");
	}

	//------------------------------------------------------------------------------------------------
	void BuyApartmentUpgradeCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			string apartment;
			int upgradeType;
			string upgradeName;
			string resourceNamePrefab;
			
			loadContext.ReadValue("apartment", apartment);
			loadContext.ReadValue("upgradeType", upgradeType);
			loadContext.ReadValue("upgradeName", upgradeName);
			loadContext.ReadValue("resourceNamePrefab", resourceNamePrefab);
			
			RL_ApartmentManagerComponent apartmentManager = GetApartmentManager();
			if (apartmentManager)
			{
				SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
				if (!character)
					return;
				
				int cid = character.GetCharacterId().ToInt();
				map<IEntity, string> apartments = apartmentManager.m_aApartments;
				if (apartments)
				{
					foreach (IEntity apartmentEntity, string persistenceId : apartments)
					{
						Print("[RL_ApartmentCharacter] BuyApartmentUpgradeCallback foreach loop");
						if (apartmentEntity)
						{
							RL_ApartmentComponent apartmentComponent = RL_ApartmentComponent.Cast(apartmentEntity.FindComponent(RL_ApartmentComponent));
							if (apartmentComponent && apartmentComponent.GetOwnerCid() == cid)
							{
								RL_BaseUpgradeData upgrade = new RL_BaseUpgradeData();
								upgrade.SetData(upgradeType, upgradeName, resourceNamePrefab, "");
								apartmentComponent.AddUpgrade(upgrade);
								break;
							}
						}
					}
				}
			}
		}
		
		Rpc(RpcDo_BuyApartmentUpgradeCallback, success, results);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_BuyApartmentUpgradeCallback(bool success, string results)
	{
		if (success)
		{
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(results);
			
			int newBank;
			string apartment;
			string upgradeName;
			int price;
			int upgradeType;
			string resourceNamePrefab;
			
			loadContext.ReadValue("newBank", newBank);
			loadContext.ReadValue("apartment", apartment);
			loadContext.ReadValue("upgradeName", upgradeName);
			loadContext.ReadValue("price", price);
			loadContext.ReadValue("upgradeType", upgradeType);
			loadContext.ReadValue("resourceNamePrefab", resourceNamePrefab);
			
			string message = string.Format("Upgrade '%1' purchased for %2!\nNew bank balance: %3", 
				upgradeName, RL_Utils.FormatMoney(price), RL_Utils.FormatMoney(newBank));
			RL_Utils.Notify(message, "APARTMENT");
		}
		else
		{
			string errorMsg = "Failed to purchase upgrade";
			if (!results.IsEmpty())
			{
				SCR_JsonLoadContext errorContext = new SCR_JsonLoadContext();
				if (errorContext.ImportFromString(results))
				{
					string error;
					if (errorContext.ReadValue("error", error) && !error.IsEmpty())
						errorMsg = error;
				}
			}
			RL_Utils.Notify(errorMsg, "APARTMENT");
		}
	}

	void CreateApartment(ResourceName apartmentPrefab = "")
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return;
		
		int characterId = character.GetCharacterId().ToInt();
		Rpc(RpcAsk_CreateApartment, characterId, apartmentPrefab);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_CreateApartment(int characterId, ResourceName apartmentPrefab)
	{
		Print("RpcAsk_CreateApartment");
		SCR_ChimeraCharacter character = RL_Utils.GetCharacterByCharacterIdLocal(characterId.ToString());
		if (!character)
			return;
		
		string playerPersistenceID = EPF_PersistenceComponent.GetPersistentId(character);

		RL_ApartmentManagerComponent apartmentManager = GetApartmentManager();
		apartmentManager.CreateApartment(characterId, playerPersistenceID, apartmentPrefab);
	}
	void SpawnApartment(vector worldLocation)
	{
		Rpc(RpcDo_TeleportPlayerToApartment, worldLocation);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_TeleportPlayerToApartment(vector worldLocation)
	{

		SCR_Global.TeleportLocalPlayer(worldLocation);
		GetGame().GetCallqueue().CallLater(FallingToTheVoidCheck, 1000, false);
		GetGame().GetCallqueue().CallLater(FallingToTheVoidCheck, 3000, false);
		GetGame().GetCallqueue().CallLater(FallingToTheVoidCheck, 5000, false);
	}

	protected void FallingToTheVoidCheck()
	{
		if (m_vEnterLocation == vector.Zero)
			m_vEnterLocation = GetNewExitLocation();

		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(GetCharacterController());

		if (!controller || !controller.IsFalling())
			return;
		
		SCR_Global.TeleportLocalPlayer(m_vEnterLocation);
		Print("FallingToTheVoidCheck, forcing exit apartment", LogLevel.ERROR);
		ExitApartment();

	}
	void ExitApartment()
	{
		if (m_vEnterLocation == vector.Zero)
			m_vEnterLocation = GetNewExitLocation();

		SCR_Global.TeleportLocalPlayer(m_vEnterLocation);

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		if (!character)
			return;
		
		int characterId = character.GetCharacterId().ToInt();
		
		Rpc(RpcAsk_ExitApartment, characterId);
	}
	vector GetNewExitLocation()
	{
		Print("GetNewExitLocation");
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this);
		int characterApartmentId = character.GetApartmentLocationId();
		IEntity ApartmentExitLocation = GetGame().GetWorld().FindEntityByName("ApartmentExit" + characterApartmentId);
		Print(ApartmentExitLocation);
		return ApartmentExitLocation.GetOrigin();
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ExitApartment(int characterId)
	{
		Print("RpcAsk_ExitApartment");
		SCR_ChimeraCharacter character = RL_Utils.GetCharacterByCharacterIdLocal(characterId.ToString());
		if (!character)
			return;
		
		string persistenceId = EPF_PersistenceComponent.GetPersistentId(character);
		RL_ApartmentManagerComponent apartmentManager = GetApartmentManager();
		apartmentManager.RemoveApartment(characterId, persistenceId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_TeleportPlayerToWorld(vector worldLocation)
	{
		SCR_Global.TeleportLocalPlayer(worldLocation);
	}

}
