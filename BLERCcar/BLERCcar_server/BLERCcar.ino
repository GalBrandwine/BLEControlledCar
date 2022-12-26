#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/Car.hpp"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

car::Car *my_car;
const char LED_PIN = 23;
BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool client_connected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// These UUID's are NUS (Nordic Uart Service) https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.4.0/nrf/include/bluetooth/services/nus.html#api-documentation
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.printf("Device %d, had connected\n", pServer->getConnId());
    deviceConnected = true;
    // client_connected = true; // turn the LED on
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.printf("Device %d, had disconnected\n", pServer->getConnId());
    deviceConnected = false;
    // client_connected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      Serial.println("*********");
      Serial.print("Received Value: ");
      Serial.println(rxValue.c_str());
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);

      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup()
{

  Serial.begin(115200);

  my_car = new car::Car();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("UART Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY); // See NOTIFY and other properties - https://embeddedcentric.com/lesson-2-ble-profiles-services-characteristics-device-roles-and-network-topology/

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

bool play_once = true;
void loop()
{

  if (deviceConnected)
  {
    if (play_once)
    {
      play_once = false;
      digitalWrite(LED_PIN, HIGH); // turn the LED on
      my_car->SetDriveMode(car::DriveMode::Forward);
      delay(2000);
      my_car->SetSpeed(80);
      delay(2000);
      my_car->SetSpeed(100);
      delay(2000);
      my_car->SetSpeed(50);
      delay(2000);
      my_car->SetDriveMode(car::DriveMode::Backward);
      my_car->SetSpeed(100);
      delay(2000);
      my_car->SetDriveMode(car::DriveMode::Stop);
      delay(2000);

      // Check steering accuracy
      my_car->TurnLeft(100);
      delay(2000);
      my_car->TurnLeft(0); // Zeroing
      delay(2000);
      my_car->TurnLeft(25);
      delay(2000);
      my_car->TurnLeft(50);
      delay(2000);
      my_car->TurnLeft(75);
      delay(2000);
      my_car->TurnLeft(100);
      delay(2000);
      my_car->TurnLeft(0); // Zeroing
      delay(2000);
      my_car->TurnRight(100);
      delay(2000);
      my_car->TurnRight(0); // Zeroing
      delay(2000);
      my_car->TurnRight(25);
      delay(2000);
      my_car->TurnRight(50);
      delay(2000);
      my_car->TurnRight(75);
      delay(2000);
      my_car->TurnRight(100);
      delay(2000);
      my_car->TurnRight(0); // Zeroing
      delay(2000);
    }

    // Serial.println("Device connected, setting value in TX");
    pTxCharacteristic->setValue(&txValue, 1);
    pTxCharacteristic->notify();
    txValue++;
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    my_car->SetDriveMode(car::DriveMode::Stop);
    digitalWrite(LED_PIN, LOW); // turn the LED off

    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
