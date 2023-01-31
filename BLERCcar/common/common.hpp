#ifndef COMMON
#define COMMON
#include <iostream>
#include "DistanceSensor.hpp"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

/**
 * @brief Bulk of my own UUID's
 * Car service UUID: 10b47b82-89d8-11ed-a1eb-0242ac120002
 * Characteristic Drive mode UUID: (CHARACTERISTIC_UUID_DRIVE_MODES) 10b47dee-89d8-11ed-a1eb-0242ac120002
 * * Write
 * * Wait-for-respond
 * * Expected (Driving mode, accel amount)
 * Characteristic Steering UUID: () 10b481ae-89d8-11ed-a1eb-0242ac120002
 * * Write
 * * Wait fro respond
 * * Expected (Direction, steer_percentage)
 * Unused
 * * 10b482e4-89d8-11ed-a1eb-0242ac120002
 * * 10b48410-89d8-11ed-a1eb-0242ac120002
 */

const std::string CAR_BLE_SERVICE_UUID{"10b47b82-89d8-11ed-a1eb-0242ac120002"};

/**
 * @brief Characteristic Drive mode UUID: (CHARACTERISTIC_UUID_DRIVE_MODES) 10b47dee-89d8-11ed-a1eb-0242ac120002
 * * Write
 * * Wait for respond
 * * Expected (DriveMode, gas_percentage)
 */
const std::string CHARACTERISTIC_UUID_DRIVE_MODES{"10b47dee-89d8-11ed-a1eb-0242ac120002"};

/**
 * @brief Characteristic Steering UUID: () 10b481ae-89d8-11ed-a1eb-0242ac120002
 * * Write
 * * Wait for respond
 * * Expected (Direction, steer_percentage)
 */
const std::string CHARACTERISTIC_UUID_STEERING{"10b481ae-89d8-11ed-a1eb-0242ac120002"};

enum DriveMode
{
    Forward,
    ForwardCollision,
    Backward,
    BackwardCollision,
    Stop,
    Unsupported
};

/**
 * @brief Convert drive mode to string
 *
 * @param mode
 * @return const std::string& representation of drive-mode, "not supported" if not found
 */
static const std::string mode_to_str(DriveMode mode)
{
    switch (mode)
    {
    case Forward:
        return "Forward";
    case ForwardCollision:
        return "ForwardCollision";
    case Backward:
        return "Backward";
    case BackwardCollision:
        return "BackwardCollision";
    case Stop:
        return "Stop";
    default:
        return "NotSupported";
    }
}

#endif // COMMON