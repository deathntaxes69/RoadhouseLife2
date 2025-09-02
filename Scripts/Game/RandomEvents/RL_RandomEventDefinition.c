[BaseContainerProps()]
class RL_RandomEventDefinition
{
    [Attribute(defvalue: "", category: "Event", desc: "Event ID")]
    string eventId;
    
    [Attribute(defvalue: "", category: "Event", desc: "Event Name")]
    string name;
    
    [Attribute(defvalue: "", category: "Event", desc: "Event Description")]
    string description;
    
    [Attribute(defvalue: "", category: "Event", desc: "Method")]
    string methodName;
    
    [Attribute(defvalue: "1", category: "Event", desc: "Weight (affects probability within group)")]
    int weight;
    
    [Attribute(defvalue: "1", category: "Event", desc: "Enabled")]
    bool enabled;
} 