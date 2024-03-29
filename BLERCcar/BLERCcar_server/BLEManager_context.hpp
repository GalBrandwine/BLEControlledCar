#ifndef BLE_MANAGER_CTX
#define BLE_MANAGER_CTX
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"
namespace ble
{
    struct Context
    {
        const char LED_PIN = 23;
        unsigned long DisconnectionTime{0};
        bool DeviceConnected{false};
        Icontroller *Controller;

    } // types, *types
    ;
} // ble

#endif // BLE_MANAGER_CTX