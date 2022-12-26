#pragma once
#include <iostream>
#include "Motor.hpp"

namespace car
{
    enum DriveMode
    {
        Forward,
        ForwardCollision,
        Backward,
        BackwardCollision,
        Stop

    };

    const std::string mode_to_str(DriveMode mode)
    {
        switch (mode)
        {
        case Forward:
            return "Forward";
        case ForwardCollision:
            return "ForwardCollision";
        case Backward:
            return "Backward";
        case BackwardCollision:
            return "BackwardCollision";
        case Stop:
            return "Stop";
        default:
            return "NotSupported";
        }
    }

    class Car
    {
    private:
        DriveMode m_mode;
        Motor driveShaft, steering;
        bool initMotors();
        void stop();
        void moveForward();
        void moveBackward();
        void zeroSteer();

    public:
        Car();
        void TurnLeft(int percentage);
        void TurnRight(int percentage);
        void SetSpeed(int speed);

        void SetDriveMode(DriveMode mode);
        const DriveMode CurrentDriveMode() { return m_mode; };
        const std::string CurrentDriveModeStr() { return mode_to_str(m_mode); };
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
void car::Car::SetSpeed(int speed)
{
    if (m_mode == Stop)
    {
        Serial.print("Car is stopped, cant increase speed");
        return;
    }
    // Move DC motor forward with increasing speed
    driveShaft.SetSpeed(speed);
}

void car::Car::stop(bool zero_steer = false)
{
    // Stop the DC motor
    Serial.println("Stopping driveShaft");
    digitalWrite(driveShaft.Pin1, LOW);
    digitalWrite(driveShaft.Pin2, LOW);
    if (zero_steer)
        zeroSteer()
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

void car::Car::TurnLeft(int percentage)
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
void car::Car::TurnRight(int percentage)
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
    if (mode != m_mode)
    {
        m_mode = mode;
        switch (mode)
        {
        case Forward:
            moveForward();
            break;
        case Backward:
            moveBackward();
            break;
        case Stop:
            stop(true);
            break;
        default:
            break;
        }
        Serial.print("Setting DriveMode: ");
        Serial.println(mode_to_str(m_mode).c_str());
    }
};