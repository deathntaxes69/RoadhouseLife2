
modded class SCR_VoterComponent
{
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override protected void VoteServer(EVotingType type, int value)
	{
		return;
	}
}
