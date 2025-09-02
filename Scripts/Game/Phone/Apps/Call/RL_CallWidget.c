sealed class RL_CallWidget : RL_AppBaseWidget
{

	TextWidget m_wCallTitle;
	TextWidget m_wCallSubtitle;

	SCR_InputButtonComponent m_wAcceptButton;

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		if(!m_character) return;

		m_wCallTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("titleText"));
		m_wCallSubtitle = TextWidget.Cast(m_wRoot.FindAnyWidget("subtitleText"));

        ButtonWidget acceptWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("AcceptCall"));
		if (acceptWidget)
			m_wAcceptButton = SCR_InputButtonComponent.Cast(acceptWidget.FindHandler(SCR_InputButtonComponent));

        ButtonWidget endWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("EndCall"));
		if (endWidget)
		{
			SCR_InputButtonComponent endButton = SCR_InputButtonComponent.Cast(endWidget.FindHandler(SCR_InputButtonComponent));
			if (endButton)
			{
				endButton.m_OnActivated.Insert(OnEnd);
			}
		}
		
		UpdateUiState();

		if(m_character.GetCallAcceptedTime())
			GetGame().GetCallqueue().CallLater(UpdateCallTime, 1000, true);
		
	}
	void UpdateUiState()
	{
		m_wCallTitle.SetText(m_character.GetCallPartyName());

		if(m_character.IsCallAccepted())
			m_wCallSubtitle.SetText("00:00");
		else
		{
			if(m_character.IsCallStarter())
				m_wCallSubtitle.SetText("Calling");
			else
				m_wCallSubtitle.SetText("Incoming Call");
		}

		if (m_wAcceptButton && (m_character.IsCallAccepted() || m_character.IsCallStarter()))
		{
			m_wAcceptButton.m_OnActivated.Remove(OnAccept);
			m_wAcceptButton.SetVisible(false);
		} else {
			m_wAcceptButton.m_OnActivated.Insert(OnAccept);
			m_wAcceptButton.SetVisible(true);
		}
	}	
	void UpdateCallTime()
	{
		int currentUnixTime = System.GetUnixTime();
		int callStartTime = m_character.GetCallAcceptedTime();
		if(!callStartTime)
		{
			GetGame().GetCallqueue().Remove(UpdateCallTime);
			return;
		}
		int deltaTime = currentUnixTime - callStartTime;
    	string formattedTime = SCR_FormatHelper.GetTimeFormatting(deltaTime, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS);
		m_wCallSubtitle.SetText(formattedTime);

	}
	void OnAccept(SCR_InputButtonComponent btn)
    {
		Print("ACCEPTCALL");
		if (m_character.IsSpamming())
			return;

        m_character.AcceptCall();
		UpdateUiState();
		GetGame().GetCallqueue().CallLater(UpdateCallTime, 1000, true);
    }
	void OnEnd(SCR_InputButtonComponent btn)
    {
		if (m_character.IsSpamming())
			return;

		if(m_character.GetPhoneStatus())
			m_character.EndCall();
		else
        	m_character.DenyCall();

		m_phoneMenu.CloseMenu();
    }
	override void HandlerDeattached(Widget w)
	{
		GetGame().GetCallqueue().Remove(UpdateCallTime);
		if(!m_character)
			return;
		//m_character.DenyCall();
	}


}