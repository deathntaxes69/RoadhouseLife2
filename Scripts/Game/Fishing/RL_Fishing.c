[EntityEditorProps()]
class RL_FishingGadgetComponentClass: SCR_GadgetComponentClass {}

class RL_FishingGadgetComponent: SCR_GadgetComponent {
	[Attribute(defvalue: "0.7", desc: "Chance to spawn Prefab 1", params: "0.0 1.0")]
	private float CarpChance;

	[Attribute(defvalue: "0.1", desc: "Chance to spawn Prefab 2", params: "0.0 1.0")]
	private float TunaChance;

	[Attribute(defvalue: "0.1", desc: "Chance to spawn Prefab 3", params: "0.0 1.0")]
	private float ShinyChance;

	[Attribute(defvalue: "0.05", desc: "Chance to spawn Prefab 4", params: "0.0 1.0")]
	private float BootsChance;

	[Attribute(defvalue: "0.05", desc: "Chance to spawn Prefab 5", params: "0.0 1.0")]
	private float ShellChance;

	[Attribute(defvalue: "0.5", desc: "Rare Prefab 1 Chance", params: "0.0 1.0")]
	private float rarePrefab1Chance;

	[Attribute(defvalue: "0.3", desc: "Rare Prefab 2 Chance", params: "0.0 1.0")]
	private float rarePrefab2Chance;

	[Attribute(defvalue: "0.2", desc: "Rare Prefab 3 Chance", params: "0.0 1.0")]
	private float rarePrefab3Chance;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab 1 Resource")]
	private ResourceName Carp;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab 2 Resource")]
	private ResourceName Tuna;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab 3 Resource")]
	private ResourceName Shiny;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab 4 Resource")]
	private ResourceName Boots;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab 5 Resource")]
	private ResourceName Shell;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Rare Prefab 1 Resource")]
	private ResourceName rarePrefab1;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Rare Prefab 2 Resource")]
	private ResourceName rarePrefab2;

	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Rare Prefab 3 Resource")]
	private ResourceName rarePrefab3;

	[Attribute(defvalue: "40.0", desc: "Minimum Random Delay", params: "0.0 100.0")]
	private float minRandomDelay;

	[Attribute(defvalue: "40.0", desc: "Maximum Random Delay", params: "0.0 100.0")]
	private float maxRandomDelay;

	private ref map<ResourceName, ENotification> m_PrefabNotificationMap = new map<ResourceName, ENotification>();

	override void EOnInit(IEntity owner) {
		super.EOnInit(owner);
		InitNotificationMap();
	}

	private void InitNotificationMap() {
		m_PrefabNotificationMap.Insert(Carp, ENotification.SPAWN_PREFAB_CARP);
		m_PrefabNotificationMap.Insert(Tuna, ENotification.SPAWN_PREFAB_TUNA);
		m_PrefabNotificationMap.Insert(Shiny, ENotification.SPAWN_PREFAB_SHINY);
		m_PrefabNotificationMap.Insert(Boots, ENotification.SPAWN_PREFAB_BOOT);
		m_PrefabNotificationMap.Insert(Shell, ENotification.SPAWN_PREFAB_SHELL);
		m_PrefabNotificationMap.Insert(rarePrefab1, ENotification.SPAWN_PREFAB_6);
		m_PrefabNotificationMap.Insert(rarePrefab2, ENotification.SPAWN_PREFAB_7);
		m_PrefabNotificationMap.Insert(rarePrefab3, ENotification.SPAWN_PREFAB_8);
	}

