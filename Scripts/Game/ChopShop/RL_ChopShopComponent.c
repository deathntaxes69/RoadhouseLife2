[ComponentEditorProps(category: "RL/ChopShop", description: "Component for vehicle chop shop system")]
class RL_ChopShopComponentClass: ScriptComponentClass {}

class RL_ChopShopComponent: ScriptComponent 
{
	[Attribute(category: "Chop Shop Settings", desc: "Bounding box corner 1")]
	protected ref PointInfo m_BoundingBoxCorner1;
	
	[Attribute(category: "Chop Shop Settings", desc: "Bounding box corner 2")]
	protected ref PointInfo m_BoundingBoxCorner2;
	
	[Attribute(category: "Chop Shop Settings", desc: "List of allowed vehicle prefabs to chop")]
	protected ref array<string> m_AllowedPrefabs;
	
	[Attribute(category: "Chop Shop Settings", defvalue: "900", desc: "Time in seconds to chop a vehicle")]
	protected int m_iChopTimeSeconds;
	
	[Attribute("", UIWidgets.EditBox, "Control Zone ID")]
	int m_sControlZoneId;
	
	protected static const float m_fChopPayoutMultiplier = 0.1;
	protected static const float m_fGangFeePercentage = 0.1;
	
	[RplProp()]
	protected bool m_bIsChopping = false;
	
	[RplProp()]
	protected float m_fChopStartTime = 0;
	
	[RplProp()]
	protected int m_iRewardAmount = 0;
	
	[RplProp()]
	protected bool m_bChopComplete = false;
	
	[RplProp()]
	protected float m_fCurrentProgress = 0.0;
	
	[RplProp()]
	protected RplId m_ChoppedVehicleId;

