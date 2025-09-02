modded class SCR_CharacterControllerComponent
{
	protected ref RL_CharacterDbHelper m_characterHelper;

	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);

		if (newLifeState != ECharacterLifeState.DEAD)
			return;

		SCR_ChimeraCharacter character = GetCharacter();
		if (!character)
			return;

		// This could be improved by getting the updated cash first
		int cash = character.GetCash();
		if (!cash || cash < 1)
			return;

		Rpc(RpcAsk_RemoveCash, character.GetCharacterId(), cash);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RemoveCash(string characterId, int amount) {
		PrintFormat("RpcAsk_RemoveCash characterid=%1 amount=%2", characterId, amount);

		Tuple2<string, int> context(characterId, amount);

		if (!m_characterHelper) m_characterHelper = new RL_CharacterDbHelper();
		m_characterHelper.TransactMoney(characterId, (-amount), 0, this, "RemoveCashCallback", context);
	}

	void RemoveCashCallback(bool success, Managed context)
	{
		if (!success)
		{
			Print("RL_DroppedCashCharacterController failed to remove cash", LogLevel.ERROR);
			return;
		}
		Tuple2<string, int> typedContext = Tuple2<string, int>.Cast(context);


		// Spawn dropped cash
		ResourceName prefabName = "{9D11935DAD20B182}Prefabs/Items/Currencies/DroppedCash.et";

		//Spawn money a bit away from dead body
		vector mat[4];
		GetOwner().GetTransform(mat);
		vector playerOrgin = GetOwner().GetOrigin();
		playerOrgin = playerOrgin + (mat[2] * 1); // mat[2] is forward vector, multiply by 1 meter
		IEntity droppedCashEntity = EPF_Utils.SpawnEntityPrefab(prefabName, playerOrgin);

		GetGame().GetCallqueue().CallLater(SetValueDelayed, 100, false, droppedCashEntity, context);
	}

	void SetValueDelayed(IEntity droppedCashEntity, Managed context)
	{
		if (!droppedCashEntity)
			return;

		Tuple2<string, int> typedContext = Tuple2<string, int>.Cast(context);

		RL_DroppedCashComponent droppedCashComponent = RL_DroppedCashComponent.Cast(droppedCashEntity.FindComponent(RL_DroppedCashComponent));
		if (!droppedCashComponent)
			return;

		droppedCashComponent.SetProps(typedContext.param1, typedContext.param2);
	}

	void PickupCashCallback(bool success, Managed context)
	{
		Tuple2<int, int> typedContext = Tuple2<int, int>.Cast(context);
		Rpc(RpcDo_PickupCashCallback, success, typedContext.param1, typedContext.param2);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_PickupCashCallback(bool success, int cashEntityRplId, int cashValue) {

		if (success)
			RL_Utils.Notify(string.Format("You picked up %1", RL_Utils.FormatMoney(cashValue)), "CASH");
		else
			RL_Utils.Notify("Something went wrong, please report this bug.", "CASH");

		Rpc(RpcAsk_DeleteCashEntity, cashEntityRplId);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_DeleteCashEntity(int cashEntityRplId) {
		Print("RpcAsk_DeleteCashEntity");
		IEntity cashEntity = EPF_NetworkUtils.FindEntityByRplId(cashEntityRplId);
		if (cashEntity)
			SCR_EntityHelper.DeleteEntityAndChildren(cashEntity);
	}

}
