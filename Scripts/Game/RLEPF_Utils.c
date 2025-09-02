modded class EPF_Utils
{
	// Temp hack until this bug is fixed https://github.com/Arkensor/EnfusionPersistenceFramework/issues/49
    override static string GetPlayerUID(int playerId)
    {
		//Print("-------- GetPlayerUID");
        if (!Replication.IsServer())
        {
            Debug.Error("GetPlayerUID can only be used on the server and after OnPlayerAuditSuccess.");
            return string.Empty;
        }

        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
        if (!uid)
        {
            if (RplSession.Mode() != RplMode.Dedicated || !GetGame().GetBackendApi().GetStorage().GetOnlineWritePrivilege())
            {
                // Peer tool supportR
                uid = string.Format("bbbbdddd-0000-0000-0000-%1", playerId.ToString(12));
            }
            else
            {
                Debug.Error("Dedicated server is not correctly configured to connect to the BI backend.\nSee https://community.bistudio.com/wiki/Arma_Reforger:Server_Hosting");
            }
        }

        return uid;
    }
}