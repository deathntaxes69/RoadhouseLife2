modded class SCR_NameTagData
{ 
    override void GetName(out string name, out notnull array<string> nameParams)
	{
		 // Changed to use the identity component for actual players instead of just AI
		if (m_eType == ENameTagEntityType.AI || m_eType == ENameTagEntityType.PLAYER)
		{
			
            CharacterIdentityComponent charIdentity = CharacterIdentityComponent.Cast(m_Entity.FindComponent(CharacterIdentityComponent));
            if (charIdentity && charIdentity.GetIdentity())
                m_sName = charIdentity.GetIdentity().GetName();
            else 
                m_sName = "No character identity!";
		}
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_Entity);
		// Clients cant see changes to identity comp so I added this workaround
        if(character)
        {
			// Clients cant see changes to identity comp so I added this workaround
			if(!m_sName)
            	m_sName = character.GetCharacterName();
			// Add character ID TO end of name
			//m_sName = string.Format("%1 (%2)", m_sName, character.GetCharacterId());
			string shortIdentityId = RL_Utils.ShortenIdentityId(character.GetIdentityId());

			m_sName = string.Format("%1 - %2", shortIdentityId, character.GetCharacterId());
        }

		name = m_sName;
		nameParams.Copy(m_aNameParams);
	}
    IEntity GetEntity()
    {
        return m_Entity;
    }

	//------------------------------------------------------------------------------------------------
	//! Set visibility of nametag widget
	//! \param widget is the target widget
	//! \param visible controls whether the widget should be made visible or invisible
	//! \param animate controls if the widget should fade in/out 
	override void SetVisibility(Widget widget, bool visible, float visibleOpacity, bool animate = true)
	{
		if ( !widget )
			return;
						
		float targetVal;
		if (visible)
		{
			m_NameTagWidget.SetVisible(true);

			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_Entity);
			if (character)
				m_NameTagWidget.FindWidget("HorizontalLayoutTop").SetVisible(character.IsNewPlayer());
			
			if (widget == m_NameTagWidget)
				m_fVisibleOpacity = visibleOpacity;
			
			m_Flags |= ENameTagFlags.VISIBLE;
			m_Flags &= ~ENameTagFlags.UPDATE_DISABLE;
			m_Flags &= ~ENameTagFlags.DISABLED;
			targetVal = visibleOpacity;
		}
		else 
			targetVal = 0;
		
		if (m_fTagFadeSpeed == 0)
			animate = false;
		
		if (animate)
			AnimateWidget.Opacity(widget, targetVal, m_fTagFadeSpeed);
		else 
		{
			AnimateWidget.StopAnimation(widget, WidgetAnimationOpacity);
			widget.SetOpacity(targetVal);
			if (targetVal >= 0.1)
				widget.SetVisible(true);
		}
	}

}