#ifndef COMMON
#define COMMON
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
 * 10b482e4-89d8-11ed-a1eb-0242ac120002
 * Unused
 * 10b48410-89d8-11ed-a1eb-0242ac120002
 */

const std::string CAR_BLE_SERVICE_UUID{"10b47b82-89d8-11ed-a1eb-0242ac120002"};
const std::string CHARACTERISTIC_UUID_DRIVE_MODES{"10b47dee-89d8-11ed-a1eb-0242ac120002"};
const std::string CHARACTERISTIC_UUID_STEERING{"10b481ae-89d8-11ed-a1eb-0242ac120002"};

enum DriveMode
{
    Forward,
    ForwardCollision,
    Backward,
    BackwardCollision,
    Stop

};

#endif // COMMON