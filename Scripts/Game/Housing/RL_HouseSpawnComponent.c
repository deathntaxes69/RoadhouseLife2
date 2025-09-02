[EntityEditorProps(category: "GameScripted/Housing", description: "")]
class RL_HouseSpawnComponentClass : ScriptComponentClass
{
}

class RL_HouseSpawnComponent : ScriptComponent
{
	protected RL_HouseComponent m_ParentHouseComponent;
	protected bool m_bRegistered = false;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		GetGame().GetCallqueue().CallLater(RegisterWithParentHouse, 1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterWithParentHouse()
	{
		if (m_bRegistered)
			return;
		
		IEntity spawnEntity = GetOwner();
		if (!spawnEntity)
			return;
		
		IEntity parent = spawnEntity.GetParent();
		if (!parent)
			return;

		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
		if (!houseComponent)
			return;
		
		RplId thisRplId = EPF_NetworkUtils.GetRplId(spawnEntity);
		if (thisRplId != Replication.INVALID_ID)
		{
			houseComponent.RegisterSpawnPoint(thisRplId);
			m_ParentHouseComponent = houseComponent;
			m_bRegistered = true;
			UpdateSpawnRestriction(houseComponent.IsOwned(), houseComponent.GetOwnerCid(), houseComponent.HasUpgradeType(HouseUpgrades.PLAYER_SPAWN));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateSpawnRestriction(bool houseIsOwned, int ownerCid, bool hasPlayerSpawnUpgrade)
	{
		IEntity spawnEntity = GetOwner();
		if (!spawnEntity)
			return;
			
		RL_SpawnPoint spawnPoint = RL_SpawnPoint.Cast(spawnEntity);
		if (!spawnPoint)
			return;
		
		if (hasPlayerSpawnUpgrade && houseIsOwned)
		{
			spawnPoint.SetCidRestrict(ownerCid);
		}
		else
		{
			spawnPoint.SetCidRestrict(-1);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		if (m_bRegistered && m_ParentHouseComponent)
		{
			RplId thisRplId = EPF_NetworkUtils.GetRplId(owner);
			if (thisRplId != Replication.INVALID_ID)
			{
				m_ParentHouseComponent.UnregisterSpawnPoint(thisRplId);
			}
		}
		super.OnDelete(owner);
	}
}