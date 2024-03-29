#ifndef BLE_CALLBACKS
#define BLE_CALLBACKS
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "BLEManager_context.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/BLERCCar_packets.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"

namespace ble
{
    class MyServerCallbacks : public BLEServerCallbacks
    {
    private:
        Context &m_BLEManager_context;

    public:
        MyServerCallbacks(Context &ble_manager_context) : m_BLEManager_context{ble_manager_context} {};
        void onConnect(BLEServer *pServer)
        {
            Serial.printf("Device %d, had connected.\n ", pServer->getConnId());
            m_BLEManager_context.DeviceConnected = true;
        };

        void onDisconnect(BLEServer *pServer)
        {
            Serial.printf("Device %d, had disconnected\n", pServer->getConnId());
            Serial.println("Restarting advertising");
            m_BLEManager_context.DisconnectionTime = millis();
            m_BLEManager_context.DeviceConnected = false;
            m_BLEManager_context.Controller->SetDriveMode(DriveMode::DisconnectionStop);
        }
    };
    class MyCharCallbacks : public BLECharacteristicCallbacks
    {
    private:
        Context &m_BLEManager_context;

    public:
        MyCharCallbacks(Context &ble_manager_context) : m_BLEManager_context{ble_manager_context} {};
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
            Serial.println();
            Serial.print("Got onWrite callback. UUID: ");
            Serial.println(pCharacteristic->getUUID().toString().c_str());
            if (pCharacteristic->getUUID().toString() == CHARACTERISTIC_UUID_SET_DRIVE_MODES)
            {
                auto raw = (char *)(pCharacteristic->getData());
                if (!raw)
                {
                    Serial.println("[warn][CHARACTERISTIC_UUID_SET_DRIVE_MODES] onRead callback triggered with no data!!!");
                    return;
                }

                BLEDrivePacket packet{raw};
                if (packet.GetDriveMode() == DriveMode::Unsupported)
                {
                    Serial.printf("[warn][CHARACTERISTIC_UUID_SET_DRIVE_MODES] onRead callback triggered with UNSUPPORTED DRIVEMODE: %d !!!\n", raw[0]);
                    m_BLEManager_context.Controller->SetDriveMode(DriveMode::Unsupported);
                    return;
                }
                Serial.printf("Got drive mode: %s [raw: %d] amount: %d .\n", mode_to_str(packet.GetDriveMode()).c_str(), raw[0], packet.GetAmount());

                m_BLEManager_context.Controller->SetDriveMode(packet.GetDriveMode());
                m_BLEManager_context.Controller->SetSpeed(packet.GetAmount());

                Serial.print("Raw value is: ");
                Serial.println(raw);
                return;
            }

            if (pCharacteristic->getUUID().toString() == CHARACTERISTIC_UUID_STEERING)
            {

                auto raw = (char *)(pCharacteristic->getData());
                if (!raw)
                {
                    Serial.println("[warn][CHARACTERISTIC_UUID_STEERING] onRead callback triggered with no data!!!");
                    return;
                }

                if (raw[0] == 'L')
                {
                    BLESteerLeftPacket packet{raw};
                    Serial.print("Got Steer left command with amount: ");
                    Serial.println(packet.GetAmount(),DEC);
                    m_BLEManager_context.Controller->TurnLeft(packet.GetAmount());
                }

                if (raw[0] == 'R')
                {
                    BLESteerRightPacket packet{raw};
                    Serial.print("Got Steer right command with amount: ");
                    Serial.println(packet.GetAmount(),DEC);
                    m_BLEManager_context.Controller->TurnRight(packet.GetAmount());
                }

                Serial.print("Raw value is: ");
                Serial.println(raw);
                return;
            }
        }
    };

} // ble

#endif // BLE_CALLBACKS