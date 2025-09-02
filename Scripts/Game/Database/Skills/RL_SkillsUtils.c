enum RL_SkillsCategory
{
	MINING = 1,
	FARMING = 2,
	CRAFTING = 3,
	DRUG_MANUFACTURING = 4,
	DRUG_DEALING = 5,
	CHARACTER = 6,
	POLICE = 7,
	EMS = 8
};
class RL_SkillsUtils
{

	protected static float m_fLevelMultiplier = 1000.0;
	
	// XP needed for a specific level, if you change this make sure to change it in characters.rs
	static float GetXPForLevel(int level)
	{
		return m_fLevelMultiplier * Math.Pow(level, 1.25);
	}

	// Level based on current XP, if you change this make sure to change it in characters.rs
	static int GetLevelFromXP(float xp)
	{
		return Math.Floor(Math.Pow(xp / m_fLevelMultiplier, 0.8));
	}
	// Progress to next level based on current xp (0.5 for 50%)
	static float GetProgressToNextLevel(float xp)
	{
		int currentLevel = GetLevelFromXP(xp);
		float xpCurrent = GetXPForLevel(currentLevel);
		float xpNext = GetXPForLevel(currentLevel + 1);

		if (xpNext == xpCurrent) // avoid division by zero (shouldn't happen, but safety)
			return 0.0;

		return Math.Clamp((xp - xpCurrent) / (xpNext - xpCurrent), 0.0, 1.0);
	}
	// Convert multi word enum to pretty name
	// There might be a base game function for this already
	static string CategoryIdToName(int categoryId)
	{
		string categoryName = "";
		string enumName = SCR_Enum.GetEnumName(RL_SkillsCategory, categoryId);
		array<string> strs = {};
		enumName.Split("_", strs, true);
 
		for ( int i = 0; i < strs.Count(); i++ )
		{
			Print("[RL_SkillsUtils] CategoryIdToName for");
			string word = strs.Get(i);
			string firstLetter = word[0];
			word.ToLower();
			firstLetter.ToUpper();
			
			// Add space if not first word
			if(i>0)
				categoryName = categoryName + " ";

			categoryName = categoryName + string.Format("%1%2", firstLetter, word.Substring(1, word.Length() - 1))

		}
		return categoryName;
	}

	static string GetSkillCategoryIcon(RL_SkillsCategory category)
	{
		switch(category)
		{
			case RL_SkillsCategory.MINING:
				return "MINING_PERK";
			case RL_SkillsCategory.FARMING:
				return "FARMING_PERK";
			case RL_SkillsCategory.CRAFTING:
				return "CRAFTING_PERK";
			case RL_SkillsCategory.DRUG_MANUFACTURING:
				return "MANUFACTURE_PERK";
			case RL_SkillsCategory.DRUG_DEALING:
				return "DEAL_PERK";
			case RL_SkillsCategory.CHARACTER:
				return "SETTINGS";
			case RL_SkillsCategory.POLICE:
				return "911";
			case RL_SkillsCategory.EMS:
				return "911";
			default:
				return "MARKET";
		}
		
		return "MARKET";
	}

}