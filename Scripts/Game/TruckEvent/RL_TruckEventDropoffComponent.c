[ComponentEditorProps(category: "RL/TruckEvent", description: "")]
class RL_TruckEventDropoffComponentClass: ScriptComponentClass {}

//! Structure to define loot pool entries
[BaseContainerProps()]
class RL_TruckEventLootEntry
{
	[Attribute(category: "Loot Settings", desc: "Prefab resource for the loot item")]
	ResourceName m_PrefabResource;
	
	[Attribute(category: "Loot Settings", defvalue: "50", desc: "Percent chance (0-100) for this item to appear", params: "0 100 1")]
	int m_iPercentChance;
	
	[Attribute(category: "Loot Settings", defvalue: "1", desc: "Minimum spawn count")]
	int m_iMinCount;
	
	[Attribute(category: "Loot Settings", defvalue: "3", desc: "Maximum spawn count")]
	int m_iMaxCount;
}

class RL_TruckEventDropoffComponent: ScriptComponent 
{
	[Attribute(category: "Dropoff Settings", desc: "Bounding box corner 1")]
	protected ref PointInfo m_BoundingBoxCorner1;
	
	[Attribute(category: "Dropoff Settings", desc: "Bounding box corner 2")]
	protected ref PointInfo m_BoundingBoxCorner2;
	
	[Attribute(category: "Dropoff Settings", defvalue: "900", desc: "Time in seconds to defend the truck")]
	protected int m_iDefendTimeSeconds;
	
	[Attribute(category: "Loot Settings", desc: "Loot pool for rewards")]
	protected ref array<ref RL_TruckEventLootEntry> m_aLootPool;
	
	[RplProp()]
	protected bool m_bIsActive = false;
	
	[RplProp()]
	protected float m_fDefendStartTime = 0;
	
	[RplProp()]
	protected bool m_bDefendComplete = false;
	
	[RplProp()]
	protected float m_fCurrentProgress = 0.0;
	
	[RplProp()]
	protected RplId m_TruckId;
	
	[RplProp()]
	protected bool m_bLootAvailable = false;
	
	protected ref array<ref RL_TruckEventLootEntry> m_aGeneratedLoot = {};

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (m_BoundingBoxCorner1)
			m_BoundingBoxCorner1.Init(owner);
			
