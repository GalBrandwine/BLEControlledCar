#ifndef BLE_CALLBACKS
#define BLE_CALLBACKS
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "BLEManager_context.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"

namespace ble
{
    class MyServerCallbacks : public BLEServerCallbacks
    {
    private:
        ble_ctx &m_BLEManager_context;

    public:
        MyServerCallbacks(ble_ctx &ble_manager_context) : m_BLEManager_context{ble_manager_context} {};
        void onConnect(BLEServer *pServer)
        {
            Serial.printf("Device %d, had connected\n", pServer->getConnId());
            m_BLEManager_context.IsDeviceConnected = true;
        };

        void onDisconnect(BLEServer *pServer)
        {
            Serial.printf("Device %d, had disconnected\n", pServer->getConnId());
            Serial.println("Restarting advertising");
            m_BLEManager_context.IsDeviceConnected = false;
            m_BLEManager_context.Controller->SetDriveMode(DriveMode::Stop);
        }
    };
    class MyCharCallbacks : public BLECharacteristicCallbacks
    {
    private:
        ble_ctx &m_BLEManager_context;

    public:
        MyCharCallbacks(ble_ctx &ble_manager_context) : m_BLEManager_context{ble_manager_context} {};
        /**
         * @brief Triggered when the Controller sending commands.
         * This callback is triggered for all characteristics of this server,
         * so within i need to filter desired logic based on callback characteristic.
         *
         * @param pCharacteristic
         * @param param
         */
        void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override
        {
            Serial.println("Got onRead callback");
            Serial.println(pCharacteristic->getUUID().toString().c_str());
            if (pCharacteristic->getUUID().toString() == CHARACTERISTIC_UUID_DRIVE_MODES)
            {
                std::string mode_accel = pCharacteristic->getValue();

                if (mode_accel.length() > 0)
                {
                    // pCharacteristic->setValue() notify();
                    Serial.println("*********");
                    Serial.print("Received Value: ");
                    Serial.println(mode_accel.c_str());
                    // for (int i = 0; i < mode_accel.length(); i++)
                    //     Serial.print(rxValue[i]);

                    Serial.println();
                    Serial.println("*********");
                }
                else
                {
                    Serial.println("onRead callback triggered with no data!!!");
                }
            }

            if (pCharacteristic->getUUID().toString() == CHARACTERISTIC_UUID_STEERING)
            {

                auto raw = (char *)(pCharacteristic->getData());
                if (!raw)
                {
                    Serial.println("[warn] onRead callback triggered with no data!!!");
                    return;
                }

                if (raw[0] == 'L')
                {
                    auto packet = BLESteerLeftPacket(raw);
                    Serial.print("Got Steer left command with amount: ");
                    Serial.println(packet.GetAmount());
                    m_BLEManager_context.Controller->TurnLeft(packet.GetAmount());
                }

                if (raw[0] == 'R')
                {
                    auto packet = BLESteerRightPacket(raw);
                    Serial.print("Got Steer right command with amount: ");
                    Serial.println(packet.GetAmount());
                    m_BLEManager_context.Controller->TurnRight(packet.GetAmount());
                }

                Serial.print("Got raw value: ");
                Serial.println(raw);
                // if (raw.length() > 0)
                // {
                //     // pCharacteristic->setValue() notify();
                //     Serial.println("*********");
                //     Serial.print("Received Value: ");
                //     Serial.println(raw.c_str());
                //     // for (int i = 0; i < mode_accel.length(); i++)
                //     //     Serial.print(rxValue[i]);

                //     Serial.println();
                //     Serial.println("*********");
                // }
            }
        }
    };

} // ble

#endif // BLE_CALLBACKS