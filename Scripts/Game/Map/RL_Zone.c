[EntityEditorProps(category: "GameScripted/ControlZones", description: "Zone trigger that creates map markers and shows enter/leave messages")]
class RL_ZoneEntityClass: SCR_BaseTriggerEntityClass
{
}

class RL_ZoneEntity : SCR_BaseTriggerEntity
{
	[Attribute("Zone", desc: "Name of this zone")]
	protected string m_sZoneName;
	
	[Attribute("1 1 1", desc: "Color of the zone marker", uiwidget: UIWidgets.ColorPicker)]
	protected vector m_vMarkerColor;
	
	[Attribute("", desc: "Message shown when player enters the zone")]
	protected string m_sEnterMessage;
	
	[Attribute("", desc: "Message shown when player leaves the zone")]
	protected string m_sLeaveMessage;
	
	protected RL_MapMarker m_mapMarker;
	protected IEntity m_ownerEntity;
	protected ref set<IEntity> m_entitiesInZone;
	
	string GetZoneName() { return m_sZoneName; }
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_ownerEntity = owner;
		m_entitiesInZone = new set<IEntity>();
		
		if (!GetGame().InPlayMode()) 
			return;

		CreateMapMarker();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_ZoneEntity()
	{
		if (m_mapMarker && m_mapMarker.GetOwner())
			delete m_mapMarker.GetOwner();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);

		if (!Replication.IsServer())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
			if (!character)
				return;

			if (m_entitiesInZone.Contains(ent))
				return;

			m_entitiesInZone.Insert(ent);

			if (!m_sEnterMessage.IsEmpty() && ent == GetGame().GetPlayerController().GetControlledEntity())
			{
				RL_Utils.Notify(m_sEnterMessage, m_sZoneName);
			}
		} else {
			//Print(string.Format("[RL_ZoneEntity] Player entered zone '%1'", m_sZoneName));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDeactivate(IEntity ent)
	{
		super.OnDeactivate(ent);
		if (!Replication.IsServer())
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
			if (!character)
				return;

			if (!m_entitiesInZone.Contains(ent))
				return;

			m_entitiesInZone.RemoveItem(ent);


			if (!m_sLeaveMessage.IsEmpty() && ent == GetGame().GetPlayerController().GetControlledEntity())
			{
				RL_Utils.Notify(m_sLeaveMessage, m_sZoneName);
			}
		} else {
			//Print(string.Format("[RL_ZoneEntity] Player left zone '%1'", m_sZoneName));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateMapMarker()
	{

		if (!m_ownerEntity)
			return;
			
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		m_ownerEntity.GetTransform(spawnParams.Transform);
		
		GenericEntity markerEntity = GenericEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load("{E595423ADC263333}Prefabs/World/Locations/Common/RL_MapMarker.et"), m_ownerEntity.GetWorld(), spawnParams));
		if (!markerEntity)
			return;
			
		m_mapMarker = RL_MapMarker.Cast(markerEntity.FindComponent(RL_MapMarker));
		if (!m_mapMarker)
		{
			delete markerEntity;
			return;
		}
		
		float sphereRadius = GetSphereRadius();
		Color markerColor = Color.FromVector(m_vMarkerColor);
		markerColor.SetA(0.3);
		m_mapMarker.SetMarkerProperties(m_sZoneName, "OBJECTIVE", markerColor);
		
		m_mapMarker.SetUseControlZoneLayout(true);
		m_mapMarker.SetRadius(Math.Round(sphereRadius));
		if (m_mapMarker.GetWidget())
		{
			RichTextWidget textWidget = RichTextWidget.Cast(m_mapMarker.GetWidget().FindAnyWidget("Text0"));
			if (textWidget)
			{
				textWidget.SetColor(new Color(0.0, 0.0, 0.0, 1.0));
			}
		}
		
	}
	

} 