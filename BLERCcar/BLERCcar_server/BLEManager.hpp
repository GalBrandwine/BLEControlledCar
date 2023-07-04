#ifndef BLE_MANAGER
#define BLE_MANAGER
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "BLEManager_context.hpp"
#include "BLECallbacks.hpp"

namespace ble
{
    auto MAXIMUM_DISCONNECTION_TIMEOUT_MS = 10000;

    // Free func
    void CoolBlink(char led_pin, int TICKS)
    {
        digitalWrite(led_pin, HIGH); // Turn the LED on
        vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
        digitalWrite(led_pin, LOW); // Turn the LED off
        vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
        digitalWrite(led_pin, HIGH); // Turn the LED on
        vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
        digitalWrite(led_pin, LOW); // Turn the LED off
        vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
        vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
    }
    class BLEManager
    {
    private:
        MyServerCallbacks *m_pServerCallbacks{NULL};
        MyCharCallbacks *m_pMyCharCallbacks{NULL};
        BLEServer *m_pServer{NULL};
        BLECharacteristic
            *m_pFrontRightDistanceCharacteristic{NULL},
            *m_pFrontLeftDistanceCharacteristic{NULL},
            *m_pReceiveDriveModeCharacteristic{NULL},
            *m_pReceiveSteeringCharacteristic{NULL},
            *m_pCurrentDriveModeCharacteristic{NULL};
        Context m_BLEManager_ctx;

    public:
        const Context &GetContext() const { return m_BLEManager_ctx; };
        BLEManager(Icontroller *controller);
        void Advertise();
        void NotifyNewDistanceRead(environment_sensing::DistanceMeasurements &distanceMeasurements);
        void NotifyNewDriveMode(const BLEDrivePacket &pkt);
        ~BLEManager();
    };

    /**
     * @brief Manage disconnection timeout logic
     *
     * @param pBLEManager
     */
    void manage_disconnection_timeout(ble::BLEManager *pBLEManager)
    {
        auto CurrentTime = millis();
        auto ElapsedTime = CurrentTime - pBLEManager->GetContext().DisconnectionTime;
        if (ElapsedTime > MAXIMUM_DISCONNECTION_TIMEOUT_MS)
        {
            // Serial.print("Time passed since disconnection: ");
            // Serial.println(ElapsedTime);
            auto current_drive_mode = pBLEManager->GetContext().Controller->CurrentDriveMode();
            if (current_drive_mode != DriveMode::DisconnectionStop)
            {
                pBLEManager->GetContext().Controller->SetDriveMode(DriveMode::DisconnectionStop);
            }
        }
    }

    void ConnectivityManagementTask_t(void *pvParameters);
    /**
     * @brief Init LED board configuration & start xTask
     *
     */
    void InitConnectivityMaintenanceTask(BLEManager *pBLEManager)
    {
        // initialize digital pin LED_BUILTIN as an output.
        auto led = pBLEManager->GetContext().LED_PIN;
        pinMode(led, OUTPUT);

        // xTaskCreate should be in SETUP() function - if not the scheduler is not working properly, and there's a xTaskCreate shadowing.
        xTaskCreate(
            ConnectivityManagementTask_t, // Function that should be called
            "ConnectivityLED task",       // Name of the task (for debugging)
            10000,                        // Stack size (bytes)
            pBLEManager,                  // Parameter to pass
            1,                            // Task priority
            NULL                          // Task handle
        );
    }

