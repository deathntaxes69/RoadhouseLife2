class RL_ItemRobberyAction : RL_RobberyAction
{

    [Attribute("", UIWidgets.Object, "List of outputs")]
	ref array<ref RL_ProcessingOutput> m_aRobberyOutputs;

    override void FinishRobbery()
	{
        m_userEntity.TransactMoney( m_iRobberyBaseReward );
		RL_Utils.Notify(string.Format("You stole %1", RL_Utils.FormatMoney(m_iRobberyBaseReward)), "ROBBERY");
        CancelRobbery();
        InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(m_userEntity);
		if (!inventoryManager) 
			return;
        foreach (RL_ProcessingOutput robberyOutput : m_aRobberyOutputs)
		{
            UIInfo uiInfo = RL_ShopUtils.GetItemDisplayInfo(robberyOutput.m_OutputPrefab);
			if (uiInfo)
                RL_Utils.Notify(string.Format("You found %1 x %2", robberyOutput.m_iOutputAmount, uiInfo.GetName(), "ROBBERY"));

            m_userEntity.AddAmount(robberyOutput.m_OutputPrefab, robberyOutput.m_iOutputAmount, null, "", true);
		}

	}


}