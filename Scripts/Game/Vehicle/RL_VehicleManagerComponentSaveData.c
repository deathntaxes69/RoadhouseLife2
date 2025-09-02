[EPF_ComponentSaveDataType(RL_VehicleManagerComponent), BaseContainerProps()]
class RL_VehicleManagerComponentSaveDataClass : EPF_ComponentSaveDataClass
{
}

[EDF_DbName.Automatic()]
class RL_VehicleManagerComponentSaveData : EPF_ComponentSaveData
{
	int m_sGarageId;
	string m_sOwnerId;
	string m_sOwnerName;
	int m_sVehiclePrice;


	//------------------------------------------------------------------------------------------------
	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		RL_VehicleManagerComponent oldComponent = RL_VehicleManagerComponent.Cast(component);

		m_sGarageId = oldComponent.m_sGarageId;
		m_sOwnerId = oldComponent.m_sOwnerId;
		m_sOwnerName = oldComponent.m_sOwnerName;
		m_sVehiclePrice = oldComponent.m_iVehiclePrice;

		return EPF_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		RL_VehicleManagerComponent oldComponent = RL_VehicleManagerComponent.Cast(component);
        
        oldComponent.SetProps(m_sGarageId, m_sOwnerId, m_sOwnerName, m_sVehiclePrice);
       
		return EPF_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EPF_ComponentSaveData other)
	{
		RL_VehicleManagerComponentSaveData otherData = RL_VehicleManagerComponentSaveData.Cast(other);
		return  (m_sGarageId == otherData.m_sGarageId) && (m_sOwnerId == otherData.m_sOwnerId) && (m_sOwnerName == otherData.m_sOwnerName) && (m_sVehiclePrice == otherData.m_sVehiclePrice);

	}
}

class EPF_PersistentVehicle
{
	int m_sGarageId;
	string m_sOwnerId;
	string m_sOwnerName;
	int m_sVehiclePrice;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EPF_PersistentVehicle other)
	{
		return (m_sGarageId == other.m_sGarageId) && (m_sOwnerId == other.m_sOwnerId) && (m_sOwnerName == other.m_sOwnerName) && (m_sVehiclePrice == other.m_sVehiclePrice);
	}
}
