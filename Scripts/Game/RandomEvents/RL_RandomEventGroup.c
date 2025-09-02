[BaseContainerProps()]
class RL_RandomEventGroup
{
    [Attribute(defvalue: "", category: "Group", desc: "Group ID")]
    string groupId;
    
    [Attribute(defvalue: "", category: "Group", desc: "Group Name")]
    string name;
    
    [Attribute(defvalue: "", category: "Group", desc: "Group Description")]
    string description;
    
    [Attribute(defvalue: "30", category: "Timing", desc: "Frequency in minutes")]
    int frequencyMinutes;
    
    [Attribute(defvalue: "0.5", category: "Probability", desc: "Chance to execute an event")]
    float executionChance;
    
    [Attribute(defvalue: "1", category: "Group", desc: "Enabled")]
    bool enabled;
    
    [Attribute(defvalue: "", category: "Events", desc: "Events in this group")]
    ref array<ref RL_RandomEventDefinition> events;
} 