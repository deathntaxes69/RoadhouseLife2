
void RL_ProgressBarWidget_OnComplete(Widget progressWidget);
typedef func RL_ProgressBarWidget_OnComplete;

sealed class RL_ProgressBarWidget : SCR_ScriptedWidgetComponent
{


	TextWidget m_wTitle;
	ProgressBarWidget m_wProgressBar;

    string m_sTitleText = "";

    float m_fUpdateInterval = 200;
    float m_fCurrentProgress = 0.0;
    int m_iSteps;
    int m_iCurrentStep = 0;
    float m_fIncrementPerStep;

    protected ref ScriptInvokerBase<RL_ProgressBarWidget_OnComplete> m_OnComplete;

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_wTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("title"));
		m_wProgressBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("progressBar"));

        
		
	}
    void InitProgressBar(string titleText, int totalMilliseconds)
    {
        m_sTitleText = titleText;
		m_iSteps = Math.Ceil(totalMilliseconds / m_fUpdateInterval);
        m_fIncrementPerStep = 100 / m_iSteps;


        GetGame().GetCallqueue().CallLater(IncrementProgressBar, m_fUpdateInterval, true);
        GetGame().GetCallqueue().CallLater(OnComplete, totalMilliseconds);

    }
    void IncrementProgressBar()
    {
        if(!m_wProgressBar || !m_wTitle) 
        {
            //AnimateWidget.Opacity(m_wRoot, false, UIConstants.FADE_RATE_SLOW);
            OnComplete();
            return;
        }

        m_iCurrentStep = m_iCurrentStep + 1;

        m_fCurrentProgress += m_fIncrementPerStep;
        m_wProgressBar.SetCurrent(m_fCurrentProgress);
        m_wTitle.SetText(m_sTitleText+" ("+(Math.Ceil(m_fCurrentProgress).ToString())+"%)");
        
        //GetGame().GetCallqueue().CallLater(IncrementProgressBar, m_fUpdateInterval);
    }
    bool IsComplete()
    {
        if(m_fCurrentProgress >= 100.0)
            return true;
        
        return false;

    }
    protected void OnComplete()
	{
        GetGame().GetCallqueue().Remove(IncrementProgressBar);
		if (m_OnComplete)
			m_OnComplete.Invoke(m_wRoot);
	}
	ScriptInvokerBase<RL_ProgressBarWidget_OnComplete> GetOnComplete()
	{
		if (!m_OnComplete)
			m_OnComplete = new ScriptInvokerBase<RL_ProgressBarWidget_OnComplete>();
		
		return m_OnComplete;
	}
	override void HandlerDeattached(Widget w)
	{
        GetGame().GetCallqueue().Remove(IncrementProgressBar);
	}


}