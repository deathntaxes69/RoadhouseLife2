[ComponentEditorProps(category: "RL/Drugs/Heroin", description: "Component for heroin campfire cooking system")]
class RL_HeroinCampfireComponentClass: ScriptComponentClass {}

class RL_HeroinCampfireComponent: ScriptComponent 
{
	protected static const int COOKING_TIME_PER_POPPY_SECONDS = 30;
	protected static const int MAX_POPPIES_PER_STOVE = 50;
	protected static const float DEFAULT_HEROIN_QUALITY = 0.20;
	protected static const float FENTANYL_HEROIN_QUALITY = 0.50;
	
	[RplProp()]
	protected int m_iPoppyCount = 0;
	
	[RplProp()]
	protected bool m_bIsCooking = false;
	
	[RplProp()]
	protected bool m_bHasFentanyl = false;
	
	[RplProp()]
	protected bool m_bCookingComplete = false;
	
	[RplProp()]
	protected float m_fCookingStartTime = 0;
	
	[RplProp()]
	protected float m_fOutputQuality = DEFAULT_HEROIN_QUALITY;
	
	[RplProp()]
	protected float m_fCurrentProgress = 0.0;
	
		//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateCookingProgress()
	{
		if (!Replication.IsServer())
			return;
			
		if (!m_bIsCooking || m_bCookingComplete)
			return;
			
		float currentTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		float elapsedTime = (currentTime - m_fCookingStartTime) / 1000.0;
		float requiredTime = GetTotalCookingTime();
		
		m_fCurrentProgress = Math.Clamp(elapsedTime / requiredTime, 0.0, 1.0);
		
		if (elapsedTime >= requiredTime)
		{
			CompleteCooking();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(UpdateCookingProgress, 10000);
		}
		
		Replication.BumpMe();
	}
	
		//------------------------------------------------------------------------------------------------
	protected override bool RplSave(ScriptBitWriter writer)
	{
		bool result = super.RplSave(writer);
		writer.WriteInt(m_iPoppyCount);
		writer.WriteBool(m_bIsCooking);
		writer.WriteBool(m_bHasFentanyl);
		writer.WriteBool(m_bCookingComplete);
		writer.WriteFloat(m_fCookingStartTime);
		writer.WriteFloat(m_fOutputQuality);
		writer.WriteFloat(m_fCurrentProgress);
		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected override bool RplLoad(ScriptBitReader reader)
	{
		bool result = super.RplLoad(reader);
		reader.ReadInt(m_iPoppyCount);
		reader.ReadBool(m_bIsCooking);
		reader.ReadBool(m_bHasFentanyl);
		reader.ReadBool(m_bCookingComplete);
		reader.ReadFloat(m_fCookingStartTime);
		reader.ReadFloat(m_fOutputQuality);
		reader.ReadFloat(m_fCurrentProgress);
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanAddPoppies()
	{
		return !m_bIsCooking && !m_bCookingComplete && m_iPoppyCount < MAX_POPPIES_PER_STOVE;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanStartCooking()
	{
		return !m_bIsCooking && !m_bCookingComplete && m_iPoppyCount > 0;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanAddFentanyl()
	{
		return !m_bHasFentanyl && !m_bCookingComplete && m_iPoppyCount > 0;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanTakePaste()
	{
		return m_bCookingComplete && m_iPoppyCount > 0;
	}
	
	//------------------------------------------------------------------------------------------------
	int AddPoppies(int amount)
	{
		if (!CanAddPoppies())
			return 0;
			
		int maxCanAdd = MAX_POPPIES_PER_STOVE - m_iPoppyCount;
		int actualAmount = Math.Min(amount, maxCanAdd);
		
		m_iPoppyCount += actualAmount;
			
		Replication.BumpMe();
		return actualAmount;
	}
	
	//------------------------------------------------------------------------------------------------
	bool StartCooking()
	{
		if (!CanStartCooking())
			return false;
			
		m_bIsCooking = true;
		m_fCookingStartTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		m_fCurrentProgress = 0.0;
		
		if (m_bHasFentanyl)
			m_fOutputQuality = FENTANYL_HEROIN_QUALITY;
		else
			m_fOutputQuality = DEFAULT_HEROIN_QUALITY;
		
		GetGame().GetCallqueue().CallLater(UpdateCookingProgress, 10000);
			
		Replication.BumpMe();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool AddFentanyl()
	{
		if (!CanAddFentanyl())
			return false;
			
		m_bHasFentanyl = true;
			
		Replication.BumpMe();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CompleteCooking()
	{
		m_bIsCooking = false;
		m_bCookingComplete = true;
		m_fCurrentProgress = 1.0;
			
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int TakeAllPaste()
	{
		if (!CanTakePaste())
			return 0;
			
		int pasteCount = m_iPoppyCount;
		
		m_iPoppyCount = 0;
		m_bIsCooking = false;
		m_bHasFentanyl = false;
		m_bCookingComplete = false;
		m_fCookingStartTime = 0;
		m_fCurrentProgress = 0.0;
		m_fOutputQuality = DEFAULT_HEROIN_QUALITY;
			
		Replication.BumpMe();
		return pasteCount;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTotalCookingTime()
	{
		return m_iPoppyCount * COOKING_TIME_PER_POPPY_SECONDS;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetCookingProgress()
	{
		return m_fCurrentProgress;
	}
	
		//------------------------------------------------------------------------------------------------
	string GetStatusText()
	{
		if (m_bCookingComplete)
		{
			return string.Format("Ready - %1 paste available", m_iPoppyCount);
		}
		
		if (m_bIsCooking)
		{
			float progress = GetCookingProgress() * 100;
			return string.Format("Cooking... %1%", Math.Round(progress));
		}
		
		if (m_iPoppyCount > 0)
		{
			return string.Format("Ready to cook - %1 poppies", m_iPoppyCount);
		}
		
		return "Empty";
	}

	//------------------------------------------------------------------------------------------------
	int GetPoppyCount() { return m_iPoppyCount; }
	bool IsCooking() { return m_bIsCooking; }
	bool HasFentanyl() { return m_bHasFentanyl; }
	bool IsCookingComplete() { return m_bCookingComplete; }
	float GetOutputQuality() { return m_fOutputQuality; }
	int GetMaxPoppies() { return MAX_POPPIES_PER_STOVE; }
}
