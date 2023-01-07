#ifndef BLE_MANAGER
#define BLE_MANAGER
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/Icontroller.hpp"

namespace ble_manager
{
    bool deviceConnected;
    const char LED_PIN = 23;
    class MyServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer *pServer)
        {
            Serial.printf("Device %d, had connected\n", pServer->getConnId());
            deviceConnected = true;
        };

        void onDisconnect(BLEServer *pServer)
        {
            Serial.printf("Device %d, had disconnected\n", pServer->getConnId());
            deviceConnected = false;
            digitalWrite(LED_PIN, LOW); // turn the LED off
        }
    };

    class BLEManager : public BLECharacteristicCallbacks
    {
    private:
        Icontroller *m_Controller;
        BLEServer *m_pServer = NULL;
        BLECharacteristic *m_pTxCharacteristic;

    public:
        BLEManager(Icontroller *controller);
        void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override;
        void Advertise();
        ~BLEManager();
    };

    void ConnectivityLEDStuff_t(void *pvParameters);
    /**
     * @brief Init LED board configuration & start xTask
     *
     */
    void InitConnectivityLEDStuff(BLEManager *m_pBLEManager)
    {
        // initialize digital pin LED_BUILTIN as an output.
        pinMode(LED_PIN, OUTPUT);

        // xTaskCreate should be in SETUP() function - if not the scheduler is not working properly, and there's a xTaskCreate shadowing.
        xTaskCreate(
            ConnectivityLEDStuff_t, // Function that should be called
            "ConnectivityLED task", // Name of the task (for debugging)
            10000,                  // Stack size (bytes)
            m_pBLEManager,          // Parameter to pass
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
        auto p_BLEManager = (BLEManager *)(pvParameters);

        for (;;)
        {
            if (ble_manager::deviceConnected)
            {
                digitalWrite(LED_PIN, HIGH); // turn the LED on
            }
            /**
             * @brief Blink while waiting for connection
             *
             */
            else
            {

                /**
                 * @brief Blink
                 *
                 */
                while (!ble_manager::deviceConnected)
                {
                    digitalWrite(LED_PIN, HIGH); // Turn the LED on
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    digitalWrite(LED_PIN, LOW); // Turn the LED off
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    digitalWrite(LED_PIN, HIGH); // Turn the LED on
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    digitalWrite(LED_PIN, LOW); // Turn the LED off
                    vTaskDelay(pdMS_TO_TICKS(TICKS / 2));
                    vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
                }
            }
            vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
        }

        vTaskDelete(NULL);
    }

} // ble_manager

void ble_manager::BLEManager::Advertise()
{
    m_pServer->startAdvertising();
};

/**
 * @brief Triggered when the Controller sending commands.
 * This callback is triggered for all characteristics of this server,
 * so within i need to filter desired logic based on callback characteristic.
 *
 * @param pCharacteristic
 * @param param
 */
void ble_manager::BLEManager::onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
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
        Serial.println("onRead callback triggered with CHARACTERISTIC_UUID_STEERING!!!");
    }
}

ble_manager::BLEManager::BLEManager(Icontroller *controller) : m_Controller(controller)
{
    InitConnectivityLEDStuff(this);

    // Create the BLE Device
    BLEDevice::init("BLE_RC_Car");

    // Create the BLE Server
    m_pServer = BLEDevice::createServer();
    m_pServer->setCallbacks(new ble_manager::MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = m_pServer->createService(CAR_BLE_SERVICE_UUID);

    // Create a BLE Characteristic
    // Ill use this characetirsic to sent data back to the client
    // m_pTxCharacteristic = pService->createCharacteristic(
    //     CHARACTERISTIC_UUID_,
    //     BLECharacteristic::PROPERTY_NOTIFY); // See NOTIFY and other properties - https://embeddedcentric.com/lesson-2-ble-profiles-services-characteristics-device-roles-and-network-topology/

    // m_pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pReceiveDriveModeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_DRIVE_MODES,
        BLECharacteristic::PROPERTY_WRITE);

    pReceiveDriveModeCharacteristic->setCallbacks(this);

    BLECharacteristic *pReceiveSteeringCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_STEERING,
        BLECharacteristic::PROPERTY_WRITE);

    pReceiveSteeringCharacteristic->setCallbacks(this);

    // Start the service
    pService->start();

    // Start advertising
    m_pServer->startAdvertising();
}

ble_manager::BLEManager::~BLEManager()
{
}

#endif // BLE_MANAGER