    /**
     * @brief Task of connectivity LED. This run in the background
     *
     * @param pvParameters Pointer that will (dangerously) will be casted into BLEManager
     */
    void ConnectivityManagementTask_t(void *pvParameters)
    {
        int TICKS = 100;
        auto pBLEManager = (BLEManager *)(pvParameters);
        auto led = pBLEManager->GetContext().LED_PIN;

        for (;;)
        {
            if ((pBLEManager->GetContext()).DeviceConnected)
            {
                // Serial.println(__PRETTY_FUNCTION__);
                // Serial.printf("Device connected LED: %d\n", led);
                digitalWrite(led, HIGH); // turn the LED on
                DriveMode tmp_drive_mode{DriveMode::InitialConenction};

                // Create one measurement and update its internal
                environment_sensing::DistanceMeasurements measurement;
                /**
                 * @brief Send distance measurements in 10hz
                 *
                 */
                // Block for 100ms.
                const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
                while ((pBLEManager->GetContext()).DeviceConnected)
                {
                    // Send new drive mode back to Client.
                    auto current_drive_mode = pBLEManager->GetContext().Controller->CurrentDriveMode();
                    if (current_drive_mode != tmp_drive_mode)
                    {
                        Serial.println("current_drive_mode != tmp_drive_mode");
                        Serial.printf("current_drive_mode: %d [%s]\n", current_drive_mode, mode_to_str(current_drive_mode).c_str());
                        Serial.printf("tmp_drive_mode: %d [%s]\n", current_drive_mode, mode_to_str(tmp_drive_mode).c_str());
                        tmp_drive_mode = current_drive_mode;
                        BLEDrivePacket p{current_drive_mode, 0};
                        pBLEManager->NotifyNewDriveMode(p);
                    }

                    pBLEManager->GetContext().Controller->GetDistanceMeasurements(measurement);
                    pBLEManager->NotifyNewDistanceRead(measurement);

                    // Sleep for releasing CPU
                    vTaskDelay(xDelay);
                }
                Serial.println("Controller disconnected");
                pBLEManager->GetContext().Controller->SetDriveMode(DriveMode::DisconnectionStop);
                pBLEManager->Advertise();
            }
            /**
             * @brief Blink while waiting for connection
             *
             */
            else
            {
                // Serial.println(__PRETTY_FUNCTION__);
                // Serial.printf("Waiting for connection LED: %d\n", led);
                /**
                 * @brief Blink
                 *
                 */
                while (!(pBLEManager->GetContext()).DeviceConnected)
                {
                    CoolBlink(led, TICKS);

                    manage_disconnection_timeout(pBLEManager);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
        }

        vTaskDelete(NULL);
    }

} // ble

void ble::BLEManager::NotifyNewDriveMode(const BLEDrivePacket &pkt)
{
    // Serial.println(__PRETTY_FUNCTION__);
    Serial.print("Setting DriveMode value: ");
    Serial.println(mode_to_str(pkt.GetDriveMode()).c_str());
    m_pCurrentDriveModeCharacteristic->setValue(pkt.GetPayload());
    m_pCurrentDriveModeCharacteristic->notify();
}

void ble::BLEManager::NotifyNewDistanceRead(environment_sensing::DistanceMeasurements &distanceMeasurements)
{
    Serial.print(__PRETTY_FUNCTION__);
    Serial.print(" FrontLeft value: ");
    Serial.print(distanceMeasurements.FrontLeft);
    Serial.print(" FrontRight value: ");
    Serial.println(distanceMeasurements.FrontRight);
    m_pFrontLeftDistanceCharacteristic->setValue(distanceMeasurements.FrontLeft);
    m_pFrontLeftDistanceCharacteristic->notify();
    m_pFrontRightDistanceCharacteristic->setValue(distanceMeasurements.FrontRight);
    m_pFrontRightDistanceCharacteristic->notify();
};

void ble::BLEManager::Advertise()
{
    Serial.println("Asking BLE server to resume advertising");
    m_pServer->startAdvertising();
};

ble::BLEManager::BLEManager(Icontroller *controller)
{
    m_BLEManager_ctx.Controller = controller;
    InitConnectivityMaintenanceTask(this);

    // Create the BLE Device
    BLEDevice::init("BLE_RC_Car");

    // Create the BLE Char Callbacks
    m_pMyCharCallbacks = new MyCharCallbacks(m_BLEManager_ctx);
    // Create the BLE Server Callbacks
    m_pServerCallbacks = new MyServerCallbacks(m_BLEManager_ctx);
    // Create the BLE Server
    m_pServer = BLEDevice::createServer();
    m_pServer->setCallbacks(m_pServerCallbacks);

    // Create the BLE Service
    BLEService *pService = m_pServer->createService(CAR_BLE_SERVICE_UUID);

    // Create a BLE Characteristic
    // Ill use this characteristic to sent data back to the client
    m_pCurrentDriveModeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_CURRENT_DRIVE,
        BLECharacteristic::PROPERTY_NOTIFY);
    m_pFrontLeftDistanceCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_FRONT_LEFT_DISTANCE_CM,
        BLECharacteristic::PROPERTY_NOTIFY); // See NOTIFY and other properties - https://embeddedcentric.com/lesson-2-ble-profiles-services-characteristics-device-roles-and-network-topology/

    m_pFrontLeftDistanceCharacteristic->addDescriptor(new BLE2902());

    m_pFrontRightDistanceCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_FRONT_RIGHT_DISTANCE_CM,
        BLECharacteristic::PROPERTY_NOTIFY);
    m_pFrontRightDistanceCharacteristic->addDescriptor(new BLE2902());

    m_pReceiveDriveModeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SET_DRIVE_MODES,
        BLECharacteristic::PROPERTY_WRITE);

    m_pReceiveDriveModeCharacteristic->setCallbacks(m_pMyCharCallbacks);

    m_pReceiveSteeringCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_STEERING,
        BLECharacteristic::PROPERTY_WRITE);

    m_pReceiveSteeringCharacteristic->setCallbacks(m_pMyCharCallbacks);

    // Start the service
    pService->start();

    // Start advertising
    m_pServer->startAdvertising();
}

ble::BLEManager::~BLEManager()
{
    delete m_pReceiveDriveModeCharacteristic;
    delete m_pReceiveSteeringCharacteristic;
    delete m_pFrontLeftDistanceCharacteristic;
    delete m_pFrontRightDistanceCharacteristic;
    delete m_pMyCharCallbacks;
    delete m_pServerCallbacks;
    delete m_pServer;
}

#endif // BLE_MANAGER