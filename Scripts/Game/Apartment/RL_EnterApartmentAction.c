class RL_EnterApartmentAction : ScriptedUserAction
{
	SCR_ChimeraCharacter m_ownerCharacter;
	SCR_ChimeraCharacter m_userCharacter;

	protected ref RL_CharacterDbHelper m_characterHelper;

	[Attribute(defvalue:"1")]
	protected int m_iAparmentLocationID;

	[Attribute(defvalue:"25000", desc:"Price to buy this apartment")]
	protected int m_iApartmentPrice;

	[Attribute(defvalue:"{2B3E7FB7276E3F3A}Assets/Narcos_Apartment/Prefab/NLR_Apartment.et", desc:"Apartment prefab to spawn")]
	protected ResourceName m_sApartmentPrefab;

	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ownerCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
	}
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
        m_userCharacter = SCR_ChimeraCharacter.Cast(pUserEntity);
		m_userCharacter.SaveEnterLocation();

		if(Replication.IsServer())
		{
			if(m_userCharacter.GetApartmentLocationId() == 0)
			{
				m_userCharacter.SetApartmentLocationId(m_iAparmentLocationID);
			}
			m_userCharacter.TryEnterApartment(m_sApartmentPrefab, m_iApartmentPrice);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if(!m_userCharacter)
		{
			outName = string.Format("Buy and Enter Apartment ($%1)", m_iApartmentPrice);
			return true;
		}

		if(m_userCharacter.HasApartment() && m_userCharacter.GetApartment() == m_sApartmentPrefab)
		{
        	outName = "Enter Apartment";
		} 
		else if(!m_userCharacter.HasApartment()) 
		{
			if(m_userCharacter.GetCash() >= m_iApartmentPrice)
			{
				outName = string.Format("Buy and Enter Apartment ($%1)", m_iApartmentPrice);
			}
			else
			{
				outName = string.Format("Buy Apartment - Insufficient Balance ($%1)", m_iApartmentPrice);
			}
		} 
		else 
		{
			outName = "You no apartment here :)";
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		m_userCharacter = SCR_ChimeraCharacter.Cast(user);
		return (
			m_userCharacter &&
			!m_userCharacter.IsRestrained()
		);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		m_userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (!m_userCharacter || m_userCharacter.IsRestrained())
			return false;
		
		if (!m_userCharacter.HasApartment())
		{
			return m_userCharacter.GetCash() >= m_iApartmentPrice;
		}
		
		return true;
 	}
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
}