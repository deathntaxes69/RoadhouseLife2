class RL_ProgressBarAction : ScriptedUserAction
{

    [Attribute(defvalue:"5000", desc: "Amount of time in milliseconds. This is multiplied for processing")]
	protected float m_fProgressTime;

    [Attribute(defvalue:"3.0", desc: "Max distance (meters) they can move after starting")]
	protected float m_fMaxMoveDistance;

    [Attribute(defvalue:"Processing Dog", desc: "Text above the progress bar")]
	protected string m_sProgressText;

    protected SCR_ChimeraCharacter m_progressCharacter;
    protected bool m_bProgressActive = false;
    protected RL_Hud m_playerHud;
    protected Widget m_progressBar;

    bool IsProgressBarActive()
    {
        return m_bProgressActive;
    }
    float GetProgressTime()
    {
        Print("GetProgressTime orginal");
        return m_fProgressTime;
    }
    float GetMaxMoveDistance()
    {
        return m_fMaxMoveDistance;
    }
    Widget StartProgressBar(SCR_ChimeraCharacter character)
    {
        m_bProgressActive = true;
        m_progressCharacter = character;
        m_playerHud = RL_Hud.GetCurrentInstance();
        Print("CreateActionProgressBar");
        float progressTime = GetProgressTime(); 
        Print(progressTime);
        m_progressBar =  m_playerHud.CreateActionProgressBar(string.Format("%1", m_sProgressText), progressTime);
        return m_progressBar;
    }
    void EndProgressBar()
    {
        m_bProgressActive = false;
        if(m_playerHud && m_progressBar)
        {
			m_playerHud.OnProgressBarComplete(m_progressBar);
        } else {
            Print("EndProgress bar missing m_playerHud && m_progressBar");
        }

    }




}