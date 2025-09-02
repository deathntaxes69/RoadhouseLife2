modded class SCR_CharacterDamageManagerComponent : SCR_CharacterDamageManagerComponent {

	protected int m_iKillAfterSeconds = 600;
	protected int m_iRespawnAfterSeconds = 300;

	[RplProp()]
	protected bool m_bWaitingForCpr = false;

	[RplProp()]
	protected bool m_bMedicallyHealed = true;

	[RplProp()]
	protected int m_fDeathTime = 0;

	protected string m_sDeathId = "";

	protected bool m_bRespawned = false;

	protected override void OnDamage(notnull BaseDamageContext damageContext)
	{

		SCR_HitZone scriptedHz = SCR_HitZone.Cast(damageContext.struckHitZone);
		if (!scriptedHz)
			return;

		IEntity hzOwner = scriptedHz.GetOwner();
		if (!hzOwner)
			return;

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(hzOwner);
		if (!character)
			return;

		
		if (!EntityUtils.IsPlayer(hzOwner))
			return;

		IEntity instigatorEnt = damageContext.instigator.GetInstigatorEntity();
		SCR_ChimeraCharacter killedByCharacter = SCR_ChimeraCharacter.Cast(instigatorEnt);

		if (instigatorEnt && EntityUtils.IsPlayerVehicle(instigatorEnt.GetParent())) {
			if (GetDefaultHitZone().GetHealthScaled() < 0.1)
			{
				GetDefaultHitZone().SetHealthScaled(0.1);
			}

			return;
		}

		if (GetDefaultHitZone().GetHealthScaled() > 0.01)
			return;
			
		if(!RL_Utils.IsDedicatedServer())
		{
			SCR_ChimeraCharacter localCharacter = RL_Utils.GetLocalCharacter();
			if(character != localCharacter)
				return;

			m_bRespawned = false;

			RL_DeathScreenHUD hud = RL_DeathScreenHUD.GetCurrentInstance();
			if(!hud) return;

			if(killedByCharacter)
				hud.SetKillerProps(killedByCharacter.GetCharacterId());
			else
				hud.SetKillerProps("UNKNOWN");
		}
		if (!m_bWaitingForCpr && Replication.IsServer())
		{
			m_bWaitingForCpr = true;
			m_fDeathTime = System.GetUnixTime();
			
			Replication.BumpMe();

			CharacterControllerComponent controller = character.GetCharacterController();
			if (controller && !controller.IsUnconscious())
			{
				controller.SetUnconscious(true);
				this.UpdateConsciousness();
			}

			character.CreateEmsEmergencyMarker(character.GetOrigin(), character.GetCharacterName());

			AlertAllMedics(character.GetCharacterName());

			if (killedByCharacter)
			{
				Print(character.GetCharacterName() + " was killed by " + killedByCharacter.GetCharacterName());
				DeathLog(character, killedByCharacter);
			} else
			{
				Print(character.GetCharacterName() + " was killed by an unknown source");
				DeathLog(character, null);
			}
		}
		GetDefaultHitZone().SetHealthScaled(0.01);
	}
	void DeathLog(SCR_ChimeraCharacter character, SCR_ChimeraCharacter killedByCharacter)
	{
		SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
		m_sDeathId = RL_Utils.GenerateRandomUid();
		jsonData.WriteValue("death_id", m_sDeathId);
		jsonData.WriteValue("victimCash", character.GetCash());
		jsonData.WriteValue("deathLocation", character.GetOrigin().ToString());
		jsonData.WriteValue("victimCId", character.GetCharacterId());
		jsonData.WriteValue("victimName", character.GetCharacterName());
		if(killedByCharacter)
		{
			jsonData.WriteValue("killerLocation", killedByCharacter.GetOrigin().ToString());
			BaseWeaponManagerComponent weaponManager = killedByCharacter.GetCharacterController().GetWeaponManagerComponent();
			if (weaponManager && weaponManager.GetCurrentWeapon())
				jsonData.WriteValue("killerWeapon", weaponManager.GetCurrentWeapon().GetOwner().GetPrefabData().GetPrefabName().GetPath());
			jsonData.WriteValue("killerCId", killedByCharacter.GetCharacterId());
			jsonData.WriteValue("killerName", killedByCharacter.GetCharacterName());
			jsonData.WriteValue("distance", RL_Utils.GetDistance(character, killedByCharacter));
		}

		character.PlayerLog("Death Log", jsonData.ExportToString());
	}
	void AlertAllMedics(string patientName)
	{
		Print("AlertAllMedics");
		array<SCR_ChimeraCharacter> characters = RL_Utils.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : characters) {
			//Print("[RL_CharacterDamageManagerComp] AlertAllMedics foreach loop");

			if (!character || !character.IsMedic())
				continue;

			character.Notify(string.Format("%1 needs medical assistance. Their location has been marked on your map", patientName), "DISPATCH");

		}
	}

	protected override bool ShouldBeUnconscious()
	{
		if (IsWaitingForCpr())
		{
			return true;
		}
		return false;
		//return super.ShouldBeUnconscious();
	}

	bool IsWaitingForCpr()
	{
		return m_bWaitingForCpr;
	}
	bool HasRespawned()
	{
		return m_bRespawned;
	}
	void SetRespawned(bool respawned)
	{
		m_bRespawned = respawned;
	}
	void LocalResetCprProps(bool killAfter = false)
	{
		m_bRespawned = true;
		Rpc(RpcAsk_ResetCprProps, killAfter, false);
	}
	bool IsMedicallyHealed()
	{
		return m_bMedicallyHealed;
	}
	void SetMedicallyHealed(bool healed)
	{
		m_bMedicallyHealed = healed;
		Replication.BumpMe();
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ResetCprProps(bool killAfter, bool medicRevive)
	{
		Print("RpcAsk_ResetCprProps");
		m_bWaitingForCpr = false;
		m_fDeathTime = 0;

		if(medicRevive)
			m_bMedicallyHealed = false;
		else
			m_bMedicallyHealed = true;

		Replication.BumpMe();
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this.GetOwner());
		if(!character) return;
		character.DeleteEmsEmergencyMarker();
		if(killAfter)
			Rpc(RpcDo_ForceDeath);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ForceDeath()
	{
		Print("RpcDo_ForceDeath");
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(this.GetOwner());
		if(!character) return;
		auto cc = CharacterControllerComponent.Cast(character.FindComponent(CharacterControllerComponent));
		if(!cc) return;
		cc.ForceDeath();
	}


	int GetTimeLeftToDie() {
		if (!m_bWaitingForCpr)
		{
			Print("Not waiting for CPR why is this being called");
			return -1;
		}

		int now = System.GetUnixTime();


		int timeLeft = (m_fDeathTime + m_iKillAfterSeconds) - now;
		return Math.Max(0, timeLeft);
	}
	int GetTimeLeftToRespawn() {
		if (!m_bWaitingForCpr)
			return 0;

		int now = System.GetUnixTime();
		if (RL_Utils.IsDevServer() && m_iRespawnAfterSeconds != 20)
			m_iRespawnAfterSeconds = 20;

		int timeLeft = (m_fDeathTime + m_iRespawnAfterSeconds) - now;
		return Math.Max(0, timeLeft);
	}
	string GetDeathId()
	{
		return m_sDeathId;
	}
	void ResetDeathId()
	{
		m_sDeathId = "";
	}

}
