enum ELicenseType
{
	LICENSE_DRIVING = 1,
	LICENSE_GUNS = 2,
	LICENSE_IRON_PROCESSING = 3,
	LICENSE_COPPER_PROCESSING = 4,
	LICENSE_DIAMOND_PROCESSING = 5,
	LICENSE_SAND_PROCESSING = 6,
	LICENSE_REBEL = 7,
	LICENSE_FISHING = 8,
	LICENSE_COTTON = 9
};


string GetLicenseName(ELicenseType licenseType)
{
    switch (licenseType)
    {
        case ELicenseType.LICENSE_DRIVING:
            return "Driving License";
        case ELicenseType.LICENSE_GUNS:
            return "Gun License";
        case ELicenseType.LICENSE_IRON_PROCESSING:
            return "Iron Processing";
		case ELicenseType.LICENSE_COPPER_PROCESSING:
            return "Copper Processing";
		case ELicenseType.LICENSE_DIAMOND_PROCESSING:
            return "Diamond Processing";
		case ELicenseType.LICENSE_SAND_PROCESSING:
            return "Sand Processing";
		case ELicenseType.LICENSE_REBEL:
            return "Black Market";
		case ELicenseType.LICENSE_FISHING:
            return "Fishing License";
		case ELicenseType.LICENSE_COTTON:
			return "Cotton License";
        default:
            return "Unknown";
    }
	
	return "Unknown";
}