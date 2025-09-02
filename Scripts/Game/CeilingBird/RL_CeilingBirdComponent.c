[EntityEditorProps(category: "GameScripted/SmokeDetector", description: "Ceiling Bird")]
class RL_CeilingBirdComponentClass : ScriptComponentClass
{
	[Attribute("{B8EDD06CD239CDC6}Prefabs/Items/9vBattery.et", UIWidgets.ResourceNamePicker, "Required Battery Item", params: "et")] // todo update this with 9v
	ResourceName m_BatteryItemPrefab;

	[Attribute("5", UIWidgets.Slider, "Flash Time Random Min (Seconds)", "1 60 1")]
	int m_FlashTimeMin;

	[Attribute("8", UIWidgets.Slider, "Flash Time Random Max (Seconds)", "1 120 1")]
	int m_FlashTimeMax;

	[Attribute("300", UIWidgets.Slider, "Flash Duration (ms)", "25 1000 25")]
	int m_FlashTimeDuration;

	[Attribute("25", UIWidgets.Slider, "Flash Emissive Value", "1 200 1")]
	int m_FlashEmissiveValue;

	[Attribute("15", UIWidgets.Slider, "Chirp Time Random Min (Seconds)", "1 120 1")]
	int m_ChirpTimeMin;

	[Attribute("20", UIWidgets.Slider, "Chirp Time Random Max (Seconds)", "1 300 1")]
	int m_ChirpTimeMax;

	[Attribute("5", UIWidgets.Slider, "Chirp Random Chance (Higher is more often)", "1 10 1")]
	int m_ChirpChance;

	[Attribute("10", UIWidgets.Slider, "Chirp Distance (Meters)", "1 50 1")]
	int m_ChirpDistance;

}

class RL_CeilingBirdComponent : ScriptComponent
{
	protected RL_CeilingBirdComponentClass settings;
	protected ParametricMaterialInstanceComponent m_Material;

	override void OnPostInit(IEntity owner)
	{
		settings = RL_CeilingBirdComponentClass.Cast(GetComponentData(GetOwner()));

		if (!Replication.IsServer()) {
			m_Material = ParametricMaterialInstanceComponent.Cast(owner.FindComponent(ParametricMaterialInstanceComponent));
			m_Material.SetEmissiveMultiplier(0);

			GetGame().GetCallqueue().CallLater(SmokeDetectorBlink, Math.RandomInt(settings.m_FlashTimeMin, settings.m_FlashTimeMax) * 1000, true, owner);

			return;
		}

		GetGame().GetCallqueue().CallLater(SmokeDetectorLoop, Math.RandomInt(settings.m_ChirpTimeMin, settings.m_ChirpTimeMax) * 1000, true, owner);
	}

	override void OnDelete(IEntity owner)
	{
		if (!Replication.IsServer()) {
			GetGame().GetCallqueue().Remove(SmokeDetectorBlink);

			super.OnDelete(owner);
			return;
		}

		GetGame().GetCallqueue().Remove(SmokeDetectorLoop);

		super.OnDelete(owner);
	}

	protected void SmokeDetectorLoop(IEntity owner)
	{
		if (Math.RandomInt(0, 10) < settings.m_ChirpChance)
			return;

		vector mat[4];
		owner.GetTransform(mat);

		Rpc(RpcDo_SmokeDetectorSound, mat);
	}

	protected void SmokeDetectorBlink(IEntity owner)
	{
		m_Material.SetEmissiveMultiplier(settings.m_FlashEmissiveValue);
		GetGame().GetCallqueue().CallLater(m_Material.SetEmissiveMultiplier, settings.m_FlashTimeDuration, false, 0);
	}

	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	private void RpcDo_SmokeDetectorSound(vector origin[4])
	{
		if (Replication.IsServer())
			return;
		Print("RpcDo_SmokeDetectorSound");

		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		vector mat[4];
		character.GetTransform(mat);

		if (!character || vector.Distance(mat, origin) > settings.m_ChirpDistance)
			return;

		character.PlayCommonSoundLocally("SOUND_SMOKEDETECTOR", origin)
	}

}
