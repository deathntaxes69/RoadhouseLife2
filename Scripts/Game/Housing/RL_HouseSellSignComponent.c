[EntityEditorProps(category: "GameScripted/Housing", description: "")]
class RL_HouseSellSignComponentClass : ScriptComponentClass
{
}

class RL_HouseSellSignComponent : ScriptComponent
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
		
		IEntity signEntity = GetOwner();
		if (!signEntity)
			return;
		
		IEntity parent = signEntity.GetParent();
		if (!parent)
			return;

		RL_HouseComponent houseComponent = RL_HouseComponent.Cast(parent.FindComponent(RL_HouseComponent));
		if (!houseComponent)
			return;
		
		RplId thisRplId = EPF_NetworkUtils.GetRplId(signEntity);
		if (thisRplId != Replication.INVALID_ID)
		{
			houseComponent.RegisterSellingSign(thisRplId);
			m_ParentHouseComponent = houseComponent;
			m_bRegistered = true;
			UpdateVisibility(houseComponent.IsOwned());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateVisibility(bool houseIsOwned)
	{
		IEntity signEntity = GetOwner();
		if (!signEntity)
			return;
		if (houseIsOwned)
			signEntity.ClearFlags(EntityFlags.VISIBLE, false);
		else
			signEntity.SetFlags(EntityFlags.VISIBLE, false);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		if (m_bRegistered && m_ParentHouseComponent)
		{
			RplId thisRplId = EPF_NetworkUtils.GetRplId(owner);
			if (thisRplId != Replication.INVALID_ID)
			{
				m_ParentHouseComponent.UnregisterSellingSign(thisRplId);
			}
		}
		super.OnDelete(owner);
	}
}