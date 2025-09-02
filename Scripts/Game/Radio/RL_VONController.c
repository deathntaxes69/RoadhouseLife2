modded class SCR_VONController
{
    /*
    override int GetVONEntries(inout array<ref SCR_VONEntry> entries)
	{
		int count = m_aEntries.Count();
		for (int i = 0; i < count; i++)
		{
            if(i == GetPhoneEntryIndex())
                continue;

			entries.Insert(m_aEntries[i]);
		}

		return count;
	}
    */
    // Phone channel will always be the last one
    SCR_VONEntryRadio GetPhoneEntry()
    {
        int phoneIndex = GetPhoneEntryIndex();
        Print(phoneIndex);
        return SCR_VONEntryRadio.Cast(m_aEntries[GetPhoneEntryIndex()]);
    } 
    int GetPhoneEntryIndex()
    {
        int count = m_aEntries.Count();
        return (count-1);
    }
    void SetChannelActive(int channel)
    {
        if (!m_aEntries || channel >= m_aEntries.Count())
        {
            Print("SetChannelActive Out of bounds!spoajfoesinfiojunsdf");
            return;
        }
        SetActiveTransmit(m_aEntries[channel]);
    }
}