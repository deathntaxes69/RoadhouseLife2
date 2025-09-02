enum EGangRank
{
	RECRUIT = 1,
	MEMBER = 2,
	CAPTAIN = 3,
	BOSS = 4
};

string GetGangRankName(EGangRank gangRank)
{
    switch (gangRank)
    {
        case EGangRank.RECRUIT:
            return "Recruit";
        case EGangRank.MEMBER:
            return "Member";
        case EGangRank.CAPTAIN:
            return "Captain";
        case EGangRank.BOSS:
            return "Boss";
        default:
            return "Unknown";
    }
	
	return "Unknown";
} 