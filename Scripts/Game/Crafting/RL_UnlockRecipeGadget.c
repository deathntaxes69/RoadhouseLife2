[ComponentEditorProps(category: "RL/Crafting", description: "")]
class RL_UnlockRecipeGadgetClass : SCR_GadgetComponentClass {}

class RL_UnlockRecipeGadget: SCR_GadgetComponent
{
	[Attribute(defvalue: "", desc: "Recipe Name", category: "Recipe")]
	protected string m_sRecipeName;
	
	[Attribute(defvalue: "", desc: "Recipe ID", category: "Recipe")]
	protected string m_sRecipeId;

	override EGadgetType GetType()
	{
		return EGadgetType.CONSUMABLE;
	}

	override void OnToggleActive(bool state)
	{
		m_bActivated = state;
		
		if (!state) 
			return;
		
		if(RL_Utils.IsDedicatedServer()) 
			return;
			
		if (!m_CharacterOwner)
			return;
			
		RplComponent rplComponent = RplComponent.Cast(m_CharacterOwner.FindComponent(RplComponent));
		if (!rplComponent || !rplComponent.IsOwner())
			return;
		
		ApplyEffect();
	}

	override void ActivateAction()
	{
		if (m_CharacterOwner && m_iMode == EGadgetMode.IN_HAND)
			ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
	}

	protected void ApplyEffect()
	{
		if (!m_CharacterOwner)
			return;
			
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_CharacterOwner);
		if (!character)
			return;
			
		if (m_sRecipeId.IsEmpty())
			return;
		
		if (character.HasRecipe(m_sRecipeId))
		{
			RL_Utils.Notify(string.Format("You already know %1", m_sRecipeName), "CRAFTING");
			return;
		}
		
		int gadgetRplId = EPF_NetworkUtils.GetRplId(GetOwner());
		character.LearnRecipeAndDestroy(m_sRecipeId, gadgetRplId);
	}

}