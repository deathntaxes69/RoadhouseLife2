class RL_RadioUtils
{
    static int m_iPhoneChannel = 1;
   /*
    static int GetRandomPhoneFreq()
    {
        SCR_VONController vonController = SCR_VONController.Cast(GetGame().GetPlayerController().FindComponent(SCR_VONController));
        SCR_VONEntryRadio phoneEntry = SCR_VONEntryRadio.Cast(vonController.GetPhoneEntry());
        BaseTransceiver phoneTsv = phoneEntry.GetTransceiver();
        Print("GetRandomPhoneFreq");
        Print(phoneTsv.GetMinFrequency());
        Print(phoneTsv.GetMaxFrequency());
        //Stupid hack becuase reforger devs have the downs
		
        return Math.RandomInt(phoneTsv.GetMinFrequency()/100, phoneTsv.GetMaxFrequency()/100)*100;

    }
   */

    static void SetRandomFrequency(IEntity playerEntity)
    {
        int randomFreq = (40 + Math.RandomInt(0, 900)) * 1000;
        Print("SetRandomFrequency 1");
        Print(randomFreq);
        SetFrequency(playerEntity, 0, randomFreq, true);

        randomFreq = (1000 + Math.RandomInt(150, 900)) * 1000;
        Print("SetRandomFrequency 2");
        Print(randomFreq);
        SetFrequency(playerEntity, 1, randomFreq, true);
    }

    static bool SetPhoneFrequency(int playerId, int freq)
    {
        IEntity playerEntity = SCR_PossessingManagerComponent.GetPlayerMainEntity(playerId);
    
        return SetFrequency(playerEntity, m_iPhoneChannel, freq);

    }
    static bool SetFrequency(IEntity playerEntity, int channel, int freq, bool ignoreIfNotDefault = false)
    {
        BaseRadioComponent radioComp = GetRadioComponent(playerEntity);
        if (!radioComp)
        {
            Print("SetFrequency no radioComp");
            return false;
        }

        BaseTransceiver tsv = radioComp.GetTransceiver(channel);
        if (!tsv)
        {
            Print("SetFrequency no tsv");
            return false;
        }
        float currentFreq = tsv.GetFrequency();
        if(ignoreIfNotDefault && (currentFreq != 40000.0 && currentFreq != 1000000.0))
            return false;

        radioComp.SetPower(true);

        //SetChannelActive(channel);
		tsv.SetFrequency(freq);
        return true;

    }
    static bool SetChannelActive(int channel)
    {
        SCR_VONController vonController = SCR_VONController.Cast(GetGame().GetPlayerController().FindComponent(SCR_VONController));
        vonController.SetChannelActive(channel);
        return true;
    }
    static BaseRadioComponent GetRadioComponent(IEntity player)
	{
        ChimeraCharacter character = ChimeraCharacter.Cast(player);
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.Cast(character.FindComponent(SCR_GadgetManagerComponent));
		if (!gadgetManager)
			return null;
		
		GenericEntity radio = GenericEntity.Cast(gadgetManager.GetGadgetByType(EGadgetType.RADIO));
		if (!radio)
			return null;
		
		BaseRadioComponent radioComp = BaseRadioComponent.Cast(radio.FindComponent(BaseRadioComponent));
		if (!radioComp)
			return null;
		
		return radioComp;
	}
    static array<BaseTransceiver> GetAllTransceivers(IEntity playerEntity, out array<BaseTransceiver>tArray)
    {
        tArray = new array<BaseTransceiver>;
        BaseRadioComponent radioComp = GetRadioComponent(playerEntity);
        if (!radioComp)
        {
            Print("GetAllTransceivers no radioComp");
            return tArray;
        }
        
        int transceiverCount = radioComp.TransceiversCount();
        
        for (int i = 0; i < transceiverCount; i++)
        {
            BaseTransceiver transceiver = radioComp.GetTransceiver(i);
            if (transceiver)
            {
                tArray.Insert(transceiver);
            }
        }
        
        return tArray;
    }
    static float HzToKhz(float freq)
    {
        return freq / 1000;
    }
    static float KhzToHz(float freq)
    {
        return freq * 1000;
    }
    //SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_RADIO_FREQUENCY_CYCLE);
    //RL_InventoryUtils.FindItemsByPrefab()
    //RegisterVONEntries();
    //int freq = tsv.GetFrequency();
	//int freqMax = tsv.GetMaxFrequency();
	//int freqMin = tsv.GetMinFrequency();
    //BaseTransceiver tsv = m_BaseRadioComp.GetTransceiver(0);
    //RadioHandlerComponent rhc = RadioHandlerComponent.Cast(pc.FindComponent(RadioHandlerComponent));
    //rhc.SetFrequency(tsv, freq);
}