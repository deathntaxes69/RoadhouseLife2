[EntityEditorProps(category: "GameScripted/Racing", description: "Race checkpoint trigger for lap timing system")]
class RL_RaceCheckpointEntityClass: SCR_BaseTriggerEntityClass
{
}

class RL_RaceCheckpointEntity : SCR_BaseTriggerEntity
{
	[Attribute("1", desc: "Checkpoint number in sequence (1, 2, 3, etc.)")]
	protected int m_iCheckpointNumber;
	
	[Attribute("Default Track", desc: "Name of the track this checkpoint belongs to")]
	protected string m_sTrackName;
	
	[Attribute("3", desc: "Total number of checkpoints in this track")]
	protected int m_iTotalCheckpoints;
	
	[Attribute("0", desc: "Mark first checkpoint as finish line", uiwidget: UIWidgets.CheckBox)]
	protected bool m_bFirstAsFinish;
	
	int GetCheckpointNumber() { return m_iCheckpointNumber; }
	string GetTrackName() { return m_sTrackName; }
	int GetTotalCheckpoints() { return m_iTotalCheckpoints; }
	bool GetFirstAsFinish() { return m_bFirstAsFinish; }
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer()) 
			return;

		AddClassType(SCR_ChimeraCharacter);
		
		GetOnActivate().Insert(OnCharacterActivate);
		
		RL_LapTimerManager manager = RL_LapTimerManager.GetInstance();
		if (manager)
			manager.RegisterCheckpoint(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_RaceCheckpointEntity()
	{
		RL_LapTimerManager manager = RL_LapTimerManager.GetInstance();
		if (manager)
			manager.UnregisterCheckpoint(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnCharacterActivate(IEntity ent)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
		if (!character)
			return;
		
		// Check if character is in a vehicle
		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
		{
			return;
		}
		
		BaseCompartmentSlot compartment = compartmentAccess.GetCompartment();
		if (!compartment)
		{
			return;
		}
		
		// Get the vehicle
		IEntity vehicle = compartment.GetOwner();
		if (!vehicle)
		{
			return;
		}
		
		string characterId = character.GetCharacterId();
		if (!characterId || !compartment.IsPiloting())
			return;
		
		// Get vehicle display name
		string vehicleName = GetVehicleDisplayName(vehicle);
		
		// Pass character and vehicle info
		RL_LapTimerManager manager = RL_LapTimerManager.GetInstance();
		if (manager)
			manager.ProcessCheckpointHit(character, vehicle, vehicleName, m_sTrackName, m_iCheckpointNumber, m_iTotalCheckpoints, m_bFirstAsFinish);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetVehicleDisplayName(IEntity vehicle)
	{
		if (!vehicle)
			return "Unknown Vehicle";
		
		VehicleWheeledSimulation vehicleSimulation = VehicleWheeledSimulation.Cast(vehicle.FindComponent(VehicleWheeledSimulation));
		if (vehicleSimulation)
		{
			ResourceName prefabName = vehicle.GetPrefabData().GetPrefabName();
			string vehicleName = prefabName.GetPath();
			
			int lastSlash = vehicleName.LastIndexOf("/");
			if (lastSlash >= 0)
				vehicleName = vehicleName.Substring(lastSlash + 1, vehicleName.Length() - lastSlash - 1);
			
			int dotIndex = vehicleName.LastIndexOf(".");
			if (dotIndex >= 0)
				vehicleName = vehicleName.Substring(0, dotIndex);
			
			return vehicleName;
		}
		
		return "Vehicle";
	}
} 