class RL_PerksUtils
{
	protected const int PERK_BASE_RESET_COST = 1000;
	protected const int PERK_RESET_COST_PER_PERK = 1000;
    	
	//------------------------------------------------------------------------------------------------
    static int GetResetCostForCategory(RL_SkillsCategory category, int spentPoints)
    {
        return PERK_BASE_RESET_COST + (spentPoints * PERK_RESET_COST_PER_PERK);
    }
}