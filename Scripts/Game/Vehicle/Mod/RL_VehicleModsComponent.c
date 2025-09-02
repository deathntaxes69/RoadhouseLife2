[EntityEditorProps(category: "GameScripted/Components", description: "")]
class RL_VehicleModsComponentClass: ScriptComponentClass
{
};

class RL_VehicleModsComponent: ScriptComponent
{
    protected Vehicle m_vehicle;
    protected SlotManagerComponent m_slotManager;
    protected ref map<string, EntitySlotInfo> m_slotInfoMap;
    
    [RplProp()]
    protected ref array<string> m_aSlotNames;

    [RplProp()]
    protected ref array<int> m_aSlotColors;
    
    [RplProp()]
    protected int m_iBodyColor = -1;
    
    //------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
        m_vehicle = Vehicle.Cast(GetOwner().GetRootParent());
        m_slotInfoMap = new map<string, EntitySlotInfo>();
        if (!m_aSlotNames)
            m_aSlotNames = new array<string>();
        if (!m_aSlotColors)
            m_aSlotColors = new array<int>();
            
		GetGame().GetCallqueue().CallLater(ProcessSlotManager, 500);
    }
    
    //------------------------------------------------------------------------------------------------
    void ProcessSlotManager()
    {
        m_slotManager = SlotManagerComponent.Cast(m_vehicle.FindComponent(SlotManagerComponent));
        
        if (m_slotManager)
        {
            array<EntitySlotInfo> slotInfos = {};
            m_slotManager.GetSlotInfos(slotInfos);
            
            foreach (EntitySlotInfo slotInfo : slotInfos)
            {
                if (slotInfo)
                {
                    string sourceName = slotInfo.GetSourceName();
                    IEntity attachedEntity = slotInfo.GetAttachedEntity();
                    
                    if (attachedEntity)
                    {
                        ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(attachedEntity.FindComponent(ParametricMaterialInstanceComponent));
                        
                        if (matComp)
                        {
                            m_slotInfoMap.Set(sourceName, slotInfo);
                        }
                    }
                }
            }
        }
        
        ApplyStoredColors();
    }
    
    //------------------------------------------------------------------------------------------------
    EntitySlotInfo GetSlotInfo(string sourceName)
    {
        if (!m_slotInfoMap)
            return null;
            
        return m_slotInfoMap.Get(sourceName);
    }
    
    //------------------------------------------------------------------------------------------------
    void SetSlotColorPreview(string sourceName, int argbColor)
    {
        EntitySlotInfo slotInfo = GetSlotInfo(sourceName);
        if (!slotInfo)
            return;
            
        IEntity attachedEntity = slotInfo.GetAttachedEntity();
        if (!attachedEntity)
            return;
            
        ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(attachedEntity.FindComponent(ParametricMaterialInstanceComponent));
        if (!matComp)
            return;
            
        matComp.SetColor(argbColor);
    }
    
    //------------------------------------------------------------------------------------------------
    void SetBodyColorPreview(int argbColor)
    {
        if (!m_vehicle)
            return;
            
        ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(m_vehicle.FindComponent(ParametricMaterialInstanceComponent));
        if (!matComp)
            return;
            
        matComp.SetColor(argbColor);
    }
    
    //------------------------------------------------------------------------------------------------
    void RequestApplyAllColors(array<string> slotNames, array<int> slotColors, int bodyColor = -1)
    {
        PlayerController playerController = GetGame().GetPlayerController();
        if (!playerController)
            return;
            
        IEntity playerEntity = playerController.GetControlledEntity();
		
        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
        if (!playerCharacter)
            return;

        RplId vehicleRplId = EPF_NetworkUtils.GetRplId(m_vehicle);
        playerCharacter.SetVehicleColors(vehicleRplId, slotNames, slotColors, bodyColor);
    }
    
    //------------------------------------------------------------------------------------------------
    void ApplyColorsWithAuthority(array<string> slotNames, array<int> slotColors, int bodyColor)
    {
        Rpc(RpcDo_SetAllSlotsColor, slotNames, slotColors, bodyColor);
    }
    
    //------------------------------------------------------------------------------------------------
    void ApplyColors(array<string> slotNames, array<int> slotColors, int bodyColor)
    {
        if (bodyColor != -1)
        {
            if (m_vehicle)
            {
                ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(m_vehicle.FindComponent(ParametricMaterialInstanceComponent));
                if (matComp)
                    matComp.SetColor(bodyColor);
            }
        }
        
        if (slotNames && slotColors && slotNames.Count() == slotColors.Count())
        {
            for (int i = 0; i < slotNames.Count(); i++)
            {
                string sourceName = slotNames[i];
                int argbColor = slotColors[i];
                
                EntitySlotInfo slotInfo = GetSlotInfo(sourceName);
                if (!slotInfo)
                    continue;
                    
                IEntity attachedEntity = slotInfo.GetAttachedEntity();
                if (!attachedEntity)
                    continue;
                    
                ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(attachedEntity.FindComponent(ParametricMaterialInstanceComponent));
                if (!matComp)
                    continue;
                    
                matComp.SetColor(argbColor);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_SetAllSlotsColor(array<string> slotNames, array<int> slotColors, int bodyColor)
    {
        if (Replication.IsServer())
        {
            m_aSlotNames.Clear();
            m_aSlotColors.Clear();

            if (slotNames && slotColors && slotNames.Count() == slotColors.Count())
            {
                for (int i = 0; i < slotNames.Count(); i++)
                {
                    m_aSlotNames.Insert(slotNames[i]);
                    m_aSlotColors.Insert(slotColors[i]);
                }
            }
            
            m_iBodyColor = bodyColor;
            Replication.BumpMe();
        }
        
        ApplyColors(slotNames, slotColors, bodyColor);
    }

    //------------------------------------------------------------------------------------------------
    void ApplyStoredColors()
    {
        ApplyColors(m_aSlotNames, m_aSlotColors, m_iBodyColor);
    }

    //------------------------------------------------------------------------------------------------
    array<ref array<string>> GetSlotsColor()
    {
        array<ref array<string>> result = {};
        if (m_vehicle)
        {
            ParametricMaterialInstanceComponent vehicleMatComp = ParametricMaterialInstanceComponent.Cast(m_vehicle.FindComponent(ParametricMaterialInstanceComponent));
            if (vehicleMatComp)
            {
                int bodyColor = vehicleMatComp.GetColor();
                array<string> bodyTuple = {"body", bodyColor.ToString()};
                result.Insert(bodyTuple);
            }
        }
        
        if (!m_slotInfoMap)
            return result;
            
        foreach (string slotName, EntitySlotInfo slotInfo : m_slotInfoMap)
        {
            if (!slotInfo)
                continue;
                
            IEntity attachedEntity = slotInfo.GetAttachedEntity();
            if (!attachedEntity)
                continue;
                
            ParametricMaterialInstanceComponent matComp = ParametricMaterialInstanceComponent.Cast(attachedEntity.FindComponent(ParametricMaterialInstanceComponent));
            if (!matComp)
                continue;
                
            int color = matComp.GetColor();
            
            array<string> tuple = {slotName, color.ToString()};
            result.Insert(tuple);
        }
        
        return result;
    }

    //------------------------------------------------------------------------------------------------
    override bool RplSave(ScriptBitWriter writer)
    {
        writer.WriteInt(m_iBodyColor);
        
        int slotCount = 0;
        if (m_aSlotNames && m_aSlotColors && m_aSlotNames.Count() == m_aSlotColors.Count())
            slotCount = m_aSlotNames.Count();
            
        writer.WriteInt(slotCount);
        for (int i = 0; i < slotCount; i++)
        {
            writer.WriteString(m_aSlotNames[i]);
            writer.WriteInt(m_aSlotColors[i]);
        }
        
        return true;
    }

    //------------------------------------------------------------------------------------------------
    override bool RplLoad(ScriptBitReader reader)
    {
        reader.ReadInt(m_iBodyColor);

        int slotCount;
        reader.ReadInt(slotCount);

        m_aSlotNames.Clear();
        m_aSlotColors.Clear();

        for (int i = 0; i < slotCount; i++)
        {
            string slotName;
            int slotColor;
            
            reader.ReadString(slotName);
            reader.ReadInt(slotColor);
            
            m_aSlotNames.Insert(slotName);
            m_aSlotColors.Insert(slotColor);
        }
        
        GetGame().GetCallqueue().CallLater(ApplyStoredColors, 100);
        return true;
    }
}

