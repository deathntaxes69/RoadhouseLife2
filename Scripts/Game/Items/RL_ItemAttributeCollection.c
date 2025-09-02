modded class SCR_ItemAttributeCollection
{
	[Attribute(defvalue: "0", desc: "Allow item to be looted from dead bodies")]
	protected bool whiteListedToLoot;
	
	bool GetIsWhitelisted() 
	{
		return whiteListedToLoot;
	}
}