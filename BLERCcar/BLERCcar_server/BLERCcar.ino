#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/BLEManager.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/Car.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"

car::Car *my_car;
ble_manager::BLEManager *ble_mgr;
bool oldDeviceConnected = false;
void setup()
{

  Serial.begin(115200);

  my_car = new car::Car();
  ble_mgr = new ble_manager::BLEManager(my_car);

  Serial.println("Waiting a client connection to notify...");
}

void loop()
{

  if (ble_manager::deviceConnected)
  {
    // if (play_once)
    // {
    //   play_once = false;
    //   digitalWrite(LED_PIN, HIGH); // turn the LED on
    //   my_car->SetDriveMode(car::DriveMode::Forward);
    //   delay(2000);
    //   my_car->SetSpeed(80);
    //   delay(2000);
    //   my_car->SetSpeed(100);
    //   delay(2000);
    //   my_car->SetSpeed(50);
    //   delay(2000);
    //   my_car->SetDriveMode(car::DriveMode::Backward);
    //   my_car->SetSpeed(100);
    //   delay(2000);
    //   my_car->SetDriveMode(car::DriveMode::Stop);
    //   delay(2000);

    //   // Check steering accuracy
    //   my_car->TurnLeft(100);
    //   delay(2000);
    //   my_car->TurnLeft(0); // Zeroing
    //   delay(2000);
    //   my_car->TurnLeft(25);
    //   delay(2000);
    //   my_car->TurnLeft(50);
    //   delay(2000);
    //   my_car->TurnLeft(75);
    //   delay(2000);
    //   my_car->TurnLeft(100);
    //   delay(2000);
    //   my_car->TurnLeft(0); // Zeroing
    //   delay(2000);
    //   my_car->TurnRight(100);
    //   delay(2000);
    //   my_car->TurnRight(0); // Zeroing
    //   delay(2000);
    //   my_car->TurnRight(25);
    //   delay(2000);
    //   my_car->TurnRight(50);
    //   delay(2000);
    //   my_car->TurnRight(75);
    //   delay(2000);
    //   my_car->TurnRight(100);
    //   delay(2000);
    //   my_car->TurnRight(0); // Zeroing
    //   delay(2000);
    // }

    // // Serial.println("Device connected, setting value in TX");
    // pTxCharacteristic->setValue(&txValue, 1);
    // pTxCharacteristic->notify();
    // txValue++;
    // Serial.println("Chilling, not burning bluetooth stack");
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!ble_manager::deviceConnected && oldDeviceConnected)
  {
    my_car->SetDriveMode(DriveMode::Stop);

    delay(500); // give the bluetooth stack the chance to get things ready
    // pServer->startAdvertising(); // restart advertising
    ble_mgr->Advertise();
    Serial.println("start advertising");
    oldDeviceConnected = ble_manager::deviceConnected;
  }
  // connecting
  if (ble_manager::deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = ble_manager::deviceConnected;
  }
}
