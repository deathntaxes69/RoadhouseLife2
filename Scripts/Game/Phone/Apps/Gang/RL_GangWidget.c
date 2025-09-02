sealed class RL_GangWidget : RL_AppBaseWidget
{
	protected SCR_ListBoxComponent m_memberListBox;
	protected SCR_ComboBoxComponent m_contactListCombo;
	protected SCR_ListBoxComponent m_recievedInviteList;
	protected SCR_InputButtonComponent m_kickButton;
	protected SCR_InputButtonComponent m_demoteButton;
	protected SCR_InputButtonComponent m_promoteButton;
	
	protected ref array<int> m_playerList;
	protected ref array<string> m_nameList;
	protected RL_GangMember m_selectedGangMember;
	protected RL_GangData m_GangData;
	
	private const string m_sGangName = "gangName";
	private const string m_sGangCount = "gangCount";
	private const string m_sGangBankExp = "gangBankExp";
	private const string m_sMemberList = "gangList";
	private const string m_sLeaveButton = "LeaveButton";
	private const string m_sContactListHLayout = "inviteHLayout";
	private const string m_sCreateGangHLayout = "createHLayout";
	private const string m_sCreateGangButton = "createGangButton";
	private const string m_sContactListCombo = "invitePlayer";
	private const string m_sContactListInviteButton = "invitePlayerButton";
	private const string m_sGangNameEditBox = "GangNameEditBox";
	private const string m_sKickButton = "Kick";
	private const string m_sDemoteButton = "DemoteButton";
	private const string m_sPromoteButton = "PromoteButton";
	private const string m_sRecievedInvitesList = "inviteList";
	// data could be stale for gang limit if you invite someone then someone else does without reopening their ui
 	private const int GANG_MAX_MEMBERS = 30;

	override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		if (!InitializeUIComponents())
			return;
		
		SetupEventHandlers();
		InitializeData();
	}
	
	protected bool InitializeUIComponents()
	{
		Widget memberListWidget = Widget.Cast(m_wRoot.FindAnyWidget(m_sMemberList));
		if (!memberListWidget)
			return false;
		m_memberListBox = SCR_ListBoxComponent.Cast(memberListWidget.FindHandler(SCR_ListBoxComponent));
		
		Widget contactListWidget = Widget.Cast(m_wRoot.FindAnyWidget(m_sContactListCombo));
		if (!contactListWidget)
			return false;
		m_contactListCombo = SCR_ComboBoxComponent.Cast(contactListWidget.FindHandler(SCR_ComboBoxComponent));
		
		Widget widgetRecievedInvitesList = Widget.Cast(m_wRoot.FindAnyWidget(m_sRecievedInvitesList));
		if (!widgetRecievedInvitesList)
			return false;
		m_recievedInviteList = SCR_ListBoxComponent.Cast(widgetRecievedInvitesList.FindHandler(SCR_ListBoxComponent));
		
		m_kickButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sKickButton, m_wRoot);
		m_demoteButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sDemoteButton, m_wRoot);
		m_promoteButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sPromoteButton, m_wRoot);
		
		return true;
	}
	
	protected void SetupEventHandlers()
	{
		if (m_memberListBox)
			m_memberListBox.m_OnChanged.Insert(OnMemberSelected);
		
		SCR_InputButtonComponent leaveButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sLeaveButton, m_wRoot);
		if (leaveButton)
			leaveButton.m_OnActivated.Insert(OnMemberLeave);
		
		SCR_InputButtonComponent contactListInviteButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sContactListInviteButton, m_wRoot);
		if (contactListInviteButton)
			contactListInviteButton.m_OnActivated.Insert(OnContactInvite);
		
		SCR_InputButtonComponent createGangButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sCreateGangButton, m_wRoot);
		if (createGangButton)
			createGangButton.m_OnActivated.Insert(OnCreateGang);
		
		if (m_kickButton)
			m_kickButton.m_OnActivated.Insert(OnKickMember);
		
		if (m_demoteButton)
			m_demoteButton.m_OnActivated.Insert(OnDemoteMember);
		
		if (m_promoteButton)
			m_promoteButton.m_OnActivated.Insert(OnPromoteMember);
	}
	
	protected void InitializeData()
	{
		UpdateMemberActionButtonsVisibility(false);
		
		if (m_character)
		{
			RefreshGangData();
			RefreshContactList();
			RefreshInvitesList();
		}
	}
	
	protected void RefreshGangData()
	{
		if (m_character)
		{
			int gangId = m_character.GetGangId();
			if (gangId > 0)
			{
				m_character.GetGang(this);
			}
			else
			{
				m_character.SetGangWidget(this);
				UpdateGangData(true, false, null);
			}
		}
	}
	
	protected void RefreshContactList()
	{
		if (m_character)
		{
			Widget contactListWidget = Widget.Cast(m_wRoot.FindAnyWidget(m_sContactListCombo));
			if (contactListWidget)
			{
				m_character.GetServerPlayerList(contactListWidget, "FillContactList");
			}
		}
	}
	
	void RefreshInvitesList()
	{
		if (!m_recievedInviteList)
			return;
		
		m_recievedInviteList.Clear();
		
		if (!m_character)
			return;
		
		array<string> invites = m_character.GetInvites();
		foreach (string inviteData : invites)
		{
			Print("[RL_GangWidget] RefreshInvitesList foreach loop");
			array<string> data = {};
			inviteData.Split("|", data, false);
			
			if (data.Count() < 3)
				continue;
			
			string senderName = data[0];
			string gangName = data[1];
			string inviteText = string.Format("%1 has invited you to their gang", senderName);
			m_recievedInviteList.AddItem(inviteText, data);
		}
	}
	
	protected void RefreshAllUI()
	{
		RefreshGangData();
		RefreshContactList();
		RefreshInvitesList();
	}
	
	protected void OnCreateGang()
	{
		if (RL_Utils.GetLocalCharacter().IsSpamming())
			return;

		Widget widgetGangNameEditBox = Widget.Cast(m_wRoot.FindAnyWidget(m_sGangNameEditBox));
		if (!widgetGangNameEditBox)
			return;
		
		SCR_EditBoxComponent gangNameEditBox = SCR_EditBoxComponent.Cast(widgetGangNameEditBox.FindHandler(SCR_EditBoxComponent));
		if (!gangNameEditBox)
			return;
		
		string gangName = gangNameEditBox.GetValue();
		
		if (gangName.IsEmpty())
		{
			RL_Utils.Notify("Please enter a gang name.", "GANG");
			return;
		}
		
		if (gangName.Length() < 3)
		{
			RL_Utils.Notify("Gang name must be at least 3 characters long.", "GANG");
			return;
		}
		
		m_character.CreateGang(gangName);
		GetGame().GetCallqueue().CallLater(RefreshGangData, 500);
	}
	
	protected void OnMemberLeave(SCR_InputButtonComponent source)
	{
		if (m_character.IsSpamming())
			return;

		m_character.SetGangIdAndRank(m_character.GetCharacterId(), -1, 0);
		GetGame().GetCallqueue().CallLater(RefreshAllUI, 500);
	}
	
	void AcceptInvite(SCR_InputButtonComponent source)
	{
		if (m_character.IsSpamming())
			return;

		if (!m_recievedInviteList)
			return;
		
		int selectedRow = m_recievedInviteList.GetSelectedItem();
		if (selectedRow < 0)
		{
			RL_Utils.Notify("Please select an invite first.", "GANG");
			return;
		}
		
		array<string> data = array<string>.Cast(m_recievedInviteList.GetItemData(selectedRow));
		if (!data || data.Count() < 3)
		{
			RL_Utils.Notify("Invalid invite data.", "GANG");
			return;
		}
		
		int gangId = data[2].ToInt();
		string gangName = data[1];
		
		if (!m_character)
			return;
		
		m_character.SetGangIdAndRank(m_character.GetCharacterId(), gangId, 1);
		m_character.RemoveInvite(selectedRow, this, "OnInviteRemoved");
		RL_Utils.Notify(string.Format("Joined gang"), "GANG");
		
		RefreshInvitesList();
		GetGame().GetCallqueue().CallLater(RefreshAllUI, 300);
		GetGame().GetCallqueue().CallLater(RefreshAllUI, 1000);
	}
	
	void DeclineInvite(SCR_InputButtonComponent source)
	{
		if (!m_recievedInviteList)
			return;
		
		int selectedRow = m_recievedInviteList.GetSelectedItem();
		if (selectedRow < 0)
		{
			RL_Utils.Notify("Please select an invite first.", "GANG");
			return;
		}
		
		m_character.RemoveInvite(selectedRow, this, "OnInviteRemoved");
		RL_Utils.Notify("Invite declined.", "GANG");
		RefreshInvitesList();
	}
	
	void OnInviteRemoved(bool success)
	{
		if (success)
		{
			RefreshInvitesList();
		}
	}
	
	protected void OnContactInvite(SCR_InputButtonComponent source)
	{
		if (m_character.IsSpamming())
			return;

		if (!m_contactListCombo || !m_playerList)
			return;
		
		int selectedRow = m_contactListCombo.GetCurrentIndex();
		
		if (selectedRow < 0 || selectedRow >= m_playerList.Count())
		{
			RL_Utils.Notify("Please select a player to invite.", "GANG");
			return;
		}
		
		int gangId = m_character.GetGangId();
		if (gangId <= 0)
		{
			RL_Utils.Notify("You must be in a gang to invite players.", "GANG");
			return;
		}

		if (m_GangData && m_GangData.GetCount() >= GANG_MAX_MEMBERS)
		{
			RL_Utils.Notify(string.Format("Your gang already has the maximum of %1 members.", GANG_MAX_MEMBERS), "GANG");
			return;
		}
		
		int recipient = m_playerList[selectedRow];
		string gangName = "Your Gang";
		if (m_GangData && m_GangData.GetName())
			gangName = m_GangData.GetName();
		
		m_character.SendGangInvite(recipient, gangName, gangId);
		
		RL_Utils.Notify("Invite sent!", "GANG");
	}
	
	protected void OnMemberSelected(SCR_ListBoxComponent source, int item, bool selected)
	{
		if (!selected)
		{
			m_selectedGangMember = null;
			UpdateMemberActionButtonsVisibility(false);
			return;
		}
		
		RL_GangMember member = RL_GangMember.Cast(source.GetItemData(item));
		if (!member)
		{
			m_selectedGangMember = null;
			UpdateMemberActionButtonsVisibility(false);
			return;
		}
		
		m_selectedGangMember = member;
		UpdateMemberActionButtonsVisibility(true);
	}
	
	protected void UpdateMemberActionButtonsVisibility(bool visible)
	{
		if (m_kickButton && m_kickButton.GetRootWidget())
			m_kickButton.GetRootWidget().SetVisible(visible);
		
		if (m_demoteButton && m_demoteButton.GetRootWidget())
			m_demoteButton.GetRootWidget().SetVisible(visible);
		
		if (m_promoteButton && m_promoteButton.GetRootWidget())
			m_promoteButton.GetRootWidget().SetVisible(visible);
	}
	
	protected void OnKickMember()
	{
		MemberAction(2);
	}
	
	protected void OnDemoteMember()
	{
		MemberAction(1);
	}
	
	protected void OnPromoteMember()
	{
		MemberAction(0);
	}
	
	void MemberAction(int action)
	{
		if (RL_Utils.GetLocalCharacter().IsSpamming())
			return;

		if (!m_selectedGangMember)
		{
			RL_Utils.Notify("Please select a member first.", "GANG");
			return;
		}
		
		if (!ValidateMemberAction())
			return;
		
		switch (action)
		{
			case 0: // Promote
			{
				int newRank = m_selectedGangMember.GetGangRank() + 1;
				if (newRank > 4)
				{
					RL_Utils.Notify("Member is already max rank.", "GANG");
					return;
				}
					m_character.SetGangIdAndRank(m_selectedGangMember.GetCharacterId().ToString(), m_selectedGangMember.GetGangId(), newRank);

					SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
					jsonData.WriteValue("gangId", m_character.GetGangId());
					jsonData.WriteValue("gangMember", m_selectedGangMember.GetCharacterId());
					jsonData.WriteValue("newRank", newRank);

					m_character.PlayerLog("Gang Promote", jsonData.ExportToString());
				break;
			}
			case 1: // Demote
			{
				int newRank = m_selectedGangMember.GetGangRank() - 1;
				if (newRank < 0)
				{
					RL_Utils.Notify("Member is already lowest rank.", "GANG");
					return;
				}
					m_character.SetGangIdAndRank(m_selectedGangMember.GetCharacterId().ToString(), m_selectedGangMember.GetGangId(), newRank);

					SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
					jsonData.WriteValue("gangId", m_character.GetGangId());
					jsonData.WriteValue("gangMember", m_selectedGangMember.GetCharacterId());
					jsonData.WriteValue("newRank", newRank);

					m_character.PlayerLog("Gang Demote", jsonData.ExportToString());
				break;
			}
			case 2: // Kick
			{
					m_character.SetGangIdAndRank(m_selectedGangMember.GetCharacterId().ToString(), -1, 0);

					SCR_JsonSaveContext jsonData = new SCR_JsonSaveContext();
					jsonData.WriteValue("gangId", m_character.GetGangId());
					jsonData.WriteValue("kickedId", m_selectedGangMember.GetCharacterId());

					m_character.PlayerLog("Gang Kick", jsonData.ExportToString());
				break;
			}
		}
		
		GetGame().GetCallqueue().CallLater(RefreshGangData, 500);
	}
	
	protected bool ValidateMemberAction()
	{
		if (m_character.GetGangRank() < 4)
		{
			RL_Utils.Notify("You do not have permission to manage others.", "GANG");
			return false;
		}
			
		if (m_GangData && m_selectedGangMember.GetUid() == m_GangData.GetOwnerUid())
		{
			RL_Utils.Notify("You cannot modify the gang owner.", "GANG");
			return false;
		}
		
		if (m_character.GetCharacterId() == m_selectedGangMember.GetCharacterId().ToString())
		{
			RL_Utils.Notify("You cannot modify yourself.", "GANG");
			return false;
		}
		
		return true;
	}
	
	void UpdateGangData(bool success, bool hasGang, RL_GangData gangData)
	{
		if (!success)
			return;
		
		m_GangData = gangData;
		
		Widget contactListHLayout = Widget.Cast(m_wRoot.FindAnyWidget(m_sContactListHLayout));
		Widget createGangHLayout = Widget.Cast(m_wRoot.FindAnyWidget(m_sCreateGangHLayout));
		TextWidget gangNameWidget = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sGangName));
		TextWidget gangCountWidget = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sGangCount));
		TextWidget gangBankExpWidget = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sGangBankExp));
		
		if (!hasGang || !gangData)
		{
			UpdateUIForNoGang(contactListHLayout, createGangHLayout, gangNameWidget, gangCountWidget, gangBankExpWidget);
			return;
		}
		
		UpdateUIForGang(contactListHLayout, createGangHLayout, gangNameWidget, gangCountWidget, gangBankExpWidget, gangData);
		UpdateMemberList(gangData);
	}
	
	protected void UpdateUIForNoGang(Widget contactListHLayout, Widget createGangHLayout, TextWidget gangNameWidget, TextWidget gangCountWidget, TextWidget gangBankExpWidget)
	{
		if (contactListHLayout)
			contactListHLayout.SetVisible(false);
		
		if (createGangHLayout)
			createGangHLayout.SetVisible(true);
		
		if (gangNameWidget)
			gangNameWidget.SetText("You are not in a gang.");
		
		if (gangBankExpWidget)
			gangBankExpWidget.SetText("");
		
		if (gangCountWidget)
			gangCountWidget.SetText("");
		
		if (m_memberListBox)
			m_memberListBox.Clear();
		
		SCR_InputButtonComponent leaveButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sLeaveButton, m_wRoot);
		if (leaveButton && leaveButton.GetRootWidget())
			leaveButton.GetRootWidget().SetVisible(false);
		
		SCR_InputButtonComponent createGangButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sCreateGangButton, m_wRoot);
		if (createGangButton && createGangButton.GetRootWidget())
			createGangButton.GetRootWidget().SetVisible(true);
		
		m_selectedGangMember = null;
		UpdateMemberActionButtonsVisibility(false);
	}
	
	protected void UpdateUIForGang(Widget contactListHLayout, Widget createGangHLayout, TextWidget gangNameWidget, TextWidget gangCountWidget, TextWidget gangBankExpWidget, RL_GangData gangData)
	{
		if (contactListHLayout)
			contactListHLayout.SetVisible(true);
		
		if (createGangHLayout)
			createGangHLayout.SetVisible(false);
		
		if (gangNameWidget)
			gangNameWidget.SetText(gangData.GetName());
		
		if (gangBankExpWidget)
			gangBankExpWidget.SetText(string.Format("$%1 | Exp: %2", gangData.GetBank(), gangData.GetGangExp()));
		
		if (gangCountWidget)
			gangCountWidget.SetText(string.Format("(%1 Members)", gangData.GetCount()));
		
		SCR_InputButtonComponent leaveButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sLeaveButton, m_wRoot);
		if (leaveButton && leaveButton.GetRootWidget())
			leaveButton.GetRootWidget().SetVisible(true);
		
		SCR_InputButtonComponent createGangButton = SCR_InputButtonComponent.GetInputButtonComponent(m_sCreateGangButton, m_wRoot);
		if (createGangButton && createGangButton.GetRootWidget())
			createGangButton.GetRootWidget().SetVisible(false);
	}
	
	protected void UpdateMemberList(RL_GangData gangData)
	{
		if (!m_memberListBox)
			return;
		
		m_memberListBox.Clear();
		m_selectedGangMember = null;
		UpdateMemberActionButtonsVisibility(false);
		
		if (!gangData.GetMembers() || gangData.GetMembers().Count() == 0)
			return;
		
		array<ref RL_GangMember> members = gangData.GetMembers();
		SortMembersByRank(members);
		
		foreach (RL_GangMember member : members)
		{
			Print("[RL_GangWidget] UpdateMemberList foreach loop");
			EGangRank gangRankEnum = member.GetGangRank();
			string rankName = GetGangRankName(gangRankEnum);
			string displayName = string.Format("%1 | %2", member.GetName(), rankName);
			m_memberListBox.AddItem(displayName, member);
		}
	}
	
	protected void SortMembersByRank(inout array<ref RL_GangMember> members)
	{
		// Sort members by rank in descending order (bubble sort)
		for (int i = 0; i < members.Count() - 1; i++)
		{
			Print("[RL_GangWidget] SortMembersByRank for loop");
			for (int j = 0; j < members.Count() - 1 - i; j++)
			{
				Print("[RL_GangWidget] SortMembersByRank for loop 2");
				if (members[j].GetGangRank() < members[j + 1].GetGangRank())
				{
					RL_GangMember temp = members[j];
					members[j] = members[j + 1];
					members[j + 1] = temp;
				}
			}
		}
	}
	
	void FillContactList(array<int> playerList, array<string> nameList)
	{
		if (!m_contactListCombo)
			return;
		
		m_contactListCombo.ClearAll();
		
		m_playerList = playerList;
		m_nameList = nameList;
		
		foreach (string fullName : nameList)
		{
			Print("[RL_GangWidget] FillContactList foreach 2");
			m_contactListCombo.AddItem(fullName);
		}
	}
}
