#ifndef BLERCCAR_PACKETS
#define BLERCCAR_PACKETS

class BLERCCarPacket
{
protected:
    const int8_t m_Command;
    const int8_t m_Amount;
    const int8_t raw[2];

public:
    BLERCCarPacket() = delete;
    BLERCCarPacket(const int8_t command, const int8_t amount) : m_Command{command}, m_Amount{amount}, raw{m_Command, m_Amount} {};
    ~BLERCCarPacket(){};

    const int8_t *GetPayload()
    {
        return raw;
    };
    const int8_t GetCommand()
    {
        return m_Command;
    };
    const int8_t GetAmount()
    {
        return m_Amount;
    };
};

class BLESteerLeftPacket : public BLERCCarPacket
{
private:
public:
    BLESteerLeftPacket() = delete;
    BLESteerLeftPacket(const int8_t amount) : BLERCCarPacket('L', amount){};
    /**
     * @brief Construct a new BLESteerLeftPacket object.
     * @note This constructor is unsafe. No safety checks on payload
     *
     * @param payload
     */
    BLESteerLeftPacket(const int8_t *payload) : BLERCCarPacket(payload[0], payload[1]){};
    ~BLESteerLeftPacket(){};
};

class BLESteerRightPacket : public BLERCCarPacket
{
private:
public:
    BLESteerRightPacket() = delete;
    BLESteerRightPacket(const int8_t amount) : BLERCCarPacket('R', amount){};
    BLESteerRightPacket(const int8_t *payload) : BLERCCarPacket(payload[0], payload[1]){};
    ~BLESteerRightPacket(){};
};
#endif // BLERCCAR_PACKETS