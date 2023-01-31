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
  // // Clears the trigPin
  // digitalWrite(trigPin, LOW);
  // delayMicroseconds(2);
  // // Sets the trigPin on HIGH state for 10 micro seconds
  // digitalWrite(trigPin, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPin, LOW);
  // // Reads the echoPin, returns the sound wave travel time in microseconds
  // duration = pulseIn(echoPin, HIGH);
  // // Calculate the distance
  // distanceCm = duration * SOUND_SPEED / 2.0f;

  // // Convert to inches
  // distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance in the Serial Monitor
  // Serial.print("Distance (cm): ");
  // Serial.println(distanceCm);
  // Serial.print("Distance (inch): ");
  // Serial.println(distanceInch);

  // delay(1000);

  /**
   * @brief When connected, sample distance in 100hz
   *
   */
  if (ble_mgr->GetContext().IsDeviceConnected)
  {
    // auto distance_measurements = my_car->GetDistanceMeasurements();
    // Serial.print("FrontLeft: ");
    // Serial.println(distance_measurements.FrontLeft);
    // Serial.print("FrontRight: ");
    // Serial.println(distance_measurements.FrontRight);
    delay(100); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!ble_mgr->GetContext().IsDeviceConnected && oldDeviceConnected)
  {
    my_car->SetDriveMode(DriveMode::Stop);
    delay(500); // give the bluetooth stack the chance to get things ready
    // pServer->startAdvertising(); // restart advertising
    ble_mgr->Advertise();
    Serial.println("start advertising");
    oldDeviceConnected = ble_mgr->GetContext().IsDeviceConnected;
  }
  // connecting
  if (ble_mgr->GetContext().IsDeviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = ble_mgr->GetContext().IsDeviceConnected;
  }

  // delay(500); // Avoid burnout
}