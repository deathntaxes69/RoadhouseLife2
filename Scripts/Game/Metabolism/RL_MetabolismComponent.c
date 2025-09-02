class RL_MetabolismComponentClass : ScriptComponentClass {}

class RL_MetabolismComponent : ScriptComponent
{
	[Attribute("0.004", UIWidgets.Auto, category: "DepletionRate")]
	protected float m_fIdleFoodDepletionRate;

	[Attribute("0.005", UIWidgets.Auto, category: "DepletionRate")]
	protected float m_fIdleThirstDepletionRate;

	[Attribute("0.005", UIWidgets.Auto, category: "DepletionRate")]
	protected float m_fWalkingFoodDepletionRate;

	[Attribute("0.006", UIWidgets.Auto, category: "DepletionRate")]
	protected float m_fWalkingThirstDepletionRate;

	[Attribute("0.007", UIWidgets.Auto, category: "DepletionRate")]
	protected float m_fSprintingFoodDepletionRate;

	[Attribute("0.008", UIWidgets.Auto, category: "DepletionRate")]
	protected float m_fSprintingThirstDepletionRate;

	[Attribute("30", UIWidgets.Slider, params: "1 300 1", category: "DepletionRate")]
	protected int m_iDepletionInterval;

	[Attribute("8", UIWidgets.Auto, category: "Starvation")]
	protected float m_fHungerStarvationDamage;

	[Attribute("8", UIWidgets.Auto, category: "Starvation")]
	protected float m_fThirstStarvationDamage;

	//[Attribute("22", UIWidgets.Slider, params: "1 300 1", category: "Starvation")]
	//protected int m_iStarvationInterval;

	[Attribute(defvalue: "1", UIWidgets.Slider, params: "0 1 0.01", category: "State"), RplProp()]
	protected float m_fFoodLevel;

	[Attribute(defvalue: "1", UIWidgets.Slider, params: "0 1 0.01", category: "State"), RplProp()]
	protected float m_fThirstLevel;

	[Attribute(defvalue: "false", category: "Debug")]
	protected bool m_bDebugPrints;

	protected SCR_ChimeraCharacter m_character;
	protected SCR_CharacterDamageManagerComponent m_DamageManager;
	protected bool m_bStarving = false;
	protected bool m_bDehydrated = false;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		if (m_bDebugPrints)
			Print("RL_MetabolismComponent 0");

		if (!Replication.IsServer())
			return;

		if (m_bDebugPrints)
			Print("RL_MetabolismComponent owner=" + owner);

		m_character = SCR_ChimeraCharacter.Cast(owner);
		m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if(!m_character || !m_DamageManager) return;
		//TODO make this run client side then ask server for damage, or 1 loop in event manager??
		GetGame().GetCallqueue().CallLater(Tick, m_iDepletionInterval * 1000, true);
		//GetGame().GetCallqueue().CallLater(TickDepletion, m_iDepletionInterval * 1000, true);
		//GetGame().GetCallqueue().CallLater(TickStarvation, m_iStarvationInterval * 1000, true);

