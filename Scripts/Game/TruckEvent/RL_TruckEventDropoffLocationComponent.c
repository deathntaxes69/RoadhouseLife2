[ComponentEditorProps(category: "RL/TruckEvent", description: "")]
class RL_TruckEventDropoffLocationComponentClass: ScriptComponentClass {}

class RL_TruckEventDropoffLocationComponent: ScriptComponent 
{
	[RplProp()]
	protected bool m_bIsActiveDropoff = false;
	
	protected static ref array<RL_TruckEventDropoffLocationComponent> s_aAllDropoffLocations = {};
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (Replication.IsServer())
		{
			s_aAllDropoffLocations.Insert(this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		if (Replication.IsServer())
		{
			int index = s_aAllDropoffLocations.Find(this);
			if (index != -1)
				s_aAllDropoffLocations.Remove(index);
		}
		
		super.OnDelete(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool RplSave(ScriptBitWriter writer)
	{
		bool result = super.RplSave(writer);
		writer.WriteBool(m_bIsActiveDropoff);
		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected override bool RplLoad(ScriptBitReader reader)
	{
		bool result = super.RplLoad(reader);
		reader.ReadBool(m_bIsActiveDropoff);
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAsActiveDropoff(bool active)
	{
		if (!Replication.IsServer())
			return;
			
		m_bIsActiveDropoff = active;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsActiveDropoff()
	{
		return m_bIsActiveDropoff;
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetWorldPosition()
	{
		if (!GetOwner())
			return vector.Zero;
			
		return GetOwner().GetOrigin();
	}
	
	//------------------------------------------------------------------------------------------------
	static array<RL_TruckEventDropoffLocationComponent> GetAllDropoffLocations()
	{
		return s_aAllDropoffLocations;
	}
	
	//------------------------------------------------------------------------------------------------
	static RL_TruckEventDropoffLocationComponent GetActiveDropoffLocation()
	{
		foreach (RL_TruckEventDropoffLocationComponent location : s_aAllDropoffLocations)
		{
			Print("[RL_TruckEventDropOffLocationComp] GetActiveDropoffLocation foreach");
			if (location && location.IsActiveDropoff())
				return location;
		}
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	static void ClearAllActiveDropoffs()
	{
		if (!Replication.IsServer())
			return;
			
		foreach (RL_TruckEventDropoffLocationComponent location : s_aAllDropoffLocations)
		{
			Print("[RL_TruckEventDropOffLocationComp] ClearAllActiveDropoffs foreach");
			if (location)
				location.SetAsActiveDropoff(false);
		}
	}
}