#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/BLEManager.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/Car.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"

car::Car *my_car;
ble::BLEManager *ble_mgr;
bool oldDeviceConnected = false;

void setup()
{

  Serial.begin(115200);

  my_car = new car::Car();
  ble_mgr = new ble::BLEManager(my_car);

  Serial.println("Waiting a client connection to notify...");
}

void loop()
{
}