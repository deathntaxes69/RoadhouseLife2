class RL_GangMember : JsonApiStruct
{
    protected int id;
    protected string uid;
    protected string name;
    protected int gangId;
    protected int gangRank;
    
    int GetCharacterId()
    {
        return id;
    }
    
    string GetUid()
    {
        return uid;
    }
    
    string GetName()
    {
        return name;
    }
    
    int GetGangId()
    {
        return gangId;
    }
    
    int GetGangRank()
    {
        return gangRank;
    }
}

class RL_GangData : JsonApiStruct
{
    protected int id;
    protected string owner_uid;
    protected string name;
    protected int bank;
    protected int freq;
    protected int gang_exp;
    protected ref array<ref RL_GangMember> members;

    int GetGangId()
    {
        return id;
    }
    string GetOwnerUid()
    {
        return owner_uid;
    }
    string GetName()
    {
        return name;
    }
    int GetBank()
    {
        return bank;
    }
    int GetFreq()
    {
        return freq;
    }
    int GetGangExp()
    {
        return gang_exp;
    }
    int GetCount()
    {
        if (!members)
            return 0;

        return members.Count();
    }
    array<ref RL_GangMember> GetMembers()
    {
        return members;
    }
}