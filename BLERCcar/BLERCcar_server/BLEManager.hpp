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

    class BLEManager
    {
    private:
        MyServerCallbacks *m_pServerCallbacks{NULL};
        MyCharCallbacks *m_pMyCharCallbacks{NULL};
        BLEServer *m_pServer{NULL};
        BLECharacteristic *m_pTxCharacteristic{NULL};
        Context m_BLEManager_ctx;

    public:
        const Context &GetContext() const { return m_BLEManager_ctx; };
        BLEManager(Icontroller *controller);
        void Advertise();
        ~BLEManager();
    };

    void ConnectivityLEDStuff_t(void *pvParameters);
    /**
     * @brief Init LED board configuration & start xTask
     *
     */
    void InitConnectivityLEDStuff(BLEManager *pBLEManager)
    {
        // initialize digital pin LED_BUILTIN as an output.
        auto led = pBLEManager->GetContext().LED_PIN;
        pinMode(led, OUTPUT);

        // xTaskCreate should be in SETUP() function - if not the scheduler is not working properly, and there's a xTaskCreate shadowing.
        xTaskCreate(
            ConnectivityLEDStuff_t, // Function that should be called
            "ConnectivityLED task", // Name of the task (for debugging)
            10000,                  // Stack size (bytes)
            pBLEManager,            // Parameter to pass
            1,                      // Task priority
            NULL                    // Task handle
        );
    }

    /**
     * @brief Task of connectivity LED. This run in the background
     *
     * @param pvParameters Pointer that will (dangerously) will be casted into BLEManager
     */
    void ConnectivityLEDStuff_t(void *pvParameters)
    {
        int TICKS = 100;
        auto pBLEManager = (BLEManager *)(pvParameters);
        auto led = pBLEManager->GetContext().LED_PIN;

        for (;;)
        {
            if ((pBLEManager->GetContext()).IsDeviceConnected)
            {
                // Serial.println(__PRETTY_FUNCTION__);
                // Serial.printf("Device connected LED: %d\n", led);
                digitalWrite(led, HIGH); // turn the LED on
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
                while (!(pBLEManager->GetContext()).IsDeviceConnected)
                {
                    digitalWrite(led, HIGH); // Turn the LED on
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    digitalWrite(led, LOW); // Turn the LED off
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    digitalWrite(led, HIGH); // Turn the LED on
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    digitalWrite(led, LOW); // Turn the LED off
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
                }
            }
            vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
        }

        vTaskDelete(NULL);
    }

} // ble

void ble::BLEManager::Advertise()
{
    m_pServer->startAdvertising();
};

ble::BLEManager::BLEManager(Icontroller *controller)
{
    m_BLEManager_ctx.Controller = controller;
    InitConnectivityLEDStuff(this);

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
    // m_pTxCharacteristic = pService->createCharacteristic(
    //     CHARACTERISTIC_UUID_,
    //     BLECharacteristic::PROPERTY_NOTIFY); // See NOTIFY and other properties - https://embeddedcentric.com/lesson-2-ble-profiles-services-characteristics-device-roles-and-network-topology/

    // m_pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pReceiveDriveModeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_DRIVE_MODES,
        BLECharacteristic::PROPERTY_WRITE);

    pReceiveDriveModeCharacteristic->setCallbacks(m_pMyCharCallbacks);

    BLECharacteristic *pReceiveSteeringCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_STEERING,
        BLECharacteristic::PROPERTY_WRITE);

    pReceiveSteeringCharacteristic->setCallbacks(m_pMyCharCallbacks);

    // Start the service
    pService->start();

    // Start advertising
    m_pServer->startAdvertising();
}

ble::BLEManager::~BLEManager()
{
    delete m_pMyCharCallbacks;
    delete m_pServerCallbacks;
    delete m_pServer;
}

#endif // BLE_MANAGER