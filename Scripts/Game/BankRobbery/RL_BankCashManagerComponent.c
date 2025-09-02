[EntityEditorProps(category: "GameScripted/Components", description: "Manages bank cash drawer states")]
class RL_BankCashManagerComponentClass : ScriptComponentClass
{
}

class RL_BankCashManagerComponent : ScriptComponent
{
	protected ref array<string> m_aRobbedDrawers = {};
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		Print("[RL_BankCashManagerComponent] Component initialized");
		
		if (!m_aRobbedDrawers)
			m_aRobbedDrawers = {};
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsDrawerRobbed(string drawerContext)
	{
		if (!m_aRobbedDrawers)
			return false;
			
		return m_aRobbedDrawers.Contains(drawerContext);
	}
	
	//------------------------------------------------------------------------------------------------
	void RobDrawer(string drawerContext)
	{
		if (!Replication.IsServer())
			return;
		
		if (!m_aRobbedDrawers)
			m_aRobbedDrawers = {};
			
		if (!m_aRobbedDrawers.Contains(drawerContext))
		{
			m_aRobbedDrawers.Insert(drawerContext);
			SyncDrawerState(drawerContext, true);
			Print(string.Format("[RL_BankCashManagerComponent] Drawer %1 marked as robbed", drawerContext));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAllDrawers()
	{
		if (!Replication.IsServer())
			return;
		
		if (!m_aRobbedDrawers)
			m_aRobbedDrawers = {};
		else
			m_aRobbedDrawers.Clear();
		
		SyncAllDrawersReset();
		Print("[RL_BankCashManagerComponent] All drawers reset");
	}
	
	//------------------------------------------------------------------------------------------------
	int GetRobbedDrawerCount()
	{
		if (!m_aRobbedDrawers)
			return 0;
			
		return m_aRobbedDrawers.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	array<string> GetRobbedDrawers()
	{
		if (!m_aRobbedDrawers)
			return {};
			
		return m_aRobbedDrawers;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SyncDrawerState(string drawerContext, bool isRobbed)
	{
		Rpc(RpcDo_SyncDrawerState, drawerContext, isRobbed);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_SyncDrawerState(string drawerContext, bool isRobbed)
	{
		Print("RpcDo_SyncDrawerState");
		if (!m_aRobbedDrawers)
			m_aRobbedDrawers = {};
		
		if (isRobbed && !m_aRobbedDrawers.Contains(drawerContext))
		{
			m_aRobbedDrawers.Insert(drawerContext);
		}
		else if (!isRobbed && m_aRobbedDrawers.Contains(drawerContext))
		{
			m_aRobbedDrawers.RemoveItem(drawerContext);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SyncAllDrawersReset()
	{
		Rpc(RpcDo_SyncAllDrawersReset);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_SyncAllDrawersReset()
	{
		Print("RpcDo_SyncAllDrawersReset");
		if (!m_aRobbedDrawers)
			m_aRobbedDrawers = {};
		else
			m_aRobbedDrawers.Clear();
	}
} 