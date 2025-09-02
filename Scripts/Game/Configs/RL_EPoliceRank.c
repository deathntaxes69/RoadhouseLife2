enum EPoliceRank
{
	CADET = 1,
	CAPTAIN = 2,
	CHIEF = 3
};


Tuple2<string, string> GetPoliceRankName(EPoliceRank whitelistNumber)
{
	
	private const string sheriffBadge = "{58BC509B53BB4B40}UI/Layouts/Identity/Assets/sherrif_badge.edds";
	
    switch (whitelistNumber)
    {
        case EPoliceRank.CADET:
			return new Tuple2<string, string>("Cadet", sheriffBadge);
        case EPoliceRank.CAPTAIN:
			return new Tuple2<string, string>("Captain", sheriffBadge);
        case EPoliceRank.CHIEF:
			return new Tuple2<string, string>("Chief", sheriffBadge);
        default:
            return new Tuple2<string, string>("", sheriffBadge);
    }
	
	return new Tuple2<string, string>("", sheriffBadge);
}