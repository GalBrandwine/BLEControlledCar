#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/BLEManager.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/BLERCcar_server/Car.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"

car::Car *my_car;
ble::BLEManager *ble_mgr;
bool oldDeviceConnected = false;

// For debug
environment_sensing::DistanceMeasurements distanceMeasurements;
void setup()
{

  Serial.begin(115200);

  my_car = new car::Car();
  ble_mgr = new ble::BLEManager(my_car);
  Serial.println("Waiting a client connection to notify...");
}

void loop()
{
  // ble_mgr->GetContext().Controller->GetDistanceMeasurements(distanceMeasurements);
  // // Serial.print(__PRETTY_FUNCTION__);
  // Serial.print(" FrontLeft value: ");
  // Serial.print(distanceMeasurements.FrontLeft);
  // Serial.print(" FrontRight value: ");
  // Serial.println(distanceMeasurements.FrontRight);
  // sleep(0.5);
}