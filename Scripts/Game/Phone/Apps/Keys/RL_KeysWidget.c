sealed class RL_KeysWidget : RL_AppBaseWidget
{
	protected Widget m_myKeysContainer;
	protected SCR_ListBoxComponent m_myKeysListBox;
	protected Widget m_charactersNearbyContainer;
	protected SCR_ListBoxComponent m_charactersNearbyListBox;
	protected SCR_InputButtonComponent m_giveKeysButton;

	protected ref array<ref RL_VehicleKeyData> m_myVehicleKeys;
	protected ref array<SCR_ChimeraCharacter> m_nearbyCharacters;
	protected RL_VehicleKeysComponent m_keysComp;

	override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		if (!m_character) 
			return;

		m_keysComp = RL_VehicleKeysComponent.Cast(m_character.FindComponent(RL_VehicleKeysComponent));
		if(!m_keysComp) return;
		
		m_myKeysContainer = Widget.Cast(m_wRoot.FindAnyWidget("MyKeys"));
		if (!m_myKeysContainer) 
			return;
		m_myKeysListBox = SCR_ListBoxComponent.Cast(m_myKeysContainer.FindHandler(SCR_ListBoxComponent));

		m_charactersNearbyContainer = Widget.Cast(m_wRoot.FindAnyWidget("CharactersNearby"));
		if (!m_charactersNearbyContainer) 
			return;
		m_charactersNearbyListBox = SCR_ListBoxComponent.Cast(m_charactersNearbyContainer.FindHandler(SCR_ListBoxComponent));

		m_giveKeysButton = SCR_InputButtonComponent.GetInputButtonComponent("GiveKeys", m_wRoot);
		if (!m_giveKeysButton) 
			return;
		m_giveKeysButton.m_OnActivated.Insert(OnGiveKeys);

		RefreshMyKeys();
		RefreshNearbyCharacters();
	}

	void RefreshMyKeys()
	{
		if (!m_myKeysListBox || !m_character)
			return;

		m_myKeysListBox.Clear();
		m_myVehicleKeys = m_keysComp.GetOwnedVehicleKeys();

		if (!m_myVehicleKeys)
		{
			m_myKeysListBox.AddItem("No vehicle keys owned");
			return;
		}

		foreach (RL_VehicleKeyData vehicleKey : m_myVehicleKeys)
		{
			Print("[RL_KeysWidget] RefreshMyKeys foreach");
			if (!vehicleKey) 
				continue;
				
			string vehicleName = vehicleKey.GetVehicleName();
			if (vehicleName.Contains("/"))
			{
				array<string> pathParts = {};
				vehicleName.Split("/", pathParts, true);
				if (pathParts.Count() > 0)
					vehicleName = pathParts[pathParts.Count() - 1];
			}
			if (vehicleName.Contains(".et"))
				vehicleName.Replace(".et", "");

			m_myKeysListBox.AddItem(vehicleName);
		}

		if (m_myVehicleKeys.Count() > 0)
		{
			m_myKeysListBox.SetItemSelected(0, true);
		}
	}

	void RefreshNearbyCharacters()
	{
		if (!m_charactersNearbyListBox || !m_character)
			return;

		m_charactersNearbyListBox.Clear();
		m_nearbyCharacters = {};

		array<IEntity> nearbyEntities = RL_Utils.GetNearbyCharacters(m_character.GetOrigin(), 5.0);
		
		foreach (IEntity entity : nearbyEntities)
		{
			Print("[RL_KeysWidget] RefreshNearbyCharacters foreach");
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
			if (!character || character == m_character)
				continue;

			m_nearbyCharacters.Insert(character);
			string characterName = character.GetCharacterName();
			if (characterName.IsEmpty())
				characterName = "Unknown Character";
				
			m_charactersNearbyListBox.AddItem(characterName);
		}
	}

	void OnGiveKeys(SCR_InputButtonComponent btn)
	{
		if (!m_character || m_character.IsSpamming())
			return;

		int selectedKeyIndex = m_myKeysListBox.GetSelectedItem();
		if (selectedKeyIndex == -1 || !m_myVehicleKeys || selectedKeyIndex >= m_myVehicleKeys.Count())
		{
			m_character.Notify("Please select a vehicle key to give", "VEHICLE KEYS");
			return;
		}

		int selectedCharacterIndex = m_charactersNearbyListBox.GetSelectedItem();
		if (selectedCharacterIndex == -1 || !m_nearbyCharacters || selectedCharacterIndex >= m_nearbyCharacters.Count())
		{
			m_character.Notify("Please select a character to give the key to", "VEHICLE KEYS");
			return;
		}

		RL_VehicleKeyData selectedKey = m_myVehicleKeys[selectedKeyIndex];
		SCR_ChimeraCharacter targetCharacter = m_nearbyCharacters[selectedCharacterIndex];

		if (!selectedKey || !targetCharacter)
		{
			m_character.Notify("Invalid selection", "VEHICLE KEYS");
			return;
		}

		m_keysComp.GiveVehicleKeyToCharacter(targetCharacter.GetCharacterId(), selectedKey.GetVehicleId());
		RefreshMyKeys();
		RefreshNearbyCharacters();
	}

	void RefreshAllData()
	{
		RefreshMyKeys();
		RefreshNearbyCharacters();
	}
}