class RL_CallbackUpdateGarageState : RL_RestCallback
{
    RL_CharacterAccount m_character;

    void SetCharacter(RL_CharacterAccount character)
    {
        m_character = character;
    }
    override void OnSuccess(string data, int dataSize)
    {

        
    }
}