sealed class RL_SkillsWidget : RL_AppBaseWidget
{
    protected Widget m_contentLayout;
    private const string m_skillEntryLayout = "{EEB867E2AD07E4B4}UI/Layouts/Phone/Apps/Skills/SkillEntryWidget.layout";

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
        if(!m_wRoot || !m_character) return;
        
        m_contentLayout = Widget.Cast(m_wRoot.FindAnyWidget("contentLayout"));
        if(!m_contentLayout)
            return;

        m_character.GetAllSkills(this, "FillSkillsList");
		
	}
    void FillSkillsList(bool success, array<ref RL_SkillData> skills)
    {
        if(!m_contentLayout)
            return;

        array<int> skillCategories = {};
        SCR_Enum.GetEnumValues(RL_SkillsCategory, skillCategories);
        foreach(int categoryId : skillCategories)
        {
            Print("[RL_SkillsWidget] FillSkillsList foreach");
            if(categoryId == RL_SkillsCategory.POLICE && !m_character.IsPolice())
                continue;
                
            if(categoryId == RL_SkillsCategory.EMS && !m_character.IsMedic())
                continue;

            int currentXp = 0;
            foreach(RL_SkillData skill : skills) {
                Print("[RL_SkillsWidget] FillSkillsList foreach 2");
                if(skill.GetCategory() == categoryId)
                {
                    currentXp = skill.GetAmount();
                    break;
                }
            }

            Widget newWidget = GetGame().GetWorkspace().CreateWidgets(m_skillEntryLayout, m_contentLayout);
            RL_SkillsEntryWidget skillWidget = RL_SkillsEntryWidget.Cast(newWidget.FindHandler(RL_SkillsEntryWidget));

            //string categoryName = SCR_Enum.GetEnumName(RL_SkillsCategory, categoryId);
            string categoryName = RL_SkillsUtils.CategoryIdToName(categoryId);
            skillWidget.SetInfo(categoryId, currentXp
            );
        }

        
    }
}