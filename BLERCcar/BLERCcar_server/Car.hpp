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
        DriveMode m_Mode;
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
         * @return const environment_sensing::DistanceMeasurements
         */
        const environment_sensing::DistanceMeasurements GetDistanceMeasurements() const override;
    };
} // Car

car::Car::Car()
{
    Serial.println("Car is initializing...");
    if (initMotors())
    {
        Serial.println("Car is initialized");
        m_LeftDistanceSensor = new environment_sensing::DistanceSensor("Left", 36, 39);
        // m_RightDistanceSensor = new DistanceSensor();
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
    Serial.println(__PRETTY_FUNCTION__);
    Serial.printf("Moving Forward at [%d]%%\n", m_DriveShaft.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = LOW\n", m_DriveShaft.Pin1);
    Serial.printf("Writing Pin2[%d] = HIGH\n", m_DriveShaft.Pin2);
    digitalWrite(m_DriveShaft.Pin1, LOW);
    digitalWrite(m_DriveShaft.Pin2, HIGH);
}

void car::Car::moveBackward()
{
    Serial.printf("Moving Backward at [%d]%\n", m_DriveShaft.CurrentSpeedPercentage());
    digitalWrite(m_DriveShaft.Pin1, HIGH);
    digitalWrite(m_DriveShaft.Pin2, LOW);
    delay(2000);
}

void car::Car::zeroSteer()
{
    digitalWrite(m_Steering.Pin1, LOW);
    digitalWrite(m_Steering.Pin2, LOW);
}

void car::Car::TurnLeft(const char percentage)
{
    if (percentage == 0)
        zeroSteer();
    Serial.println(__PRETTY_FUNCTION__);
    m_Steering.SetSpeed(percentage);
    Serial.printf("TurnLeft [%d]%%\n", m_Steering.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = HIGH\n", m_Steering.Pin1);
    Serial.printf("Writing Pin2[%d] = LOW\n", m_Steering.Pin2);
    digitalWrite(m_Steering.Pin1, HIGH);
    digitalWrite(m_Steering.Pin2, LOW);
}
void car::Car::TurnRight(const char percentage)
{
    if (percentage == 0)
        zeroSteer();
    Serial.println(__PRETTY_FUNCTION__);
    m_Steering.SetSpeed(percentage);
    Serial.printf("TurnRight [%d]%%\n", m_Steering.CurrentSpeedPercentage());
    Serial.printf("Writing Pin1[%d] = LOW\n", m_Steering.Pin1);
    Serial.printf("Writing Pin2[%d] = HIGH\n", m_Steering.Pin2);
    digitalWrite(m_Steering.Pin1, LOW);
    digitalWrite(m_Steering.Pin2, HIGH);
}

void car::Car::SetDriveMode(DriveMode mode)
{
    Serial.println(__PRETTY_FUNCTION__);
    if (mode != m_Mode)
    {

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
            stop(true);
            break;
        default:
            m_Mode = DriveMode::Stop;
            Serial.print("Called with unsupported mode, setting Stop");
            break;
        }
        Serial.print("Setting DriveMode: ");
        Serial.println(mode_to_str(m_Mode).c_str());
        return;
    }

    Serial.printf("Called with mode: %s. But mode is already: %s\n", mode_to_str(mode).c_str(), mode_to_str(m_Mode).c_str());
};

const environment_sensing::DistanceMeasurements car::Car::GetDistanceMeasurements() const
{
    environment_sensing::DistanceMeasurements d;
    if (m_LeftDistanceSensor)
    {
        Serial.println("Sampling Left distance sensor");
        d.FrontLeft = m_LeftDistanceSensor->GetCurrentDistanceInCm();
    }
    if (m_RightDistanceSensor)
    {
        Serial.println("Sampling Right distance sensor");
        d.FrontRight = m_RightDistanceSensor->GetCurrentDistanceInCm();
    }
    return d;
};
#endif // CAR