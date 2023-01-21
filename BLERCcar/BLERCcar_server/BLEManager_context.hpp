#ifndef BLE_MANAGER_CTX
#define BLE_MANAGER_CTX
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"
namespace ble
{
    struct ble_ctx
    {
        const char LED_PIN = 23;
        bool IsDeviceConnected{false};
        Icontroller *Controller;
        // Data members

    } // types, *types
    ;
} // ble

#endif // BLE_MANAGER_CTX