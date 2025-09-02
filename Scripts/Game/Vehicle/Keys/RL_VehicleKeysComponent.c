class RL_VehicleKeysComponentClass : ScriptComponentClass {}

class RL_VehicleKeysComponent : ScriptComponent
{

    protected ref array<string> m_aOwnedVehicleKeys = {};


	void SetVehicleKeys(array<string> ownedVehicleStrings, bool replace = true)
    {
		Print("SetVehicleKeys");
		if(replace || m_aOwnedVehicleKeys.Count() == 0)
        	m_aOwnedVehicleKeys = ownedVehicleStrings;

		Rpc(RpcDo_SetVehicleKeys, m_aOwnedVehicleKeys);
    }
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_SetVehicleKeys(array<string> ownedVehicleStrings)
	{
		Print("RpcDo_SetVehicleKeys");
		m_aOwnedVehicleKeys = ownedVehicleStrings;
	}

	//------------------------------------------------------------------------------------------------
	array<string> GetOwnedVehicleKeyStrings()
	{
		if (!m_aOwnedVehicleKeys)
			m_aOwnedVehicleKeys = new array<string>();
		
		return m_aOwnedVehicleKeys;
	}

	//------------------------------------------------------------------------------------------------
	array<ref RL_VehicleKeyData> GetOwnedVehicleKeys()
	{
		array<ref RL_VehicleKeyData> vehicleKeys = {};
		
		if (!m_aOwnedVehicleKeys)
			return vehicleKeys;
			
		foreach (string keyString : m_aOwnedVehicleKeys)
		{
			Print("[RL_VehicleKeysCharacter] GetOwnedVehicleKeys foreach loop");
			RL_VehicleKeyData vehicleKey = RL_VehicleKeyData.FromReplicationString(keyString);
			if (vehicleKey)
				vehicleKeys.Insert(vehicleKey);
		}
		
		return vehicleKeys;
	}

