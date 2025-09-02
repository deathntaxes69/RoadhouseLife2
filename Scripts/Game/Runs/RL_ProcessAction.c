class RL_ProcessAction : RL_ProgressBarAction
{
	protected static const int PROGRESS_LOOP_INTERVAL_MS = 500;
	
	[Attribute("", UIWidgets.Object, "List of inputs")]
	ref array<ref RL_ProcessingInput> m_aProcessingInputs;

	[Attribute("", UIWidgets.Object, "List of outputs")]
	ref array<ref RL_ProcessingOutput> m_aProcessingOutputs;

	[Attribute("", UIWidgets.EditBox, "Control Zone ID")]
	int m_sControlZoneId;

	[Attribute(defvalue: "0", uiwidget: UIWidgets.Auto, desc: "Process Fee")]
	protected float m_fProcessFee;

	[Attribute("", UIWidgets.EditBox, "Processer Name")]
	protected string m_sProcesserName;

	[Attribute(defvalue: "1", category: "Skill", desc: "Skill Category", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RL_SkillsCategory))]
	RL_SkillsCategory m_eSkillCategory;

	[Attribute(defvalue: "1", category: "Skill", desc: "XP per item processed", uiwidget: UIWidgets.Auto)]
	int m_iXpPerItem;

	[Attribute(defvalue: "0", desc: "Enforce license requirement")]
	protected bool m_bEnforceLicenseRequirement;

	[Attribute(defvalue: "1", desc: "Required license type", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ELicenseType))]
	protected ELicenseType m_eRequiredLicense;

	protected float m_fProcessStartTime = 0;
	protected float m_fStartDistance = 0;
	protected string m_sInputDisplayName;
	protected int m_iProcessCount = 0;
	protected float m_iTotalProcessTime = 0;
	protected Widget m_wProgressBar;
	protected SCR_ChimeraCharacter m_userEntity;
	protected bool m_bWaitingForFeesDecision = false;
	protected bool m_bFeesDecisionMade = false;
	protected bool m_bFeesApproved = false;
	protected RL_PayFeesUI m_payFeesUI;
	protected SCR_ChimeraCharacter m_currentCharacter;

	//protected ref map<int, int> m_mCharacterProcessCount = new map<int, int>();

	//------------------------------------------------------------------------------------------------	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;

		m_userEntity = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!m_userEntity)
			return;
		
		int processCount = m_userEntity.GetProcessCount();
		Print(string.Format("PerformAction found %1 for %2", processCount, EPF_NetworkUtils.GetRplId(m_userEntity)));
		Print("Before ValidateProcessing");
		if (!ValidateProcessing(pUserEntity, processCount))
		{
			Print("Failed ValidateProcessing");
			m_userEntity.Notify("Process Failed", "PROCESS");
			return;
		}
		Print("Before ProcessItems");
		ProcessItems(pUserEntity, processCount);
		AwardExperience(m_userEntity, processCount);
		m_userEntity.SetProcessCount(0);
	}
	// DONT USE THIS DUMB SHIT IT SOMETIMES DOES NOT RUN
	override void OnActionStart(IEntity pUserEntity)
	{
		//ResetFeesState();
		//int maxProcess = GetMaxPossibleProcesses(pUserEntity);
		//Print(string.Format("OnActionStart set %1 for %2", maxProcess, EPF_NetworkUtils.GetRplId(pUserEntity)));
		//m_mCharacterProcessCount.Set(EPF_NetworkUtils.GetRplId(pUserEntity), maxProcess);

	}	
	override void OnConfirmed(IEntity pUserEntity)
	{
		Print("OnConfirmed");

	}	
	//------------------------------------------------------------------------------------------------
	override Widget StartProgressBar(SCR_ChimeraCharacter character)
	{

		Print("StartProgressBar override");
		if (IsProgressBarActive())
		{
			RL_Utils.Notify("Action already in progress.", "ACTION");
			return null;
		}
		
		ResetFeesState();

		if (ShouldShowFeesUI() && !m_bFeesDecisionMade)
		{
			HandleFeesUI(character);
			return null; // Delay progress bar
		}
		m_iProcessCount = GetMaxPossibleProcesses(character);
		Widget progressBar = super.StartProgressBar(character);
		if(!progressBar) return null;
		character.SetProcessCount(m_iProcessCount);

		Print(progressBar);
		return progressBar;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ShouldShowFeesUI()
	{
		return (m_sControlZoneId > 0 && m_fProcessFee > 0);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HandleFeesUI(SCR_ChimeraCharacter character)
	{
		if (!character)
			return;
		
		m_currentCharacter = character;
		
		RL_ControlZoneEntity controlZone = GetControlZone();
		if (!controlZone)
		{
			ProceedWithoutFees(character);
			return;
		}
		
		int controllingGangId = controlZone.GetControllingGangId();
		if (!IsFeesRequired(character, controllingGangId))
		{
			ProceedWithoutFees(character);
			return;
		}
		
		ShowFeesUI(character, controlZone, controllingGangId);
	}
	
	//------------------------------------------------------------------------------------------------
	protected RL_ControlZoneEntity GetControlZone()
	{
		RL_ControlZoneManager controlZoneManager = RL_ControlZoneManager.GetInstance();
		if (!controlZoneManager)
		{
			return null;
		}
		
		RL_ControlZoneEntity controlZone = controlZoneManager.FindControlZoneById(m_sControlZoneId);
		if (!controlZone)
		{
			return null;
		}
		
		return controlZone;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsFeesRequired(SCR_ChimeraCharacter character, int controllingGangId)
	{
		if (controllingGangId == -1)
		{
			return false;
		}
		
		int userGangId = character.GetGangId();
		if (userGangId == controllingGangId)
		{
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ShowFeesUI(SCR_ChimeraCharacter character, RL_ControlZoneEntity controlZone, int controllingGangId)
	{
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.RL_PayFeesMenu);
		m_payFeesUI = RL_PayFeesUI.Cast(menuBase);
		
		if (!m_payFeesUI)
		{
			ProceedWithoutFees(character);
			return;
		}
		
		string controllingGangName = GetGangDisplayName(controlZone, controllingGangId);
		m_payFeesUI.SetProcessAction(this, character, controllingGangId, controllingGangName);
		m_payFeesUI.GetOnFeesDecision().Insert(OnFeesDecision);
		
		m_bWaitingForFeesDecision = true;
		m_bFeesDecisionMade = false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected string GetGangDisplayName(RL_ControlZoneEntity controlZone, int gangId)
	{
		string gangName = controlZone.GetControllingGangName();
		
		if (!gangName || gangName.IsEmpty())
			gangName = controlZone.GetGangName(gangId);
		
		return gangName;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ProceedWithoutFees(SCR_ChimeraCharacter character)
	{
		m_bFeesDecisionMade = true;
		m_bWaitingForFeesDecision = false;
		StartProgressAfterFeesDecision(character);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnFeesDecision(bool approved)
	{
		m_bFeesDecisionMade = true;
		m_bFeesApproved = approved;
		m_bWaitingForFeesDecision = false;
		
		CleanupFeesUI();
		StartProgressAfterFeesDecision(m_currentCharacter);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CleanupFeesUI()
	{
		if (m_payFeesUI)
		{
			m_payFeesUI.GetOnFeesDecision().Remove(OnFeesDecision);
			m_payFeesUI = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void StartProgressAfterFeesDecision(SCR_ChimeraCharacter character)
	{
		if (!character)
			return;
		
		m_iProcessCount = GetMaxPossibleProcesses(character);
		Widget progressBar = super.StartProgressBar(character);
		if (!progressBar) 
			return;
		character.SetProcessCount(m_iProcessCount);
			
		float progressStartTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		GetGame().GetCallqueue().CallLater(ProgressLoop, PROGRESS_LOOP_INTERVAL_MS, false, this, progressStartTime, character.GetOrigin());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ProgressLoop(RL_ProgressBarAction action, float startTime, vector startPos)
	{   

		if (!m_currentCharacter)
		{
			EndProgressBar();
			return;
		}

		if (ShouldCancelProgress(startPos))
		{
			EndProgressBar();
			RL_Utils.Notify("Canceled", "ACTION");
			return;
		}

		if (IsProgressComplete(startTime))
		{
			EndProgressBar();
			m_currentCharacter.DoPerformObjectAction(action);
			return;
		}
		
		GetGame().GetCallqueue().CallLater(ProgressLoop, PROGRESS_LOOP_INTERVAL_MS, false, action, startTime, startPos);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ShouldCancelProgress(vector startPos)
	{
		float distance = vector.Distance(startPos, m_currentCharacter.GetOrigin());
		return (distance > GetMaxMoveDistance() || !RL_Utils.CanPerformAction(m_currentCharacter));
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsProgressComplete(float startTime)
	{
		float currentTime = GetGame().GetWorld().GetTimestamp().DiffMilliseconds(null);
		return (currentTime - startTime) >= GetProgressTime();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ResetFeesState()
	{
		m_bFeesDecisionMade = false;
		m_bWaitingForFeesDecision = false;
		m_bFeesApproved = false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ValidateProcessing(IEntity user , int processCount)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(user);
		if (!inventoryManager) 
			return false;
		int maxProcess = GetMaxPossibleProcesses(user);
		Print(string.Format("ValidateProcessing passed %1 and new count is %2", processCount, maxProcess));
		if (processCount < 1 || processCount > maxProcess)
		{
			Print("ValidateProcessing return false");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ProcessItems(IEntity user , int processCount)
	{
		Print("Start ProcessItems");
		Print(processCount);
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(user);
		if(!inventoryManager) return;
		
		ref array<ref DrugInfo> drugInfos = {};
		CollectDrugInfo(inventoryManager, processCount, drugInfos);
		
		foreach (RL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			Print("[RL_ProcessAction] ProcessItems foreach");
			int totalAmount = (processCount * processingInput.m_iInputAmount);
			int amountRemoved = RL_InventoryUtils.RemoveAmount(inventoryManager, processingInput.m_InputPrefab, totalAmount);
			if (amountRemoved < totalAmount)
			{
				Print("ProcessItems removed less than total");
				return;
			}
		}
		
		foreach (RL_ProcessingOutput processingOutput : m_aProcessingOutputs)
		{
			Print("[RL_ProcessAction] ProcessItems foreach 2");
			int totalAmount = (processCount * processingOutput.m_iOutputAmount);
			array<RplId> addedItemIds = {};
			RL_InventoryUtils.AddAmountWithIds(inventoryManager, processingOutput.m_OutputPrefab, totalAmount, addedItemIds);
			ApplyDrugProcessing(addedItemIds, drugInfos);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CollectDrugInfo(InventoryStorageManagerComponent inventoryManager, int processCount, out array<ref DrugInfo> drugInfos)
	{
		foreach (RL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			Print("[RL_ProcessAction] CollectDrugInfo foreach");
			array<IEntity> inputItems = RL_InventoryUtils.FindItemsByPrefab(inventoryManager, processingInput.m_InputPrefab);
			if (!inputItems || inputItems.Count() == 0)
				continue;
				
			int totalAmountNeeded = processCount * processingInput.m_iInputAmount;
			int itemsToProcess = Math.Min(totalAmountNeeded, inputItems.Count());
			
			for (int i = 0; i < itemsToProcess; i++)
			{
				Print("[RL_ProcessAction] CollectDrugInfo for");
				IEntity item = inputItems[i];
				if (!item) continue;
				
				RL_DrugComponent drugComp = RL_DrugComponent.Cast(item.FindComponent(RL_DrugComponent));
				if (drugComp)
				{
					DrugInfo info = new DrugInfo();
					info.m_sName = drugComp.GetName();
					info.m_fQuality = drugComp.GetQuality();
					drugInfos.Insert(info);
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ApplyDrugProcessing(array<RplId> addedItemIds, array<ref DrugInfo> drugInfos)
	{
		if (!drugInfos || drugInfos.Count() == 0)
			return;
		
		int infoIndex = 0;
		foreach (RplId itemId : addedItemIds)
		{
			Print("[RL_ProcessAction] ApplyDrugProcessing foreach");
			if (infoIndex >= drugInfos.Count())
				break;
				
			IEntity processedItem = EPF_NetworkUtils.FindEntityByRplId(itemId);
			if (!processedItem)
				continue;
				
			RL_DrugComponent drugComp = RL_DrugComponent.Cast(processedItem.FindComponent(RL_DrugComponent));
			if (!drugComp)
				continue;
				
			DrugInfo drugInfo = drugInfos[infoIndex];
			drugComp.SetDrugInfo(drugInfo.m_sName, drugInfo.m_fQuality);
			
			infoIndex++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AwardExperience(SCR_ChimeraCharacter user, int processCount)
	{
		if (m_iXpPerItem > 0 && processCount > 0)
		{
			int totalXp = m_iXpPerItem * processCount;
			user.AddXp(m_eSkillCategory, totalXp);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override float GetProgressTime()
    {
		Print("GetProgressTime");
		if (m_bWaitingForFeesDecision)
		{
			Print("GetProgressTime m_bWaitingForFeesDecision");
			return 0;
		}
		SCR_ChimeraCharacter character = RL_Utils.GetLocalCharacter();
		if(!character)
		{
			Print("GetProgressTime No Character");
			Print(character);
		}
		if (m_iProcessCount < 1)
		{
			Print("GetProgressTime processCount");
			return 0;
		}
		float modifiedProcessTime = GetModifiedProcessTime(m_fProgressTime);
		float totalTime = m_iProcessCount * modifiedProcessTime;
		return totalTime;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetModifiedProcessTime(float baseProcessTime)
	{
		if (!m_userEntity)
			return baseProcessTime;
		
		float timeMultiplier = 1.0;

		if (m_userEntity.HasPerk("processing_efficiency"))
		{
			timeMultiplier = 0.9; // 10% reduction
		}
		Print("modifiedProcessTime");
		float modifiedProcessTime = baseProcessTime * timeMultiplier;
		Print(modifiedProcessTime);
		return modifiedProcessTime;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMaxPossibleProcesses(IEntity user)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(user);
		if (!inventoryManager)
		{
			Print("GetMaxPossibleProcesses found no InventoryStorageManagerComponent");
			return 0;
		}
		int possibleProcesses = 1000;

		foreach (RL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			Print("[RL_ProcessAction] GetMaxPossibleProcesses foreach");
			int inputPrefabsInInv = RL_InventoryUtils.GetAmount(inventoryManager, processingInput.m_InputPrefab);
			Print(string.Format("Found %1 x %2", inputPrefabsInInv, processingInput.m_InputPrefab));
			int processesWithThisInput = Math.Floor(inputPrefabsInInv / processingInput.m_iInputAmount);
			Print(processesWithThisInput);
			possibleProcesses = Math.Min(possibleProcesses, processesWithThisInput);
			Print(possibleProcesses);
		}
		Print("maxProcess");
		int maxProcess = Math.Max(0, possibleProcesses);
		return maxProcess;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool m_bLastResult = false;
	protected int m_fUpdateInterval = 1;
	protected int m_fLastUpdateTime = 0;
	override bool CanBePerformedScript(IEntity user)
 	{
		//Print(string.Format("CanbePreformed %1", m_fLastUpdateTime));
		if (((System.GetUnixTime() - m_fLastUpdateTime) < m_fUpdateInterval) && m_fLastUpdateTime != 0)
		{
			return m_bLastResult;
		}
		Print("------------------ run CanbePreformed");
		m_fLastUpdateTime = System.GetUnixTime();
		if (m_bEnforceLicenseRequirement)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
			if (!character)
			{
				SetCannotPerformReason("Invalid character");
				m_bLastResult = false;
				return false;
			}
			
			string licenseString = m_eRequiredLicense.ToString();
			if (!character.HasLicense(licenseString))
			{
				string licenseName = GetLicenseName(m_eRequiredLicense);
				SetCannotPerformReason(string.Format("Required license: %1", licenseName));
				m_bLastResult = false;
				return false;
			}
		}
		
		if (GetMaxPossibleProcesses(user) > 0)
		{
			m_bLastResult = true;
			return true;
		}
		SetCannotPerformReason("Missing item");
		m_bLastResult = false;
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetProcessFee()
	{
		return m_fProcessFee;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetProcesserName()
	{
		return m_sProcesserName;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetControlZoneId()
	{
		return m_sControlZoneId;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasControlZone()
	{
		return (m_sControlZoneId > 0);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_sInputDisplayName)
		{
			m_sInputDisplayName = BuildInputDisplayName();
		}

		if (m_fProcessStartTime != 0)
			outName = "Stop Processing";
		else
			outName = string.Format("Process %1", m_sInputDisplayName);

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected string BuildInputDisplayName()
	{
		string readableNames;
		
		foreach (RL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			Print("[RL_ProcessAction] BuildInputDisplayName foreach");
			UIInfo uiInfo = RL_ShopUtils.GetItemDisplayInfo(processingInput.m_InputPrefab);
			if (!uiInfo) 
				continue;
				
			if (!readableNames)
				readableNames = uiInfo.GetName();
			else
				readableNames = string.Format("%1, %2", readableNames, uiInfo.GetName());
		}
		
		if (readableNames.IsEmpty())
			return "Unknown";
		else
			return readableNames;
	}
}

[BaseContainerProps()]
class RL_ProcessingInput
{
	[Attribute(ResourceName.Empty, UIWidgets.ResourcePickerThumbnail, "Prefab to Input", "et")]
	ResourceName m_InputPrefab;

	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input/s amount per process")]
	int m_iInputAmount;
}

[BaseContainerProps()]
class RL_ProcessingOutput
{
	[Attribute(ResourceName.Empty, UIWidgets.ResourcePickerThumbnail, "Prefab to Output", "et")]
	ResourceName m_OutputPrefab;

	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Output amount per process")]
	int m_iOutputAmount;
}

//------------------------------------------------------------------------------------------------
class DrugInfo
{
	string m_sName;
	float m_fQuality;
}

