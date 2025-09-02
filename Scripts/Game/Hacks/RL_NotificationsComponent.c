modded class SCR_NotificationsComponent
{
    // Disable notifications when admins login to game master
    override bool SendToOwner(ENotification id, SCR_NotificationData data = null)
	{
        if(id == ENotification.EDITOR_EDITOR_RIGHTS_ASSIGNED || id == ENotification.EDITOR_ADMIN_RIGHTS_ASSIGNED || id == ENotification.EDITOR_PHOTOMODE_RIGHTS_ASSIGNED)
        {
            return true;
        }
        return super.SendToOwner(id, data);
    }
}