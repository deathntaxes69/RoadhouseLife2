[EntityEditorProps(category: "GameScripted/Apartments", description: "Apartment component for player apartments")]
class RL_ApartmentComponentClass : ScriptComponentClass
{
	[Attribute(category: "Upgrade Spawn Points", desc: "Upgrade spawn point locations")]
	ref array<ref RL_HouseUpgradeSpawnPoint> m_UpgradeSpawnPoints;
}

class RL_ApartmentComponent : ScriptComponent
{
	[RplProp(onRplName: "OnOwnerCidReplicated")]
	protected int m_iOwnerCid = -1;
	
	[RplProp()]
	protected ref array<int> m_aOwnedUpgradeTypes;
	
	protected ref array<ref RL_BaseUpgradeData> m_aUpgrades;
	protected ref array<IEntity> m_aSpawnedUpgrades;
	
	protected ref RL_ApartmentDbHelper m_apartmentDbHelper;
	protected bool m_bUpgradesLoaded = false;
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_HouseUpgradeSpawnPoint> GetUpgradeSpawnPoints()
	{
		RL_ApartmentComponentClass componentData = RL_ApartmentComponentClass.Cast(GetComponentData(GetOwner()));
		if (componentData)
			return componentData.m_UpgradeSpawnPoints;
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerCid()
	{
		return m_iOwnerCid;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetOwnerCid(int ownerCid)
	{
		m_iOwnerCid = ownerCid;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnOwnerCidReplicated()
	{
		PrintFormat("will we need this later if other people can enter apartment? %1", m_iOwnerCid)
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (GetGame().InPlayMode())
		{
			m_apartmentDbHelper = new RL_ApartmentDbHelper();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsOwned()
	{
		return m_iOwnerCid != -1;
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadAndSpawnUpgrades()
	{
		if (!IsOwned() || !m_apartmentDbHelper || m_bUpgradesLoaded)
			return;
		
		m_bUpgradesLoaded = true;
		m_apartmentDbHelper.GetApartmentByOwner(m_iOwnerCid, this, "OnApartmentDataLoaded");
	}
	
	//------------------------------------------------------------------------------------------------
	void OnApartmentDataLoaded(bool success, string response)
	{
		if (!success)
			return;
		
		RL_ApartmentGetResponse responseData = new RL_ApartmentGetResponse();
		responseData.ExpandFromRAW(response);
		
		if (!responseData.GetSuccess())
			return;

		array<ref RL_BaseUpgradeJsonData> upgradesJson = responseData.GetUpgrades();
		if (upgradesJson)
		{
			if (!m_aUpgrades)
				m_aUpgrades = new array<ref RL_BaseUpgradeData>();
			
			m_aUpgrades.Clear();
			
			foreach (RL_BaseUpgradeJsonData upgradeJson : upgradesJson)
			{
				Print("[RL_ApartmentComponent] OnApartmentDataLoaded foreach loop");
				RL_BaseUpgradeData upgrade = new RL_BaseUpgradeData();
				upgrade.SetData(
					upgradeJson.GetUpgradeType(),
					upgradeJson.GetUpgradeName(),
					upgradeJson.GetResourceNamePrefab(),
					upgradeJson.GetPurchasedAt()
				);
				m_aUpgrades.Insert(upgrade);
			}
		}
		
		RL_BaseUpgradeHelper.UpdateOwnedUpgradeTypes(m_aUpgrades, m_aOwnedUpgradeTypes);
		SpawnUpgrades();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void AddUpgrade(RL_BaseUpgradeData upgrade)
	{
		if (!upgrade)
			return;
		
		if (!m_aUpgrades)
			m_aUpgrades = new array<ref RL_BaseUpgradeData>();
		
		m_aUpgrades.Insert(upgrade);
		RL_BaseUpgradeHelper.UpdateOwnedUpgradeTypes(m_aUpgrades, m_aOwnedUpgradeTypes);
		
		if (Replication.IsServer())
		{
			SpawnUpgrades();
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SpawnUpgrades()
	{
		array<ref RL_HouseUpgradeSpawnPoint> spawnPoints = GetUpgradeSpawnPoints();
		RL_BaseUpgradeHelper.SpawnUpgrades(GetOwner(), m_aUpgrades, spawnPoints, m_aSpawnedUpgrades, "apartment");
	}
	
	//------------------------------------------------------------------------------------------------
	array<int> GetOwnedUpgradeTypes()
	{
		return m_aOwnedUpgradeTypes;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasUpgradeType(HouseUpgrades upgradeType)
	{
		return RL_BaseUpgradeHelper.HasUpgradeType(m_aOwnedUpgradeTypes, upgradeType);
	}
	
	//------------------------------------------------------------------------------------------------
	void RL_ApartmentComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		if (!GetGame().InPlayMode()) 
			return;
		
		if (!Replication.IsServer())
			return;
		
		if (!m_aOwnedUpgradeTypes)
			m_aOwnedUpgradeTypes = new array<int>();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_ApartmentComponent()
	{
	}
} 