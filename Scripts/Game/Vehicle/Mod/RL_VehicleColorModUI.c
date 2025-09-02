class RL_VehicleColorModUI : ChimeraMenuBase
{
	protected IEntity m_Vehicle;
	protected RL_VehicleModsComponent m_VehicleModsComponent;
	protected RL_VehicleModSprayCanGadget m_SprayCanGadget;
	
	protected Widget m_wRoot;
	protected SCR_ListBoxComponent m_partsList;
	protected SCR_EditBoxComponent m_wEditBoxR;
	protected SCR_EditBoxComponent m_wEditBoxG;
	protected SCR_EditBoxComponent m_wEditBoxB;
	protected SCR_EditBoxComponent m_wEditBoxBrightness;
	protected ImageWidget m_wColorPreview;
	protected SCR_InputButtonComponent m_wSetColorsButton;
	
	protected ref array<string> m_aSlotNames;
	protected string m_sSelectedSlot;
	protected bool m_bIsBodySelected;
	
	protected ref map<string, int> m_mOriginalColors;
	protected int m_iOriginalBodyColor;
	
	protected ref map<string, ref array<float>> m_mSlotSettings;
	protected ref array<float> m_aBodySettings;
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		Widget partsListWidget = m_wRoot.FindAnyWidget("partList");
		if (partsListWidget)
			m_partsList = SCR_ListBoxComponent.Cast(partsListWidget.FindHandler(SCR_ListBoxComponent));
		
		Widget editBoxR = m_wRoot.FindAnyWidget("ARGBEditBoxR");
		if (editBoxR)
			m_wEditBoxR = SCR_EditBoxComponent.Cast(editBoxR.FindHandler(SCR_EditBoxComponent));
		
		Widget editBoxG = m_wRoot.FindAnyWidget("ARGBEditBoxG");
		if (editBoxG)
			m_wEditBoxG = SCR_EditBoxComponent.Cast(editBoxG.FindHandler(SCR_EditBoxComponent));
		
		Widget editBoxB = m_wRoot.FindAnyWidget("ARGBEditBoxB");
		if (editBoxB)
			m_wEditBoxB = SCR_EditBoxComponent.Cast(editBoxB.FindHandler(SCR_EditBoxComponent));
		
		Widget editBoxBrightness = m_wRoot.FindAnyWidget("BrightnessEditBox");
		if (editBoxBrightness)
			m_wEditBoxBrightness = SCR_EditBoxComponent.Cast(editBoxBrightness.FindHandler(SCR_EditBoxComponent));
		
		m_wColorPreview = ImageWidget.Cast(m_wRoot.FindAnyWidget("ARGBColor"));
		
		Widget setColorsWidget = m_wRoot.FindAnyWidget("SetColors");
		if (setColorsWidget)
			m_wSetColorsButton = SCR_InputButtonComponent.Cast(setColorsWidget.FindHandler(SCR_InputButtonComponent));
		
		if (m_partsList)
			m_partsList.m_OnChanged.Insert(OnPartSelected);
		
		if (m_wEditBoxR)
			m_wEditBoxR.m_OnChanged.Insert(OnColorChanged);
		if (m_wEditBoxG)
			m_wEditBoxG.m_OnChanged.Insert(OnColorChanged);
		if (m_wEditBoxB)
			m_wEditBoxB.m_OnChanged.Insert(OnColorChanged);
		if (m_wEditBoxBrightness)
			m_wEditBoxBrightness.m_OnChanged.Insert(OnColorChanged);
		
		if (m_wSetColorsButton)
			m_wSetColorsButton.m_OnActivated.Insert(OnSetColorsPressed);
		
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVehicleData(IEntity vehicle, RL_VehicleModsComponent modsComponent, RL_VehicleModSprayCanGadget sprayCanGadget = null)
	{
		m_Vehicle = vehicle;
		m_VehicleModsComponent = modsComponent;
		m_SprayCanGadget = sprayCanGadget;
		InitializeSlotSettings();
		StoreOriginalColors();
		PopulatePartsList();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PopulatePartsList()
	{
		if (!m_partsList || !m_VehicleModsComponent)
			return;
		
		m_partsList.Clear();
		m_aSlotNames = {};

		array<ref array<string>> slotsColorData = m_VehicleModsComponent.GetSlotsColor();
		if (slotsColorData)
		{
			foreach (array<string> slotData : slotsColorData)
			{
				if (slotData && slotData.Count() >= 2)
				{
					string slotName = slotData[0];
					if (slotName && !slotName.IsEmpty())
					{
						string displayName = FormatSlotName(slotName);
						m_partsList.AddItem(displayName);
						m_aSlotNames.Insert(slotName);
					}
				}
			}
		}
		
		if (m_aSlotNames.Count() > 0)
		{
			m_partsList.SetItemSelected(0, true);
			OnPartSelected(m_partsList, 0, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected string FormatSlotName(string slotName)
	{
		if (slotName == "body")
			return "Vehicle Body";
			
		string formatted = slotName;
		formatted.Replace("_", " ");
		array<string> words = {};
		formatted.Split(" ", words, true);
		string result = "";
		foreach (string word : words)
		{
			word.ToLower();
			if (word.Length() > 0)
			{
				string firstChar = word.Substring(0, 1);
				firstChar.ToUpper();
				
				if (!result.IsEmpty())
					result += " ";
				
				if (word.Length() > 1)
					result += firstChar + word.Substring(1, word.Length() - 1);
				else
					result += firstChar;
			}
		}
		
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPartSelected(SCR_ListBoxComponent listBox, int selectedRow, bool selected)
	{
		if (!selected || selectedRow < 0 || selectedRow >= m_aSlotNames.Count())
			return;
		
		SaveCurrentSlotSettings();
		m_sSelectedSlot = m_aSlotNames[selectedRow];
		m_bIsBodySelected = (m_sSelectedSlot == "body");
		LoadCurrentColor();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void LoadCurrentColor()
	{
		if (!m_VehicleModsComponent)
			return;
		
		array<float> savedSettings = GetCurrentSlotSettings();
		if (savedSettings && savedSettings.Count() >= 4)
		{
			if (m_wEditBoxR)
				m_wEditBoxR.SetValue(savedSettings[0].ToString());
			if (m_wEditBoxG)
				m_wEditBoxG.SetValue(savedSettings[1].ToString());
			if (m_wEditBoxB)
				m_wEditBoxB.SetValue(savedSettings[2].ToString());
			if (m_wEditBoxBrightness)
				m_wEditBoxBrightness.SetValue(savedSettings[3].ToString());
			
			OnColorChanged(null);
			return;
		}
		
		int currentColor = 0xFFFFFFFF;
		
		if (m_bIsBodySelected)
		{
			if (m_Vehicle)
			{
				ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(m_Vehicle.FindComponent(ParametricMaterialInstanceComponent));
				if (matComp)
					currentColor = matComp.GetColor();
			}
		}
		else
		{
			array<ref array<string>> slotsColorData = m_VehicleModsComponent.GetSlotsColor();
			if (slotsColorData)
			{
				foreach (array<string> slotData : slotsColorData)
				{
					if (slotData && slotData.Count() >= 2 && slotData[0] == m_sSelectedSlot)
					{
						string colorStr = slotData[1];
						currentColor = colorStr.ToInt();
						break;
					}
				}
			}
		}
		
		int r = (currentColor >> 16) & 0xFF;
		int g = (currentColor >> 8) & 0xFF;
		int b = currentColor & 0xFF;
		
		if (m_wEditBoxR)
			m_wEditBoxR.SetValue(r.ToString());
		if (m_wEditBoxG)
			m_wEditBoxG.SetValue(g.ToString());
		if (m_wEditBoxB)
			m_wEditBoxB.SetValue(b.ToString());
		if (m_wEditBoxBrightness)
			m_wEditBoxBrightness.SetValue("100");
		
		UpdateColorPreview(r, g, b);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnColorChanged(SCR_EditBoxComponent editBox)
	{
		if (!m_wEditBoxR || !m_wEditBoxG || !m_wEditBoxB || !m_wEditBoxBrightness)
			return;
		
		int r = Math.ClampInt(m_wEditBoxR.GetValue().ToInt(), 0, 255);
		int g = Math.ClampInt(m_wEditBoxG.GetValue().ToInt(), 0, 255);
		int b = Math.ClampInt(m_wEditBoxB.GetValue().ToInt(), 0, 255);
		float brightness = Math.Clamp(m_wEditBoxBrightness.GetValue().ToFloat(), 0, 200) / 100.0;

		int finalR = Math.ClampInt((r * brightness), 0, 255);
		int finalG = Math.ClampInt((g * brightness), 0, 255);
		int finalB = Math.ClampInt((b * brightness), 0, 255);

		UpdateColorPreview(finalR, finalG, finalB);
		ApplyColor(finalR, finalG, finalB);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateColorPreview(int r, int g, int b)
	{
		if (!m_wColorPreview)
			return;
		
		float normalizedR = r / 255.0;
		float normalizedG = g / 255.0;
		float normalizedB = b / 255.0;
		
		m_wColorPreview.SetColor(Color.FromRGBA(normalizedR, normalizedG, normalizedB, 1.0));
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ApplyColor(int r, int g, int b)
	{
		if (!m_VehicleModsComponent || m_sSelectedSlot.IsEmpty())
			return;
		
		int argbColor = (255 << 24) | (r << 16) | (g << 8) | b;
		if (m_bIsBodySelected)
		{
			m_VehicleModsComponent.SetBodyColorPreview(argbColor);
		}
		else
		{
			m_VehicleModsComponent.SetSlotColorPreview(m_sSelectedSlot, argbColor);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void StoreOriginalColors()
	{
		if (!m_VehicleModsComponent || !m_Vehicle)
			return;
		
		m_mOriginalColors = new map<string, int>();
		
		ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(m_Vehicle.FindComponent(ParametricMaterialInstanceComponent));
		if (matComp)
			m_iOriginalBodyColor = matComp.GetColor();
		else
			m_iOriginalBodyColor = 0xFFFFFFFF;
		
		array<ref array<string>> slotsColorData = m_VehicleModsComponent.GetSlotsColor();
		if (slotsColorData)
		{
			foreach (array<string> slotData : slotsColorData)
			{
				if (slotData && slotData.Count() >= 2)
				{
					string slotName = slotData[0];
					int color = slotData[1].ToInt();
					m_mOriginalColors.Set(slotName, color);
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RestoreOriginalColors()
	{
		if (!m_VehicleModsComponent || !m_mOriginalColors)
			return;
		
		m_VehicleModsComponent.SetBodyColorPreview(m_iOriginalBodyColor);
		foreach (string slotName, int originalColor : m_mOriginalColors)
		{
			m_VehicleModsComponent.SetSlotColorPreview(slotName, originalColor);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnSetColorsPressed()
	{
		if (!m_SprayCanGadget)
		{
			CloseMenu();
			return;
		}
		
		m_SprayCanGadget.ApplyAllCurrentColors();
		CloseMenu();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		RestoreOriginalColors();
		super.OnMenuClose();
		GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, CloseMenu);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CloseMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void InitializeSlotSettings()
	{
		m_mSlotSettings = new map<string, ref array<float>>();
		m_aBodySettings = {255, 255, 255, 100};
	}
	
	//------------------------------------------------------------------------------------------------
	protected array<float> GetCurrentSlotSettings()
	{
		if (m_bIsBodySelected)
			return m_aBodySettings;
		
		if (!m_mSlotSettings || m_sSelectedSlot.IsEmpty())
			return null;
			
		return m_mSlotSettings.Get(m_sSelectedSlot);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SaveCurrentSlotSettings()
	{
		if (!m_wEditBoxR || !m_wEditBoxG || !m_wEditBoxB || !m_wEditBoxBrightness)
			return;
		
		float r = m_wEditBoxR.GetValue().ToFloat();
		float g = m_wEditBoxG.GetValue().ToFloat();
		float b = m_wEditBoxB.GetValue().ToFloat();
		float brightness = m_wEditBoxBrightness.GetValue().ToFloat();
		
		array<float> settings = {r, g, b, brightness};
		
		if (m_bIsBodySelected)
		{
			m_aBodySettings = settings;
		}
		else if (!m_sSelectedSlot.IsEmpty() && m_mSlotSettings)
		{
			m_mSlotSettings.Set(m_sSelectedSlot, settings);
		}
	}
}
