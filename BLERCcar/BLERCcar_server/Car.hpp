#ifndef CAR
#define CAR
#include <iostream>
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/Icontroller.hpp"
#include "Motor.hpp"
#include "DistanceSensor.hpp"
namespace car
{

    class Car : public Icontroller
    {
    private:
        DriveMode m_Mode{DriveMode::Stop};
        Motor m_DriveShaft, m_Steering;
        bool initMotors();
        void stop(bool zero_steer);
        void moveForward();
        void moveBackward();
        void zeroSteer();
        environment_sensing::DistanceSensor *m_LeftDistanceSensor{NULL}, *m_RightDistanceSensor{NULL};

    public:
        Car();
        ~Car() override { Serial.println("Car destructed..."); };
        void TurnLeft(const char percentage) override;
        void TurnRight(const char percentage) override;
        void SetSpeed(const char speed) override;
        void SetSpeed(const DriveMode &mode, const char speed) override{};
        void SetDriveMode(DriveMode mode) override;
        const DriveMode CurrentDriveMode() override { return m_Mode; };
        const std::string CurrentDriveModeStr() override { return mode_to_str(m_Mode); };

        /**
         * @brief Get current Distance Measurements.
         * @note Unless there will be a User for this measurements on the Server side, than it's implementation is redundant.
         * @note Expected user wil  be BLEManager
         *
         * @param in Reference to environment_sensing::DistanceMeasurements
         */
        void GetDistanceMeasurements(environment_sensing::DistanceMeasurements &in) const override;
    };
} // Car

car::Car::Car()
{
    Serial.println("Car is initializing...");
    if (initMotors())
    {
        Serial.println("Car is initialized");
        m_LeftDistanceSensor = new environment_sensing::DistanceSensor("Left", 26, 25);
        m_RightDistanceSensor = new environment_sensing::DistanceSensor("Right", 14, 27);
        return;
    }
    Serial.println("Failed to initialize");
}

bool car::Car::initMotors()
{

    // Motor A
    m_DriveShaft = Motor(0, 2, 15, 0);
    Serial.println("driveShaft initialized");
    // Motor B
    m_Steering = Motor(16, 4, 17, 1);
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
    if (m_Mode == Stop)
    {
        Serial.print("Car is stopped, cant increase speed");
        return;
    }
    // Move DC motor forward with increasing speed
    m_DriveShaft.SetSpeed(speed);
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
    digitalWrite(m_DriveShaft.Pin1, LOW);
    digitalWrite(m_DriveShaft.Pin2, LOW);
    if (zero_steer)
        zeroSteer();
}

void car::Car::moveForward()
{
    Serial.printf("Moving Forward at [%d]%%. ", m_DriveShaft.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = LOW. ", m_DriveShaft.Pin1);
    Serial.printf("Writing Pin2[%d] = HIGH.", m_DriveShaft.Pin2);
    Serial.println();
    digitalWrite(m_DriveShaft.Pin1, LOW);
    digitalWrite(m_DriveShaft.Pin2, HIGH);
}

void car::Car::moveBackward()
{
    Serial.printf("Moving Backward at [%d]%%. ", m_DriveShaft.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = HIGH. ", m_DriveShaft.Pin1);
    Serial.printf("Writing Pin2[%d] = LOW.", m_DriveShaft.Pin2);
    Serial.println();
    digitalWrite(m_DriveShaft.Pin1, HIGH);
    digitalWrite(m_DriveShaft.Pin2, LOW);
}

void car::Car::zeroSteer()
{
    Serial.println("Zeroing steering");
    digitalWrite(m_Steering.Pin1, LOW);
    digitalWrite(m_Steering.Pin2, LOW);
}

void car::Car::TurnLeft(const char percentage)
{
    if (percentage == 0)
        zeroSteer();
    m_Steering.SetSpeed(percentage);
    Serial.printf("TurnLeft [%d]%%. ", m_Steering.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = HIGH. ", m_Steering.Pin1);
    Serial.printf("Writing Pin2[%d] = LOW. ", m_Steering.Pin2);
    Serial.println();
    digitalWrite(m_Steering.Pin1, HIGH);
    digitalWrite(m_Steering.Pin2, LOW);
}
void car::Car::TurnRight(const char percentage)
{
    if (percentage == 0)
        zeroSteer();
    m_Steering.SetSpeed(percentage);
    Serial.printf("TurnRight [%d]%%. ", m_Steering.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = LOW. ", m_Steering.Pin1);
    Serial.printf("Writing Pin2[%d] = HIGH. ", m_Steering.Pin2);
    Serial.println();
    digitalWrite(m_Steering.Pin1, LOW);
    digitalWrite(m_Steering.Pin2, HIGH);
}

void car::Car::SetDriveMode(DriveMode mode)
{
    Serial.println(__PRETTY_FUNCTION__);

    switch (mode)
    {
    case Forward:
        m_Mode = mode;
        moveForward();
        break;
    case Backward:
        m_Mode = mode;
        moveBackward();
        break;
    case Stop:
        m_Mode = mode;
        break;
    case DisconnectionStop:
        m_Mode = mode;
        stop(true);
        break;
    default:
        m_Mode = DriveMode::Stop;
        stop(true);
        Serial.print("Called with unsupported mode, setting Stop");
        break;
    }
    Serial.print("Setting DriveMode: ");
    Serial.println(mode_to_str(m_Mode).c_str());
    return;
};

void car::Car::GetDistanceMeasurements(environment_sensing::DistanceMeasurements &in) const
{
    if (m_LeftDistanceSensor)
    {
        Serial.println("Sampling Left distance sensor");
        in.FrontLeft = m_LeftDistanceSensor->GetCurrentDistanceInCm();
    }
    if (m_RightDistanceSensor)
    {
        Serial.println("Sampling Right distance sensor");
        in.FrontRight = m_RightDistanceSensor->GetCurrentDistanceInCm();
    }
};
#endif // CAR