	protected ref RL_GarageDbHelper m_garageHelper;

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
	protected void UpdateChopProgress()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bIsChopping || m_bChopComplete)
			return;
			
		float currentTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		float elapsedTime = (currentTime - m_fChopStartTime) / 1000.0;
		float requiredTime = m_iChopTimeSeconds;
		
		m_fCurrentProgress = Math.Clamp(elapsedTime / requiredTime, 0.0, 1.0);
		
		if (elapsedTime >= requiredTime)
		{
			CompleteChop();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(UpdateChopProgress, 10000);
		}
		
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	protected void CheckVehicleBounds()
	{
		if (!Replication.IsServer())
			return;
		Print("[RL_ChopShopComponent] CheckVehicleBounds");
			
		if (!m_bIsChopping || m_bChopComplete)
		{
			GetGame().GetCallqueue().Remove(CheckVehicleBounds);
			return;
		}
		
		if (m_ChoppedVehicleId != RplId.Invalid())
		{
			IEntity choppedVehicle = EPF_NetworkUtils.FindEntityByRplId(m_ChoppedVehicleId);
			if (!choppedVehicle)
			{
				CancelChop();
				return;
			}
			
			array<IEntity> vehiclesInBounds = GetVehiclesInBounds();
			bool vehicleStillInBounds = false;
			
			foreach (IEntity vehicle : vehiclesInBounds)
			{
				Print("[RL_ChopShopComponent] CheckVehicleBounds foreach loop");
				if (vehicle == choppedVehicle)
				{
					vehicleStillInBounds = true;
					break;
				}
			}
			
			if (!vehicleStillInBounds)
			{
				CancelChop();
				return;
			}
		}
	}
	
		//------------------------------------------------------------------------------------------------
	protected override bool RplSave(ScriptBitWriter writer)
	{
		bool result = super.RplSave(writer);
		writer.WriteBool(m_bIsChopping);
		writer.WriteFloat(m_fChopStartTime);
		writer.WriteInt(m_iRewardAmount);
		writer.WriteBool(m_bChopComplete);
		writer.WriteFloat(m_fCurrentProgress);
		writer.WriteRplId(m_ChoppedVehicleId);
		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected override bool RplLoad(ScriptBitReader reader)
	{
		bool result = super.RplLoad(reader);
		reader.ReadBool(m_bIsChopping);
		reader.ReadFloat(m_fChopStartTime);
		reader.ReadInt(m_iRewardAmount);
		reader.ReadBool(m_bChopComplete);
		reader.ReadFloat(m_fCurrentProgress);
		reader.ReadRplId(m_ChoppedVehicleId);
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanStartChop()
	{
		return !m_bIsChopping && !m_bChopComplete;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanCancelChop()
	{
		return m_bIsChopping && !m_bChopComplete;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanTakeReward()
	{
		return m_bChopComplete && m_iRewardAmount > 0;
	}
	
	//------------------------------------------------------------------------------------------------
	array<IEntity> GetVehiclesInBounds()
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
		
		m_VehiclesInBounds = {};
		GetGame().GetWorld().QueryEntitiesByAABB(mins, maxs, InsertVehicleInBounds, FilterVehicles);
		
		return m_VehiclesInBounds;
	}
	
	//------------------------------------------------------------------------------------------------
	protected ref array<IEntity> m_VehiclesInBounds = {};
	
	//------------------------------------------------------------------------------------------------
	protected bool InsertVehicleInBounds(IEntity entity)
	{
		if (entity)
		{
			m_VehiclesInBounds.Insert(entity);
			return true;
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool FilterVehicles(IEntity entity)
	{
		return entity && entity.IsInherited(Vehicle);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsVehicleAllowed(IEntity vehicle)
	{
		if (!vehicle)
			return false;
			
		EntityPrefabData prefabData = vehicle.GetPrefabData();
		if (!prefabData)
			return false;
			
		string vehiclePrefab = prefabData.GetPrefabName();
		
		foreach (string allowedPrefab : m_AllowedPrefabs)
		{
			Print("[RL_ChopShopComponent] IsVehicleAllowed foreach loop");
			if (vehiclePrefab.Contains(allowedPrefab))
				return true;
		}
		
		return false;
	}

	
	//------------------------------------------------------------------------------------------------
	void StartChopWithPrice(IEntity vehicle, int vehicleValue)
	{
		RplComponent vehicleRpl = RplComponent.Cast(vehicle.FindComponent(RplComponent));
		if (vehicleRpl)
		{
			m_ChoppedVehicleId = vehicleRpl.Id();
		}
		
		m_bIsChopping = true;
		m_fChopStartTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		m_fCurrentProgress = 0.0;
		m_iRewardAmount = (int)Math.Min(30000, Math.Round(vehicleValue * m_fChopPayoutMultiplier));

		GetGame().GetCallqueue().CallLater(UpdateChopProgress, 10000);
		GetGame().GetCallqueue().CallLater(CheckVehicleBounds, 10000, true);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool StartChop(IEntity vehicle)
	{
		if (!CanStartChop() || !vehicle || !IsVehicleAllowed(vehicle))
			return false;
			
		RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(vehicle.FindComponent(RL_VehicleManagerComponent));
		if (!vehicleManagerComponent)
			return false;
			
		int vehiclePrice = vehicleManagerComponent.GetVehiclePrice();
		if (vehiclePrice <= 0)
			vehiclePrice = 5000;
			
		RollPoliceDetection(GetOwner().GetOrigin());
		StartChopWithPrice(vehicle, vehiclePrice);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CancelChop()
	{
		if (!CanCancelChop())
			return false;
			
		GetGame().GetCallqueue().Remove(CheckVehicleBounds);
		GetGame().GetCallqueue().Remove(UpdateChopProgress);
		m_bIsChopping = false;
		m_bChopComplete = false;
		m_fChopStartTime = 0;
		m_fCurrentProgress = 0.0;
		m_iRewardAmount = 0;
		m_ChoppedVehicleId = RplId.Invalid();
		
		Replication.BumpMe();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CompleteChop()
	{
		GetGame().GetCallqueue().Remove(CheckVehicleBounds);
		
		m_bIsChopping = false;
		m_bChopComplete = true;
		m_fCurrentProgress = 1.0;
		
		int garageId = 0;
		IEntity choppedVehicle = EPF_NetworkUtils.FindEntityByRplId(m_ChoppedVehicleId);
		if (choppedVehicle)
		{
			RL_VehicleManagerComponent vehicleManagerComponent = RL_VehicleManagerComponent.Cast(choppedVehicle.FindComponent(RL_VehicleManagerComponent));
			if (vehicleManagerComponent)
			{
				garageId = vehicleManagerComponent.GetGarageId();
			}
			SCR_EntityHelper.DeleteEntityAndChildren(choppedVehicle);
		}
		
		if (garageId > 0)
		{
			GetGame().GetCallqueue().CallLater(UpdateGarageState, 500, false, garageId, 3);
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int TakeReward(SCR_ChimeraCharacter character = null)
	{
		if (!CanTakeReward())
			return 0;
			
		int reward = m_iRewardAmount;
		if (character)
		{
			RL_ControlZoneEntity controlZone = GetControlZone();
			if (controlZone)
			{
				int controllingGangId = controlZone.GetControllingGangId();
				if (controllingGangId != -1)
				{
					int gangFeeAmount = (int)Math.Round(reward * m_fGangFeePercentage);
					if (gangFeeAmount > 0)
					{
						reward -= gangFeeAmount;
						PayGangFee(character, controllingGangId, gangFeeAmount);
					}
				}
			}
		}
		
		m_bIsChopping = false;
		m_bChopComplete = false;
		m_fChopStartTime = 0;
		m_fCurrentProgress = 0.0;
		m_iRewardAmount = 0;
		m_ChoppedVehicleId = RplId.Invalid();
		
		Replication.BumpMe();
		return reward;
	}
	
	//-----------------------------------------------------------------------------------------------
	void RollPoliceDetection(vector position)
	{
		int randomChance = Math.RandomInt(1, 4);
		if (randomChance == 1)
		{
			string messageTitle = "SUSPICIOUS ACTIVITY REPORTED";
			string message = "An informant has reported a stolen car being chopshopped.";
			string markerMessage = "Stolen Car Being Chopped";
			RL_Utils.NotifyAllPolice(message, messageTitle);
			RL_MapUtils.CreateMarkerFromPrefabServer(position, "{CFDEC4E7A4407DCF}Prefabs/World/Locations/Common/RL_MapMarker_PoliceEmergency.et", markerMessage, 1800000);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	float GetChopProgress()
	{
		return m_fCurrentProgress;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetStatusText()
	{
		if (m_bChopComplete)
		{
			return string.Format("Ready - %1 reward available", RL_Utils.FormatMoney(m_iRewardAmount));
		}
		
		if (m_bIsChopping)
		{
			float progress = GetChopProgress() * 100;
			return string.Format("Chopping... %1%", Math.Round(progress));
		}
		
		return "Ready to chop vehicle";
	}
	


	//------------------------------------------------------------------------------------------------
	bool IsChopping() { return m_bIsChopping; }
	bool IsChopComplete() { return m_bChopComplete; }
	int GetRewardAmount() { return m_iRewardAmount; }
	int GetChopTimeSeconds() { return m_iChopTimeSeconds; }
	float GetPayoutMultiplier() { return m_fChopPayoutMultiplier; }
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateGarageState(int garageId, int active)
	{
		if (!m_garageHelper) m_garageHelper = new RL_GarageDbHelper();
		m_garageHelper.UpdateGarageState(garageId, active);
	}
	
	//------------------------------------------------------------------------------------------------
	protected RL_ControlZoneEntity GetControlZone()
	{
		if (m_sControlZoneId <= 0)
			return null;
			
		RL_ControlZoneManager controlZoneManager = RL_ControlZoneManager.GetInstance();
		if (!controlZoneManager)
			return null;
		
		RL_ControlZoneEntity controlZone = controlZoneManager.FindControlZoneById(m_sControlZoneId);
		return controlZone;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PayGangFee(SCR_ChimeraCharacter character, int gangId, int feeAmount)
	{
		if (!character)
			return;
		RL_GangDbHelper gangHelper = new RL_GangDbHelper();
		gangHelper.GangTransaction(character.GetCharacterId(), gangId.ToString(), 0, 0, feeAmount);
	}
}
