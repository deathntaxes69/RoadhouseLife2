modded class SCR_ChimeraCharacter
{
    protected bool m_EarplugsIn = false;

    void ToggleEarplugs()
    {
        m_EarplugsIn = !m_EarplugsIn;
    }
    bool IsEarplugged()
    {
        return m_EarplugsIn;
    }


}