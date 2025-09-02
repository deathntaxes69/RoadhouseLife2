[EntityEditorProps(category: "NarcosLife/Core", description: "Core gamemode")]
class RL_GameModeRoleplayClass: SCR_BaseGameModeClass
{
}

class RL_GameModeRoleplay: SCR_BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	void ~RL_GameModeRoleplay()
	{
		Print("RL_GameModeRoleplay Other Delete");
		Debug.DumpStack();
	}
	void OnDelete()
	{

		Print("RL_GameModeRoleplay Delete");
		Debug.DumpStack();
		
	}
}
