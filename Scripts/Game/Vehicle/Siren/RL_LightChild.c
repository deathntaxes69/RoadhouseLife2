
// This controls the actual lights that spawn on the light entity
[BaseContainerProps()]
class RL_LightChild
{
	[Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(LightType), desc: "Point is light that shines to all directions. Spot shines to a specific direction.")]
	protected LightType m_eLightType;	
	
	[Attribute("0 0 0", UIWidgets.EditBox, "Light offset in local space from the pivot point")]
	protected vector m_vLightOffset;
	
	[Attribute("1 1 1", UIWidgets.ColorPicker, "Color of the light")]
	protected vector m_vLightColor;
	
	[Attribute("0 0 0", UIWidgets.EditBox, "Direction of the light cone (useful for LightType.SPOT only).")]
	protected vector m_vLightConeDirection;
	
	[Attribute("130", UIWidgets.EditBox, "Angle of the light cone (useful for LightType.SPOT only).")]
	protected float m_fConeAngle;
	
	[Attribute("30", UIWidgets.EditBox, "Radius of the light effect")]
	protected float m_fRadius;
	
	[Attribute(desc: "Parent bone")]
	protected string m_PivotPoint;
	
	[Attribute("0.01", desc: "Near plane of the light", params: "0.01 2")]
	protected float m_NearPlane;
	
	[Attribute("0.5", desc: "Higher value means less brightness at night", params: "0 3")]
	protected float m_NightLVAttenuation;
	
	[Attribute("15", desc: "Light intensity during night time", params: "0 20")]
	protected float m_DayLV;
	
	protected LightEntity m_LightEntity;
	
	protected ref Resource m_LightResource;
	
	protected float m_currentLV;
	
	protected float m_Sunrise, m_Sunset, m_MidnightOffset, m_NightDuration;
	
	LightEntity GetLight()
	{
		return m_LightEntity;
	}

	//m_LightEntity.SetColor(Color.FromVector(color), m_currentLV);

	void Spawn(IEntity parent)
	{
		//m_currentLV = m_DayLV / (1 + Math.Pow(GetGame().GetWorld().GetCameraHDRBrightness(0), 0.5) * m_NightLVAttenuation);
		m_currentLV = 5;

		LightFlags flags;
		//LightFlags flags = LightFlags.DYNAMIC;

		vector color = m_vLightColor;
		color.Normalize();

		m_LightEntity = LightEntity.CreateLight(m_eLightType, 
												flags, 
												m_fRadius, 
												Color.FromVector(color), 
												m_currentLV,
												m_vLightOffset,
												m_vLightConeDirection);

		//m_LightEntity.SetConeAngle(m_fConeAngle);
		m_LightEntity.SetNearPlane(m_NearPlane);


		m_LightEntity.SetLensFlareType(LightLensFlareType.Disabled);
		parent.AddChild(m_LightEntity, parent.GetBoneIndex(m_PivotPoint));
	}
	
	//------------------------------------------------------------------------------------------------
	void Destroy()
	{
		delete m_LightEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_LightChild()
	{
		Destroy();
	}
}