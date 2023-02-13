#ifndef COMMON
#define COMMON
#include <iostream>
#include "DistanceSensor_data.hpp"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

/**
 * @brief Bulk of my own UUID's
 * Car service UUID: 2c6702d8-a1b0-11ed-a8fc-0242ac120002
 * Characteristic Drive mode UUID: (CHARACTERISTIC_UUID_DRIVE_MODES) 2c670756-a1b0-11ed-a8fc-0242ac120002
 * * Write
 * * Wait-for-respond
 * * Expected (Driving mode, accel amount)
 * Characteristic Steering UUID: () 2c6708d2-a1b0-11ed-a8fc-0242ac120002
 * * Write
 * * Wait fro respond
 * * Expected (Direction, steer_percentage)
 * Characteristic front left distance cm UUID: () 2c670594-a1b0-11ed-a8fc-0242ac120002
 * * Notify
 * Characteristic front right distance cm UUID: () 2c670a12-a1b0-11ed-a8fc-0242ac120002
 * * Notify
 * Unused
 * *
 * * 2c670c60-a1b0-11ed-a8fc-0242ac120002
 * * 2c670d82-a1b0-11ed-a8fc-0242ac120002
 * * 2c67119c-a1b0-11ed-a8fc-0242ac120002
 * * 2c6712d2-a1b0-11ed-a8fc-0242ac120002
 */

const std::string CAR_BLE_SERVICE_UUID{"2c6702d8-a1b0-11ed-a8fc-0242ac120002"};

/**
 * @brief Characteristic front right distance cm UUID: () 2c670a12-a1b0-11ed-a8fc-0242ac120002
 * * Notify
 */
const std::string CHARACTERISTIC_UUID_FRONT_RIGHT_DISTANCE_CM{"2c670a12-a1b0-11ed-a8fc-0242ac120002"};

/**
 * @brief Characteristic front left distance cm UUID: () 2c670594-a1b0-11ed-a8fc-0242ac120002
 * * Notify
 */
const std::string CHARACTERISTIC_UUID_FRONT_LEFT_DISTANCE_CM{"2c670594-a1b0-11ed-a8fc-0242ac120002"};

/**
 * @brief Characteristic Drive mode UUID: (CHARACTERISTIC_UUID_DRIVE_MODES) 2c670756-a1b0-11ed-a8fc-0242ac120002
 * * Write
 * * Wait for respond
 * * Expected (DriveMode, gas_percentage)
 */
const std::string CHARACTERISTIC_UUID_SET_DRIVE_MODES{"2c670756-a1b0-11ed-a8fc-0242ac120002"};

/**
 * @brief Characteristic Drive mode UUID: (CHARACTERISTIC_UUID_GET_DRIVE_MODES) 2c670b48-a1b0-11ed-a8fc-0242ac120002
 * * Notify
 */
const std::string CHARACTERISTIC_UUID_CURRENT_DRIVE{"2c670b48-a1b0-11ed-a8fc-0242ac120002"};

/**
 * @brief Characteristic Steering UUID: () 2c6708d2-a1b0-11ed-a8fc-0242ac120002
 * * Write
 * * Wait for respond
 * * Expected (Direction, steer_percentage)
 */
const std::string CHARACTERISTIC_UUID_STEERING{"2c6708d2-a1b0-11ed-a8fc-0242ac120002"};

enum DriveMode
{
    Forward = 1,
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
        return "Unsupported";
    }
}

#endif // COMMON