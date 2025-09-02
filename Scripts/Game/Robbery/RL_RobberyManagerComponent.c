class RL_RobberyManagerComponentClass: ScriptComponentClass
{
    [Attribute(defvalue:"Gas Station", desc: "Robbery Location Name", category: "General")]
    string m_sRobberyName;
};
class RL_RobberyManagerComponent: ScriptComponent
{

    [RplProp(), Attribute(defvalue:"2400")]
	protected int m_fRobberyBreakTime;

    [Attribute(defvalue:"600000", desc: "Marker Display Time in milliseconds", category: "General")]
    protected float m_fMarkerDisplayTime;

    [RplProp()]
    protected bool m_bRobberyActive = false;

    [RplProp()]
    protected int m_fRobberyStartTime = 0;
    
    [RplProp()]
    protected int m_fNextRobberyTime = 0;
    
    protected IEntity m_robberyMarker;
	
    bool IsRobberyActive()
    {
        return m_bRobberyActive;
    }
    bool CanRobNow()
	{
        int time = System.GetUnixTime();
        return !m_bRobberyActive && time > m_fNextRobberyTime;
    
	}
    void SetRobberyState(bool state)
	{
        m_bRobberyActive = state;
        if(state)
        {
            m_fNextRobberyTime = System.GetUnixTime()+m_fRobberyBreakTime;
            Replication.BumpMe();
            AlertAllPolice();
            
        } else {
            m_fRobberyStartTime = 0;
            Replication.BumpMe();
        }
	}
    void AlertAllPolice()
    {
        if (!Replication.IsServer())
            return;
        CreateRobberyMarker();
        
        RL_RobberyManagerComponentClass settings = RL_RobberyManagerComponentClass.Cast(GetComponentData(GetOwner()));
        string robberyName = "Location";
        if (settings && !settings.m_sRobberyName.IsEmpty())
            robberyName = settings.m_sRobberyName;
        
        RL_Utils.NotifyAllPolice(string.Format("ROBBERY ALARM TRIGGERED at %1. Location marked on your map.", robberyName), "DISPATCH");
    }
    void CreateRobberyMarker()
    {
        DeleteMarker();
        
        RL_RobberyManagerComponentClass settings = RL_RobberyManagerComponentClass.Cast(GetComponentData(GetOwner()));
        string markerText = "ROBBERY IN PROGRESS";
        if (settings && !settings.m_sRobberyName.IsEmpty())
        {
            string robberyName = settings.m_sRobberyName;
            markerText = string.Format("ROBBERY IN PROGRESS AT %1", robberyName);
        }
        
        m_robberyMarker = RL_MapUtils.CreateMarkerFromPrefabServer(
            GetOwner().GetOrigin(),
            "{DF3550E63EBBE99E}Prefabs/World/Locations/Common/RL_MapMarker_Robbery.et",
            markerText,
            m_fMarkerDisplayTime
        );
    }
    
    void DeleteMarker()
    {
        if (m_robberyMarker)
        {
            RL_MapUtils.DeleteMarkerEntity(m_robberyMarker);
            m_robberyMarker = null;
        }
    }
    
    void RL_RobberyManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
        if (!GetGame().InPlayMode()) return;

    }
    void ~RL_RobberyManagerComponent() {
        DeleteMarker();
    }
	
};
