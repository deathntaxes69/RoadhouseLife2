[BaseContainerProps(configRoot: true)] 
sealed class RL_CrimeConfig 
{ 
    [Attribute()] 
    private ref array<ref RL_CrimeData> m_crimeList; 
    ref array<ref RL_CrimeData> GetCrimeList()
    {
        return m_crimeList;
    } 
} 

[BaseContainerProps(configRoot: false)] 
sealed class RL_CrimeData { 
    
    [Attribute(defvalue: "0")] 
    private int m_id; 
    [Attribute(defvalue: "")] 
    private string m_title; 

    int GetCrimeId()
    {
        return m_id;
    }; 
    string GetTitle() {
        return m_title;
    }; 

} 

