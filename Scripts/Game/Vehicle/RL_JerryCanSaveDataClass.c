[EPF_ComponentSaveDataType(SCR_FuelManagerComponent), BaseContainerProps()]
class RL_JerryCanSaveDataClass : EPF_ComponentSaveDataClass {}

[EDF_DbName.Automatic()]
class RL_JerryCanSaveData : EPF_ComponentSaveData
{
	float m_fCurrentFuel;

	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		SCR_FuelManagerComponent fuelComp = SCR_FuelManagerComponent.Cast(component);
		if (!fuelComp)
			return EPF_EReadResult.ERROR;

		float maxFuel, percentage;
		fuelComp.GetTotalValuesOfFuelNodes(m_fCurrentFuel, maxFuel, percentage);
		return EPF_EReadResult.OK;
	}

	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		SCR_FuelManagerComponent fuelComp = SCR_FuelManagerComponent.Cast(component);
		if (!fuelComp)
			return EPF_EApplyResult.ERROR;

		float total, maxFuel, dummy;
		fuelComp.GetTotalValuesOfFuelNodes(total, maxFuel, dummy);
		if (maxFuel <= 0)
			return EPF_EApplyResult.ERROR;

		float newPercentage = m_fCurrentFuel / maxFuel;
		fuelComp.SetTotalFuelPercentage(newPercentage);
		return EPF_EApplyResult.OK;
	}

	override bool Equals(notnull EPF_ComponentSaveData other)
	{
		RL_JerryCanSaveData otherData = RL_JerryCanSaveData.Cast(other);
		return m_fCurrentFuel == otherData.m_fCurrentFuel;
	}
}
