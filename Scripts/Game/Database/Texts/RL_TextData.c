class RL_TextData : JsonApiStruct
{
    protected int id = 0;
    protected string recipient;
    protected string sender;
    protected string partyName = "";
    protected string content = "";

    string GetPartyName()
    {
        return partyName;
    }
    string GetContent()
    {
        return content;
    }
    string GetSender()
    {
        return sender;
    }
    string GetRecipient()
    {
        return recipient;
    }
    void RL_TextData()
    {
        RegV("id");
        RegV("recipient");
        RegV("sender");
        RegV("partyName");
        RegV("content");
    }
}
