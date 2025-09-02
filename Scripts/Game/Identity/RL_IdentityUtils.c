class RL_IdentityUtils
{
	//------------------------------------------------------------------------------------------------
	static void ExtractIdentityInfo(SCR_ChimeraCharacter character, out string firstName, out string lastName, out string birthDate, out array<string> licenses, out int policeRank)
	{
		// Extract and split character name
		array<string> characterNameSplit = {};
		string characterName = character.GetCharacterName();
		characterName.Split(" ", characterNameSplit, false);
		
		firstName = characterNameSplit.Get(0);
		lastName = "";
		if (characterNameSplit.Count() >= 2)
		{
		    lastName = characterNameSplit.Get(1);
		}
		
		// Calculate birth date
		int age = character.GetAge();
		int birthYear = 2025 - age;
		int birthMonth = 4;
		int birthDay = 20;
		birthDate = string.Format("%1/%2/%3", birthMonth, birthDay, birthYear);
		
		// Extract and parse licenses
		string licenseString = character.GetLicenses();
		array<string> licenseListStrings = {};
		licenseString.Split(",", licenseListStrings, true);
		licenses = {};

		foreach (string str : licenseListStrings)
		{
			Print("[RL_IdentityUtils] ExtractIdentityInfo foreach loop");
		    int licenseNum = str.ToInt();
			string licenseName = GetLicenseName(licenseNum);
			if (licenseName != "")
			{
				licenses.Insert(licenseName);
			}
		}
		
		policeRank = character.GetPoliceRank();
	}
	
	//------------------------------------------------------------------------------------------------
	static void ShowIdentityToUser(SCR_ChimeraCharacter targetCharacter, SCR_ChimeraCharacter userCharacter)
	{
		string firstName, lastName, birthDate;
		array<string> licenses;
		int policeRank;
		
		ExtractIdentityInfo(targetCharacter, firstName, lastName, birthDate, licenses, policeRank);
		userCharacter.ShowIdentity(firstName, lastName, birthDate, licenses, policeRank);
	}
} 