[BaseContainerProps(configRoot: true)]
class RL_RandomEventConfig : Managed
{
    [Attribute(defvalue: "", category: "Random Events", desc: "All Random Event Groups")]
    ref array<ref RL_RandomEventGroup> eventGroups;
} 