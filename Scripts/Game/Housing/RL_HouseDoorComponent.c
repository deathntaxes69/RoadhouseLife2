[EntityEditorProps(category: "GameScripted/Housing", description: "Door component")]
class RL_HouseDoorComponentClass : ScriptComponentClass
{
}

class RL_HouseDoorComponent : ScriptComponent
{
	protected RL_HouseComponent m_ParentHouseComponent;
	protected bool m_bRegistered = false;
	protected DoorComponent m_DoorComponent;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
			
		m_DoorComponent = DoorComponent.Cast(owner.FindComponent(DoorComponent));
		GetGame().GetCallqueue().CallLater(RegisterWithParentHouse, 1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterWithParentHouse()
	{
		if (m_bRegistered)
			return;
		
		IEntity doorEntity = GetOwner();
		if (!doorEntity)
			return;
		
		IEntity parent = doorEntity.GetParent();
		if (!parent)
			return;

		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
		if (!houseComponent)
			return;
		
		RplId thisRplId = EPF_NetworkUtils.GetRplId(doorEntity);
		if (thisRplId != Replication.INVALID_ID)
		{
			houseComponent.RegisterDoor(thisRplId);
			m_ParentHouseComponent = houseComponent;
			m_bRegistered = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void CloseDoor()
	{
		if (m_DoorComponent)
		{
			m_DoorComponent.SetControlValue(0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	DoorComponent GetDoorComponent()
	{
		return m_DoorComponent;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		if (m_bRegistered && m_ParentHouseComponent)
		{
			RplId thisRplId = EPF_NetworkUtils.GetRplId(owner);
			if (thisRplId != Replication.INVALID_ID)
			{
				m_ParentHouseComponent.UnregisterDoor(thisRplId);
			}
		}
		super.OnDelete(owner);
	}
}
