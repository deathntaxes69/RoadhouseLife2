sealed class RL_SkillsEntryWidget : ScriptedWidgetComponent
{
    protected TextWidget m_wTitle;
    protected TextWidget m_wXpAmount;
    protected TextWidget m_wLevelNumber;
    protected SCR_RadialProgressBarUIComponent m_RadialProgressUI;


    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

        m_wTitle = TextWidget.Cast(w.FindAnyWidget("title"));
        m_wXpAmount = TextWidget.Cast(w.FindAnyWidget("xpAmount"));
        m_wLevelNumber = TextWidget.Cast(w.FindAnyWidget("levelNumber"));

        m_RadialProgressUI = SCR_RadialProgressBarUIComponent.Cast(SCR_WidgetTools.FindHandlerInChildren(w, SCR_RadialProgressBarUIComponent));
        //m_contactsListBox = SCR_ListBoxComponent.Cast(contactsListWidget.FindHandler(SCR_ListBoxComponent));

		
	}
    void SetInfo(int categoryId, int currentXp)
    {
       m_wTitle.SetText(RL_SkillsUtils.CategoryIdToName(categoryId));
       m_wXpAmount.SetText(string.Format("%1xp", currentXp));
       m_wLevelNumber.SetText(string.Format("%1", RL_SkillsUtils.GetLevelFromXP(currentXp)));
       m_RadialProgressUI.SetProgress(RL_SkillsUtils.GetProgressToNextLevel(currentXp));
        
    }
}