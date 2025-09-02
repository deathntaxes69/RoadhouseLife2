class RL_WarrantData : JsonApiStruct
{
    protected int id = 0;
    protected string characterName;
    protected int characterId;
    protected int crimeId;
    protected string issuerName;

    string GetCharacterName()
    {
        return characterName;
    }
    int GetCharacterId()
    {
        return characterId;
    }
    int GetCrimeId()
    {
        return crimeId;
    }
    string GetIssuerName()
    {
        return issuerName;
    }
	int GetWarrantId()
	{
		return id;
	}
}