	//------------------------------------------------------------------------------------------------
	bool HasVehicleKey(string vehiclePrefab)
	{
		if (!m_aOwnedVehicleKeys)
			return false;

		foreach (string keyString : m_aOwnedVehicleKeys)
		{
			Print("[RL_VehicleKeysCharacter] HasVehicleKey foreach loop");
			RL_VehicleKeyData vehicleKey = RL_VehicleKeyData.FromReplicationString(keyString);
			if (vehicleKey && vehicleKey.GetVehicleName() == vehiclePrefab)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	bool HasVehicleKeyById(int vehicleId)
	{
		if (!m_aOwnedVehicleKeys)
			return false;

		foreach (string keyString : m_aOwnedVehicleKeys)
		{
			RL_VehicleKeyData vehicleKey = RL_VehicleKeyData.FromReplicationString(keyString);
			if (vehicleKey && vehicleKey.GetVehicleId() == vehicleId)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	RL_VehicleKeyData GetVehicleKeyById(int vehicleId)
	{
		if (!m_aOwnedVehicleKeys)
			return null;

		foreach (string keyString : m_aOwnedVehicleKeys)
		{
			Print("[RL_VehicleKeysCharacter] GetVehicleKeyById foreach loop");
			RL_VehicleKeyData vehicleKey = RL_VehicleKeyData.FromReplicationString(keyString);
			if (vehicleKey && vehicleKey.GetVehicleId() == vehicleId)
				return vehicleKey;
		}

		return null;
	}

	//------------------------------------------------------------------------------------------------
	RL_VehicleKeyData GetVehicleKeyByName(string vehiclePrefab)
	{
		if (!m_aOwnedVehicleKeys)
			return null;

		foreach (string keyString : m_aOwnedVehicleKeys)
		{
			Print("[RL_VehicleKeysCharacter] GetVehicleKeyByName foreach loop");
			RL_VehicleKeyData vehicleKey = RL_VehicleKeyData.FromReplicationString(keyString);
			if (vehicleKey && vehicleKey.GetVehicleName() == vehiclePrefab)
				return vehicleKey;
		}

		return null;
	}

	//------------------------------------------------------------------------------------------------
	void AddVehicleKey(int vehicleId, string vehiclePrefab)
	{
		if (!m_aOwnedVehicleKeys)
			m_aOwnedVehicleKeys = new array<string>();

		if (!HasVehicleKeyById(vehicleId))
		{
			RL_VehicleKeyData newKey = RL_VehicleKeyData.Create(vehicleId, vehiclePrefab);
			m_aOwnedVehicleKeys.Insert(newKey.ToReplicationString());
		}
	}

	//------------------------------------------------------------------------------------------------
	void RemoveVehicleKeyById(int vehicleId)
	{
		if (!m_aOwnedVehicleKeys)
			return;

		for (int i = m_aOwnedVehicleKeys.Count() - 1; i >= 0; i--)
		{
			Print("[RL_VehicleKeysCharacter] RemoveVehicleKeyById for loop");
			RL_VehicleKeyData vehicleKey = RL_VehicleKeyData.FromReplicationString(m_aOwnedVehicleKeys[i]);
			if (vehicleKey && vehicleKey.GetVehicleId() == vehicleId)
			{
				m_aOwnedVehicleKeys.Remove(i);
				break;
			}
		}
	}
	//------------------------------------------------------------------------------------------------
	int GetVehicleKeyCount()
	{
		if (!m_aOwnedVehicleKeys)
			return 0;
		
		return m_aOwnedVehicleKeys.Count();
	}
	//------------------------------------------------------------------------------------------------
	void GiveVehicleKeyToCharacter(string targetCharacterId, int vehicleId)
	{
		RL_VehicleKeyData vehicleKey = GetVehicleKeyById(vehicleId);
		if (!vehicleKey)
		{
			RL_Utils.Notify("Problem finding key", "VEHICLE KEYS");
			return;
		}
		SCR_ChimeraCharacter localCharacter = RL_Utils.GetLocalCharacter();
		Rpc(RpcAsk_GiveVehicleKey, localCharacter.GetCharacterId(), targetCharacterId, vehicleId, vehicleKey.GetVehicleName());
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_GiveVehicleKey(string senderCharacterId, string targetCharacterId, int vehicleId, string vehiclePrefab)
	{
		if (!Replication.IsServer())
			return;

		Print("RpcAsk_GiveVehicleKey");

		SCR_ChimeraCharacter targetCharacter = RL_Utils.FindCharacterById(targetCharacterId);
		if (!targetCharacter)
		{
			SCR_ChimeraCharacter senderCharacter = RL_Utils.FindCharacterById(senderCharacterId);
			if (senderCharacter)
				senderCharacter.Notify("Target character not found", "VEHICLE KEYS");
			return;
		}

		SCR_ChimeraCharacter senderCharacter = RL_Utils.FindCharacterById(senderCharacterId);
		if (!senderCharacter)
			return;

        auto keysComp = RL_VehicleKeysComponent.Cast(targetCharacter.FindComponent(RL_VehicleKeysComponent));
        if(!keysComp) return;
		keysComp.ReceiveVehicleKey(vehicleId, vehiclePrefab);
		senderCharacter.Notify(string.Format("Gave key to %1", targetCharacter.GetCharacterName()), "VEHICLE KEYS");
	}

	//------------------------------------------------------------------------------------------------
	void ReceiveVehicleKey(int vehicleId, string vehiclePrefab)
	{
		AddVehicleKey(vehicleId, vehiclePrefab);
		Rpc(RpcDo_ReceiveVehicleKey, vehicleId, vehiclePrefab);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ReceiveVehicleKey(int vehicleId, string vehiclePrefab)
	{
		Print("RpcDo_ReceiveVehicleKey");

		AddVehicleKey(vehicleId, vehiclePrefab);
		string vehicleName = RL_ShopUtils.GetEntityDisplayName(vehiclePrefab);
		RL_Utils.Notify(string.Format("Received key to %1", vehicleName), "VEHICLE KEYS");
	}

}