#ifndef BLERCCAR_PACKETS
#define BLERCCAR_PACKETS

#include "common.hpp"
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
    /**
     * @brief Construct a new BLESteerRightPacket object
     * * Use this packet for sending data to the server
     *
     * @param amount
     */
    BLESteerRightPacket(const char amount) : BLERCCarPacket('R', amount){};
    BLESteerRightPacket(const char *payload) : BLERCCarPacket(payload[0], payload[1]){};
    ~BLESteerRightPacket(){};
};

class BLEDrivePacket : public BLERCCarPacket
{
private:
public:
    /**
     * @brief Get the DriveMode object Internally parse raw_data into DriveMode
     *
     * @return const DriveMode
     */
    const DriveMode GetDriveMode()
    {
        auto raw_drive_mode = (DriveMode)m_Command;

        switch (raw_drive_mode)
        {
        case DriveMode::Forward:
            return DriveMode::Forward;
        case DriveMode::ForwardCollision:
            return DriveMode::ForwardCollision;
        case DriveMode::Backward:
            return DriveMode::Backward;
        case DriveMode::BackwardCollision:
            return DriveMode::BackwardCollision;
        default:
            break;
        }
    };

    BLEDrivePacket() = delete;
    /**
     * @brief Construct a new BLEDrivePacket object
     * * Use this constructor for sending data to the server
     *
     * @param mode
     * @param gas_percentage
     */
    BLEDrivePacket(const DriveMode mode, const char gas_percentage) : BLERCCarPacket((const char)mode, gas_percentage){};
    /**
     * @brief Construct a new BLEDrivePacket object
     * * Use this constructor when getting data from client (On the server side)
     *
     * @param payload
     */
    BLEDrivePacket(const char *payload) : BLERCCarPacket(payload[0], payload[1]){};
    ~BLEDrivePacket(){};
};
#endif // BLERCCAR_PACKETS