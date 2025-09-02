class RL_ApartmentUtils
{
    protected static int m_iMaxInRow = 15;    // X direction
    protected static int m_iMaxStacked = 5;      // Y direction
    protected static int m_iSeparationSpace = 22;   // X and Z spacing

    static void OffsetApartmentByPlayerId(int playerId, inout vector transform[4])
    {
        int xIndex = playerId % m_iMaxInRow;
        int temp = playerId / m_iMaxInRow;
        int yIndex = temp % m_iMaxStacked;
        int zIndex = temp / m_iMaxStacked;

        transform[3][0] = transform[3][0] + (xIndex * m_iSeparationSpace);     // X
        transform[3][1] = transform[3][1] + (yIndex * m_iSeparationSpace);     // Y (height)
        transform[3][2] = transform[3][2] + (zIndex * m_iSeparationSpace);     // Z

    }
}