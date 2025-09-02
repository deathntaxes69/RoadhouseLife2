modded class SCR_ChimeraCharacter
{ 
	
	private const string m_identityLayout = "{FBF24D903F46E9C3}UI/Layouts/Identity/RL_Identity.layout";
	private const string m_identityLayoutPolice = "{2A4414147EB23726}UI/Layouts/Identity/RL_IdentityPolice.layout";
	private Widget m_identityWidget;
	
    void ShowIdentity(string firstName, string lastName, string birthDate, array<string> licenses, int policeRank)
    { 
        Rpc(RpcDo_ShowIdentity, firstName, lastName, birthDate, licenses, policeRank);
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    private void RpcDo_ShowIdentity(string firstName, string lastName, string birthDate, array<string> licenses, int policeRank)
    {
		RL_Hud hud = RL_Hud.GetCurrentInstance();
		if(!hud)
			return;
		
		// maybe add null checks here...
		if (policeRank > 0)
		{
			m_identityWidget = GetGame().GetWorkspace().CreateWidgets(m_identityLayoutPolice);
			Tuple2<string, string> rank = GetPoliceRankName(policeRank);
			string rankName = rank.param1;
			string badgeImage = rank.param2;
			
			TextWidget wRank = TextWidget.Cast(m_identityWidget.FindAnyWidget("rank"));
			wRank.SetText(rankName);
			
			ImageWidget wBadge = ImageWidget.Cast(m_identityWidget.FindAnyWidget("badge"));
			wBadge.LoadImageTexture(0, badgeImage);
		}
		else
		{
			m_identityWidget = GetGame().GetWorkspace().CreateWidgets(m_identityLayout);
		}
		
		Widget identityLicenses = Widget.Cast(m_identityWidget.FindAnyWidget("LicensesListBox"));
		
		SCR_ListBoxComponent identityLicensesComponent = SCR_ListBoxComponent.Cast(identityLicenses.FindHandler(SCR_ListBoxComponent));
		foreach (string license : licenses)
		{
			Print("[RL_IdentityCharacter] RpcDo_ShowIdentity foreach loop");
			identityLicensesComponent.AddItem(license);
		}
		
        TextWidget identityFirstName = TextWidget.Cast(m_identityWidget.FindAnyWidget("FirstName"));
        identityFirstName.SetText(firstName);
	
		TextWidget identityLastName = TextWidget.Cast(m_identityWidget.FindAnyWidget("LastName"));
        identityLastName.SetText(lastName);
		
		TextWidget identityBirth = TextWidget.Cast(m_identityWidget.FindAnyWidget("Age"));
		
		identityBirth.SetText(birthDate);
		
        		ResourceName prefabToPreview = RL_ShopUtils.ExtractEntityPrefabName(this);
        ItemPreviewWidget characterPreview = ItemPreviewWidget.Cast(m_identityWidget.FindAnyWidget("characterPreview"));
        if(!characterPreview) return;

        ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world) return;

        ItemPreviewManagerEntity characterPreviewManager = world.GetItemPreviewManager();
        if(!characterPreviewManager) return;

        characterPreviewManager.SetPreviewItemFromPrefab(characterPreview, prefabToPreview, null, true);
		
		
		hud.AddIdentity(m_identityWidget);
    } 

}