#ifndef BLERCCAR_PACKETS
#define BLERCCAR_PACKETS

class BLERCCarPacket
{
protected:
    const char m_Command;
    const char m_Amount;
    const char raw[2];

public:
    BLERCCarPacket() = delete;
    BLERCCarPacket(const char command, const char amount) : m_Command{command}, m_Amount{amount}, raw{m_Command, m_Amount} {};
    ~BLERCCarPacket(){};

    const char *GetPayload()
    {
        return raw;
    };
    const char GetCommand()
    {
        return m_Command;
    };
    const char GetAmount()
    {
        return m_Amount;
    };
};

class BLESteerLeftPacket : public BLERCCarPacket
{
private:
public:
    BLESteerLeftPacket() = delete;
    BLESteerLeftPacket(const char amount) : BLERCCarPacket('L', amount){};
    /**
     * @brief Construct a new BLESteerLeftPacket object.
     * @note This constructor is unsafe. No safety checks on payload
     *
     * @param payload
     */
    BLESteerLeftPacket(const char *payload) : BLERCCarPacket(payload[0], payload[1]){};
    ~BLESteerLeftPacket(){};
};

class BLESteerRightPacket : public BLERCCarPacket
{
private:
public:
    BLESteerRightPacket() = delete;
    BLESteerRightPacket(const char amount) : BLERCCarPacket('R', amount){};
    BLESteerRightPacket(const char *payload) : BLERCCarPacket(payload[0], payload[1]){};
    ~BLESteerRightPacket(){};
};
#endif // BLERCCAR_PACKETS