		if (m_BoundingBoxCorner2)
			m_BoundingBoxCorner2.Init(owner);
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateDefendProgress()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bIsActive || m_bDefendComplete)
			return;
			
		float currentTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		float elapsedTime = (currentTime - m_fDefendStartTime) / 1000.0;
		float requiredTime = m_iDefendTimeSeconds;
		
		m_fCurrentProgress = Math.Clamp(elapsedTime / requiredTime, 0.0, 1.0);
		
		if (elapsedTime >= requiredTime)
		{
			CompleteDefend();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(UpdateDefendProgress, 10000);
		}
		
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	protected void CheckTruckBounds()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bIsActive || m_bDefendComplete)
		{
			GetGame().GetCallqueue().Remove(CheckTruckBounds);
			return;
		}
		
		if (m_TruckId != RplId.Invalid())
		{
			IEntity truck = EPF_NetworkUtils.FindEntityByRplId(m_TruckId);
			if (!truck)
			{
				CancelDefend();
				return;
			}
			
			array<IEntity> trucksInBounds = GetTrucksInBounds();
			bool truckStillInBounds = false;
			
			foreach (IEntity vehicle : trucksInBounds)
			{
				Print("[RL_TruckEventDropOffComp] CheckTruckBounds foreach");
				if (vehicle == truck)
				{
					truckStillInBounds = true;
					break;
				}
			}
			
			if (!truckStillInBounds)
			{
				CancelDefend();
				return;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool RplSave(ScriptBitWriter writer)
	{
		bool result = super.RplSave(writer);
		writer.WriteBool(m_bIsActive);
		writer.WriteFloat(m_fDefendStartTime);
		writer.WriteBool(m_bDefendComplete);
		writer.WriteFloat(m_fCurrentProgress);
		writer.WriteRplId(m_TruckId);
		writer.WriteBool(m_bLootAvailable);
		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected override bool RplLoad(ScriptBitReader reader)
	{
		bool result = super.RplLoad(reader);
		reader.ReadBool(m_bIsActive);
		reader.ReadFloat(m_fDefendStartTime);
		reader.ReadBool(m_bDefendComplete);
		reader.ReadFloat(m_fCurrentProgress);
		reader.ReadRplId(m_TruckId);
		reader.ReadBool(m_bLootAvailable);
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanStartDefend()
	{
		return !m_bIsActive && !m_bDefendComplete;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanCancelDefend()
	{
		return m_bIsActive && !m_bDefendComplete;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanTakeLoot()
	{
		return m_bDefendComplete && m_bLootAvailable;
	}
	
	//------------------------------------------------------------------------------------------------
	array<IEntity> GetTrucksInBounds()
	{
		if (!m_BoundingBoxCorner1 || !m_BoundingBoxCorner2)
			return {};
			
		vector corner1, corner2;
		vector mat[4];
		m_BoundingBoxCorner1.GetModelTransform(mat);
		corner1 = GetOwner().CoordToParent(mat[3]);
		
		m_BoundingBoxCorner2.GetModelTransform(mat);
		corner2 = GetOwner().CoordToParent(mat[3]);
		
		vector mins = Vector(
			Math.Min(corner1[0], corner2[0]),
			Math.Min(corner1[1], corner2[1]),
			Math.Min(corner1[2], corner2[2])
		);
		
		vector maxs = Vector(
			Math.Max(corner1[0], corner2[0]),
			Math.Max(corner1[1], corner2[1]),
			Math.Max(corner1[2], corner2[2])
		);
		
		m_TrucksInBounds = {};
		GetGame().GetWorld().QueryEntitiesByAABB(mins, maxs, InsertTruckInBounds, FilterTrucks);
		
		return m_TrucksInBounds;
	}
	
	//------------------------------------------------------------------------------------------------
	protected ref array<IEntity> m_TrucksInBounds = {};
	
	//------------------------------------------------------------------------------------------------
	protected bool InsertTruckInBounds(IEntity entity)
	{
		if (entity)
		{
			m_TrucksInBounds.Insert(entity);
			return true;
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool FilterTrucks(IEntity entity)
	{
		return entity && entity.IsInherited(Vehicle);
	}
	
	//------------------------------------------------------------------------------------------------
	bool StartDefend(IEntity truck)
	{
		if (!CanStartDefend() || !truck)
			return false;

		RL_TruckEventComponent truckEventComponent = RL_TruckEventComponent.Cast(truck.FindComponent(RL_TruckEventComponent));
		if (!truckEventComponent || !truckEventComponent.HasDropoffLocation())
			return false;
		
		RL_TruckEventDropoffLocationComponent currentDropoffLocation = RL_TruckEventDropoffLocationComponent.Cast(GetOwner().FindComponent(RL_TruckEventDropoffLocationComponent));
		if (!currentDropoffLocation)
			return false;
		
		if (!currentDropoffLocation.IsActiveDropoff())
			return false;
		
		vector truckAssignedDropoff = truckEventComponent.GetDropoffLocation();
		vector currentDropoffPosition = currentDropoffLocation.GetWorldPosition();
		float distance = vector.Distance(truckAssignedDropoff, currentDropoffPosition);
		if (distance > 200)
			return false;
			
		RplComponent truckRpl = RplComponent.Cast(truck.FindComponent(RplComponent));
		if (truckRpl)
		{
			m_TruckId = truckRpl.Id();
		}
		
		m_bIsActive = true;
		m_fDefendStartTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		m_fCurrentProgress = 0.0;
		
		GetGame().GetCallqueue().CallLater(UpdateDefendProgress, 10000);
		GetGame().GetCallqueue().CallLater(CheckTruckBounds, 10000, true);

		Replication.BumpMe();
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CancelDefend()
	{
		if (!CanCancelDefend())
			return false;
			
		GetGame().GetCallqueue().Remove(CheckTruckBounds);
		GetGame().GetCallqueue().Remove(UpdateDefendProgress);
		m_bIsActive = false;
		m_bDefendComplete = false;
		m_fDefendStartTime = 0;
		m_fCurrentProgress = 0.0;
		m_TruckId = RplId.Invalid();
		m_bLootAvailable = false;
		m_aGeneratedLoot.Clear();
		
		Replication.BumpMe();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CompleteDefend()
	{
		GetGame().GetCallqueue().Remove(CheckTruckBounds);
		
		m_bIsActive = false;
		m_bDefendComplete = true;
		m_fCurrentProgress = 1.0;
		
		IEntity truck = EPF_NetworkUtils.FindEntityByRplId(m_TruckId);
		if (truck)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(truck);
		}
		
		GenerateLoot();
		m_bLootAvailable = true;
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void GenerateLoot()
	{
		m_aGeneratedLoot.Clear();
		
		if (!m_aLootPool || m_aLootPool.Count() == 0)
			return;
		
		foreach (RL_TruckEventLootEntry entry : m_aLootPool)
		{
			Print("[RL_TruckEventDropOffComp] GenerateLoot foreach");
			if (!entry || !entry.m_PrefabResource)
				continue;
			
			int roll = Math.RandomInt(1, 100);
			
			if (roll <= entry.m_iPercentChance)
			{
				RL_TruckEventLootEntry generatedEntry = new RL_TruckEventLootEntry();
				generatedEntry.m_PrefabResource = entry.m_PrefabResource;
				generatedEntry.m_iPercentChance = entry.m_iPercentChance;
				generatedEntry.m_iMinCount = Math.RandomInt(entry.m_iMinCount, entry.m_iMaxCount + 1);
				generatedEntry.m_iMaxCount = generatedEntry.m_iMinCount;
				m_aGeneratedLoot.Insert(generatedEntry);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool TakeLoot(IEntity player)
	{
		if (!CanTakeLoot() || !player)
			return false;
		
		InventoryStorageManagerComponent playerInventory = RL_InventoryUtils.GetResponsibleStorageManager(player);
		if (!playerInventory)
			return false;
		
		foreach (RL_TruckEventLootEntry lootEntry : m_aGeneratedLoot)
		{
			Print("[RL_TruckEventDropOffComp] TakeLoot foreach");
			if (!lootEntry || !lootEntry.m_PrefabResource)
				continue;
			
			int amountToAdd = lootEntry.m_iMinCount;
			RL_InventoryUtils.AddAmount(playerInventory, lootEntry.m_PrefabResource, amountToAdd, true);
		}
		
		m_bIsActive = false;
		m_bDefendComplete = false;
		m_fDefendStartTime = 0;
		m_fCurrentProgress = 0.0;
		m_TruckId = RplId.Invalid();
		m_bLootAvailable = false;
		m_aGeneratedLoot.Clear();
		
		Replication.BumpMe();
		return true;
	}
	
	
	//------------------------------------------------------------------------------------------------
	float GetDefendProgress()
	{
		return m_fCurrentProgress;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsDropoffActive() 
	{ 
		return m_bIsActive; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsDefendComplete() 
	{ 
		return m_bDefendComplete; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsLootAvailable() 
	{ 
		return m_bLootAvailable; 
	}
	
	//------------------------------------------------------------------------------------------------
	int GetDefendTimeSeconds() 
	{ 
		return m_iDefendTimeSeconds; 
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref RL_TruckEventLootEntry> GetGeneratedLoot() 
	{ 
		return m_aGeneratedLoot; 
	}
}