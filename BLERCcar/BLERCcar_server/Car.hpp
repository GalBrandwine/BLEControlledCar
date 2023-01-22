#ifndef CAR
#define CAR
#include <iostream>
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"
#include "Motor.hpp"

namespace car
{

    class Car : public Icontroller
    {
    private:
        DriveMode m_mode;
        Motor driveShaft, steering;
        bool initMotors();
        void stop(bool zero_steer);
        void moveForward();
        void moveBackward();
        void zeroSteer();

    public:
        Car();
        ~Car() override { Serial.println("Car destructed..."); };
        void TurnLeft(const char percentage) override;
        void TurnRight(const char percentage) override;
        void SetSpeed(const char speed) override;
        void SetSpeed(const DriveMode &mode, const char speed) override{};
        void SetDriveMode(DriveMode mode) override;
        const DriveMode CurrentDriveMode() override { return m_mode; };
        const std::string CurrentDriveModeStr() override { return mode_to_str(m_mode); };
    };
} // Car

car::Car::Car()
{
    Serial.println("Car is initializing...");
    if (initMotors())
    {
        Serial.println("Car is initialized");
        return;
    }
    Serial.println("Failed to initialize");
}

bool car::Car::initMotors()
{

    // Motor A
    driveShaft = Motor(0, 2, 15, 0);
    Serial.println("driveShaft initialized");
    // Motor B
    steering = Motor(16, 4, 17, 1);
    Serial.println("steering initialized");
    return true;
};

/**
 * @brief Set car speed (regardless of direction)
 *
 * @param speed percentage value [0,100]
 */
void car::Car::SetSpeed(const char speed)
{
    if (m_mode == Stop)
    {
        Serial.print("Car is stopped, cant increase speed");
        return;
    }
    // Move DC motor forward with increasing speed
    driveShaft.SetSpeed(speed);
}

/**
 * @brief Stops the car, turn off driveshaft power
 *
 * @param zero_steer if true - align wheels as well
 */
void car::Car::stop(bool zero_steer = false)
{
    // Stop the DC motor
    Serial.println("Stopping driveShaft");
    digitalWrite(driveShaft.Pin1, LOW);
    digitalWrite(driveShaft.Pin2, LOW);
    if (zero_steer)
        zeroSteer();
}

void car::Car::moveForward()
{
    Serial.println(__PRETTY_FUNCTION__);
    Serial.printf("Moving Forward at [%d]%%\n", driveShaft.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = LOW\n", driveShaft.Pin1);
    Serial.printf("Writing Pin2[%d] = HIGH\n", driveShaft.Pin2);
    digitalWrite(driveShaft.Pin1, LOW);
    digitalWrite(driveShaft.Pin2, HIGH);
}

void car::Car::moveBackward()
{
    Serial.printf("Moving Backward at [%d]%\n", driveShaft.CurrentSpeedPercentage());
    digitalWrite(driveShaft.Pin1, HIGH);
    digitalWrite(driveShaft.Pin2, LOW);
    delay(2000);
}

void car::Car::zeroSteer()
{
    digitalWrite(steering.Pin1, LOW);
    digitalWrite(steering.Pin2, LOW);
}

void car::Car::TurnLeft(const char percentage)
{
    if (percentage == 0)
        zeroSteer();
    Serial.println(__PRETTY_FUNCTION__);
    steering.SetSpeed(percentage);
    Serial.printf("TurnLeft [%d]%%\n", steering.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = HIGH\n", steering.Pin1);
    Serial.printf("Writing Pin2[%d] = LOW\n", steering.Pin2);
    digitalWrite(steering.Pin1, HIGH);
    digitalWrite(steering.Pin2, LOW);
}
void car::Car::TurnRight(const char percentage)
{
    if (percentage == 0)
        zeroSteer();
    Serial.println(__PRETTY_FUNCTION__);
    steering.SetSpeed(percentage);
    Serial.printf("TurnRight [%d]%%\n", steering.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = LOW\n", steering.Pin1);
    Serial.printf("Writing Pin2[%d] = HIGH\n", steering.Pin2);
    digitalWrite(steering.Pin1, LOW);
    digitalWrite(steering.Pin2, HIGH);
}

void car::Car::SetDriveMode(DriveMode mode)
{
    Serial.println(__PRETTY_FUNCTION__);
    if (mode != m_mode)
    {

        switch (mode)
        {
        case Forward:
            m_mode = mode;
            moveForward();
            break;
        case Backward:
            m_mode = mode;
            moveBackward();
            break;
        case Stop:
            m_mode = mode;
            stop(true);
            break;
        default:
            m_mode = DriveMode::Stop;
            Serial.print("Called with unsupported mode, setting Stop");
            break;
        }
        Serial.print("Setting DriveMode: ");
        Serial.println(mode_to_str(m_mode).c_str());
        return;
    }

    Serial.printf("Called with mode: %s. But mode is already: %s\n", mode_to_str(mode).c_str(), mode_to_str(m_mode).c_str());
};

#endif // CAR