		if (m_bDebugPrints)
			Print("RL_MetabolismComponent initialized.");
	}

	override event protected void OnDelete(IEntity owner)
	{
		if (!Replication.IsServer())
			return;

		if (m_bDebugPrints)
			Print("RL_MetabolismComponent OnDelete owner=" + owner);

		EndLoops();
	}

	void EndLoops()
	{
		if (m_bDebugPrints)
			Print("RL_MetabolismComponent EndLoops");

		GetGame().GetCallqueue().Remove(Tick);
		//GetGame().GetCallqueue().Remove(TickDepletion);
		//GetGame().GetCallqueue().Remove(TickStarvation);
	}

	void Tick()
	{
		TickDepletion();
		TickStarvation();
	}

	void TickDepletion()
	{
		if (m_bDebugPrints)
			Print("TickDepletion");

		if (!m_DamageManager)
			return;

		if (m_DamageManager.IsWaitingForCpr())
			return;

		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(GetOwner().FindComponent(SCR_CharacterControllerComponent));
		if (!controller)
			return;

		int movementSpeed = controller.GetMovementSpeed();

		if (m_bDebugPrints)
			PrintFormat("DEBUG: Current movementSpeed = %1", movementSpeed);

		if (movementSpeed == 0)
		{
			if (m_bDebugPrints)
				Print("DEBUG: Detected IDLE state.");

			if (m_fFoodLevel > 0)
				SetFoodLevel(m_fFoodLevel - m_fIdleFoodDepletionRate);
			if (m_fThirstLevel > 0)
				SetThirstLevel(m_fThirstLevel - m_fIdleThirstDepletionRate);

		} else if (movementSpeed >= 1 && movementSpeed <= 2) {
			if (m_bDebugPrints)
				Print("DEBUG: Detected WALKING/JOGGING state.");

			if (m_fFoodLevel > 0)
				SetFoodLevel(m_fFoodLevel - m_fWalkingFoodDepletionRate);
			if (m_fThirstLevel > 0)
				SetThirstLevel(m_fThirstLevel - m_fWalkingThirstDepletionRate);

		} else if (movementSpeed == 3) {
			if (m_bDebugPrints)
				Print("DEBUG: Detected SPRINTING state.");

			if (m_fFoodLevel > 0)
				SetFoodLevel(m_fFoodLevel - m_fSprintingFoodDepletionRate);
			if (m_fThirstLevel > 0)
				SetThirstLevel(m_fThirstLevel - m_fSprintingThirstDepletionRate);
		}

		if (m_bDebugPrints)
			PrintFormat("Food after drain: %1 | Thirst after drain: %2", m_fFoodLevel, m_fThirstLevel);
	}

	void TickStarvation()
	{
		if (m_bDebugPrints)
			Print("TickStarvation");

		if (!m_DamageManager)
			return;

		if (m_DamageManager.IsWaitingForCpr())
			return;

		if (m_fFoodLevel <= 0)
		{
			if (!m_bStarving && m_bDebugPrints)
				Print("Starvation started: Food at 0");

			m_bStarving = true;
			ApplyStarvationDamage(m_fHungerStarvationDamage);
		} else {
			if (m_bStarving && m_bDebugPrints)
				Print("Starvation stopped: Food above 0");

			m_bStarving = false;
		}

		if (m_fThirstLevel <= 0)
		{
			if (!m_bDehydrated && m_bDebugPrints)
				Print("Dehydration started: Thirst at 0");

			m_bDehydrated = true;
			ApplyDehydrationDamage(m_fThirstStarvationDamage);
		} else {
			if (m_bDehydrated && m_bDebugPrints)
				Print("Dehydration stopped: Thirst above 0");

			m_bDehydrated = false;
		}
	}
	void ApplyStarvationDamage(float damageValue)
	{
		if(m_character)
			m_character.Notify("You’re starving to death. You need to eat", "HEALTH");
		ApplyDamage(damageValue);
	}
	void ApplyDehydrationDamage(float damageValue)
	{
		if(m_character)
			m_character.Notify("You’re dying from dehydration. Go get something to drink", "HEALTH");
		ApplyDamage(damageValue);
	}
	void ApplyDamage(float damageValue)
	{

		if (!m_DamageManager)
			m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));

		if (!m_DamageManager)
			return;

		vector outMat[3];
		SCR_DamageContext damageContext = new SCR_DamageContext(EDamageType.TRUE, 0, outMat, GetOwner(), null, Instigator.CreateInstigator(GetOwner()), null, -1, -1);
		HitZone hitZone = m_DamageManager.GetDefaultHitZone();
		damageContext.damageValue = damageValue;
		damageContext.struckHitZone = hitZone;
		DamageManagerComponent dmgManager = DamageManagerComponent.Cast(hitZone.GetHitZoneContainer());
		dmgManager.HandleDamage(damageContext);

		//if (m_bDebugPrints)
			PrintFormat("Applied starvation damage: %1 for %2", damageValue, m_character.GetCharacterId());
	}

	void SetFoodLevel(float value)
	{
		m_fFoodLevel = Math.Clamp(value, 0, 1);
		Replication.BumpMe();
	}

	void SetThirstLevel(float value)
	{
		m_fThirstLevel = Math.Clamp(value, 0, 1);
		Replication.BumpMe();
	}

	void IncreaseFoodLevel(float amount)
	{
		SetFoodLevel(m_fFoodLevel + amount);

		if (m_bDebugPrints)
			PrintFormat("IncreaseFoodLevel: +%1 (New: %2)", amount, m_fFoodLevel);
	}

	void IncreaseThirstLevel(float amount)
	{
		SetThirstLevel(m_fThirstLevel + amount);

		if (m_bDebugPrints)
			PrintFormat("IncreaseThirstLevel: +%1 (New: %2)", amount, m_fThirstLevel);
	}

	void TriggerFoodDrinkIncrease(float totalFood, float totalThirst)
	{
		if (totalFood > 0)
			IncreaseFoodLevel(totalFood);

		if (totalThirst > 0)
			IncreaseThirstLevel(totalThirst);

		if (m_bDebugPrints)
			PrintFormat("Instantly added: +%1 food, +%2 thirst", totalFood, totalThirst);
	}

	float GetFoodLevel()
	{
		return m_fFoodLevel;
	}

	float GetThirstLevel()
	{
		return m_fThirstLevel;
	}

}