	override void ActivateAction() {
		Print("RL_FishingGadgetComponent ActivateAction");
		IEntity ownerEntity = GetOwner();
		if (!ownerEntity) return;

		IEntity playerEntity = ownerEntity.GetParent();
		if (!playerEntity) return;

		RL_FishingSenderComponent fishingComponent = RL_FishingSenderComponent.Cast(playerEntity.FindComponent(RL_FishingSenderComponent));
		if (fishingComponent && fishingComponent.IsFishing()) {
			SendNotification(ENotification.STILL_FISHING, playerEntity);
			return;
		}

		RL_FishingZoneStateComponent zoneComponent = RL_FishingZoneStateComponent.Cast(playerEntity.FindComponent(RL_FishingZoneStateComponent));
		if (!zoneComponent || !zoneComponent.GetIsInZone()) {
			SendNotification(ENotification.FISHING_OUTSIDE, playerEntity);
			return;
		}

		if (zoneComponent.IsInRestZone()) {
			SendNotification(ENotification.FISHING_IN_RESTRICTED_PLACE, playerEntity);
			return;
		}

		vector playerPos = playerEntity.GetOrigin();
		float distanceToTrader = RL_EntityLocationManager.GetInstance().GetDistanceToEntityType(playerPos, ELocationEntityType.FISH_TRADER);
		if (distanceToTrader < 2000) {
			SendNotification(ENotification.FISHING_TOO_CLOSE_TO_TRADER, playerEntity);
			return;
		}

		if (fishingComponent)
			fishingComponent.SetFishing(true);

		super.ActivateAction();
		SendNotification(ENotification.FISHIN_IN_PROGRESS, playerEntity);

		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (soundManagerEntity) {
			soundManagerEntity.CreateAndPlayAudioSource(GetOwner(), SCR_SoundEvent.SOUND_FLASHLIGHT_ON);
		}

		float randomDelay = Math.RandomFloatInclusive(minRandomDelay, maxRandomDelay);
		int delayMs = randomDelay * 1000;

		GetGame().GetCallqueue().CallLater(AddItemToInventory, delayMs, false, playerEntity);
		GetGame().GetCallqueue().CallLater(ResetFishingFlag, delayMs, false, playerEntity);
	}

	private void AddItemToInventory(IEntity playerEntity) {
		RL_FishingZoneStateComponent zoneComponent = RL_FishingZoneStateComponent.Cast(playerEntity.FindComponent(RL_FishingZoneStateComponent));
		if (!zoneComponent || !zoneComponent.GetIsInZone()) {
			SendNotification(ENotification.FISHING_OUTSIDE, playerEntity);
			return;
		}

		ResourceName selectedPrefab = SelectPrefabBasedOnChance(zoneComponent);

		if (selectedPrefab.IsEmpty()) {
			SendNotification(ENotification.NOTHING_FISHED, playerEntity);
			return;
		}

		ENotification notificationId = ENotification.NOTHING_FISHED;
		if (m_PrefabNotificationMap.Contains(selectedPrefab)) {
			notificationId = m_PrefabNotificationMap.Get(selectedPrefab);
		}

		SendNotification(notificationId, playerEntity);

		SCR_ChimeraCharacter m_character = SCR_ChimeraCharacter.Cast(playerEntity);
		m_character.PlayCommonSoundLocally("SOUND_FISHSPLASH");

		RL_FishingSenderComponent fishingComponent = RL_FishingSenderComponent.Cast(playerEntity.FindComponent(RL_FishingSenderComponent));
		if (fishingComponent) {
			fishingComponent.AddToInventory(selectedPrefab);
		}
	}

	private void ResetFishingFlag(IEntity playerEntity) {
		RL_FishingSenderComponent fishingComponent = RL_FishingSenderComponent.Cast(playerEntity.FindComponent(RL_FishingSenderComponent));
		if (fishingComponent)
			fishingComponent.SetFishing(false);
	}

	private ResourceName SelectPrefabBasedOnChance(RL_FishingZoneStateComponent zoneComponent) {
		float randomValue = Math.RandomFloatInclusive(0.0, 1.0);

		if (zoneComponent.IsInRareZone()) {
			if (randomValue <= rarePrefab1Chance) return rarePrefab1;
			if (randomValue <= rarePrefab1Chance + rarePrefab2Chance) return rarePrefab2;
			if (randomValue <= rarePrefab1Chance + rarePrefab2Chance + rarePrefab3Chance) return rarePrefab3;
			return ResourceName.Empty;
		}

		if (randomValue <= CarpChance) return Carp;
		if (randomValue <= CarpChance + TunaChance) return Tuna;
		if (randomValue <= CarpChance + TunaChance + ShinyChance) return Shiny;
		if (randomValue <= CarpChance + TunaChance + ShinyChance + BootsChance) return Boots;
		if (randomValue <= CarpChance + TunaChance + ShinyChance + BootsChance + ShellChance) return Shell;

		return ResourceName.Empty;
	}

	private void SendNotification(ENotification notificationId, IEntity playerEntity) {
		SCR_NotificationsComponent.SendLocal(notificationId);
	}

	override EGadgetType GetType() {
		return EGadgetType.FISHING_ROD;
	}
}
