[EntityEditorProps(category: "GameScripted/Building", description: "SPikestrip")]
class RL_SpikeStripEntityClass: SCR_BaseTriggerEntityClass
{
}

class RL_SpikeStripEntity : SCR_BaseTriggerEntity
{
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer()) 
			return;

		AddClassType(SCR_ChimeraCharacter);
		GetOnActivate().Insert(OnCharacterActivate);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnCharacterActivate(IEntity ent)
	{

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
		if (!character)
			return;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return;

		BaseCompartmentSlot compartment = compartmentAccess.GetCompartment();
		if (!compartment)
			return;
		
		IEntity vehicle = compartment.GetOwner();
		if (!vehicle)
			return;
		
		string characterId = character.GetCharacterId();
		if (!characterId || !compartment.IsPiloting())
			return;

		SetVehicleFuelToZero(vehicle);
		DamageVehicleEngine(vehicle);
	}
	
	
	//------------------------------------------------------------------------------------------------
	void SetVehicleFuelToZero(IEntity vehicle)
	{
		if (!vehicle)
			return;
			
		SCR_FuelManagerComponent fuelManager = SCR_FuelManagerComponent.Cast(vehicle.FindComponent(SCR_FuelManagerComponent));
		if (!fuelManager)
			return;
		
		fuelManager.SetTotalFuelPercentage(0);
	}
	
	//------------------------------------------------------------------------------------------------
	void DamageVehicleEngine(IEntity vehicle)
	{
		if (!vehicle)
			return;
			
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!damageManager)
			return;
		
		vector empty[3];
		empty[0] = vector.Zero;
		empty[1] = vector.Zero; 
		empty[2] = vector.Zero;
		
		SCR_DamageContext damageContext = new SCR_DamageContext(
			EDamageType.COLLISION, 
			400,
			empty, 
			vehicle, 
			damageManager.GetDefaultHitZone(), 
			Instigator.CreateInstigator(this), 
			null, 
			-1, 
			-1
		);
		damageContext.damageEffect = new SCR_CollisionDamageEffect();
		damageManager.HandleDamage(damageContext);
		
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(vehicle.FindComponent(VehicleControllerComponent));
		if (vehicleController && vehicleController.IsEngineOn())
		{
			vehicleController.ForceStopEngine();
		}
		
		SCR_CarControllerComponent carController = SCR_CarControllerComponent.Cast(vehicle.FindComponent(SCR_CarControllerComponent));
		if (carController && carController.IsEngineOn())
		{
			carController.ForceStopEngine();
		}
	}
}