#pragma once
namespace car
{
    class Motor
    {
    private:
        int m_Enable1Pin{-1};
        // Setting PWM properties
        int m_Freq{30000};
        int m_PwmChannel{-1};
        int m_Resolution{8};
        int m_Pwmduty{200};

    public:
        int Pin1{-1};
        int Pin2{-1};

        Motor(){};
        Motor(int pin1, int pin2, int enable1Pin, int pwmChannel);
        void SetSpeed(int speed);
        int speedPercentageToDutyCycle(int speed);
        int CurrentSpeedPercentage();
    };

    Motor::Motor(){};
    Motor::Motor(int pin1, int pin2, int enable1Pin, int pwmChannel)
        : Pin1(pin1),
          Pin2(pin2),
          m_Enable1Pin(enable1Pin),
          m_PwmChannel(pwmChannel)

    {
        Serial.printf("Motor constructing:\n");
        Serial.printf("Pin1=%d\n", Pin1);
        Serial.printf("Pin2=%d\n", Pin2);
        Serial.printf("Enable1Pin=%d\n", m_Enable1Pin);
        Serial.printf("PwmChannel=%d\n", m_PwmChannel);
        // sets the pins as outputs:
        pinMode(Pin1, OUTPUT);
        pinMode(Pin2, OUTPUT);
        pinMode(m_Enable1Pin, OUTPUT);

        // configure LED PWM functionalities
        ledcSetup(m_PwmChannel, m_Freq, m_Resolution);

        // attach the channel to the GPIO to be controlled
        ledcAttachPin(m_Enable1Pin, m_PwmChannel);
        ledcWrite(m_PwmChannel, m_Pwmduty);
    };

    void Motor::SetSpeed(int speed)
    {
        m_Pwmduty = speedPercentageToDutyCycle(speed);
        Serial.println(__PRETTY_FUNCTION__);
        Serial.printf("Input speed [%d]%%\n", speed);
        Serial.printf("Setting [%d] pwmduty\n", m_Pwmduty);
        ledcWrite(m_PwmChannel, m_Pwmduty);
    }

    int Motor::speedPercentageToDutyCycle(int speed)
    {
        return map(speed, 0, 100, 0, 255);
    }
    int Motor::CurrentSpeedPercentage()
    {
        Serial.println(__PRETTY_FUNCTION__);
        Serial.printf("Pwmduty: %d\n", m_Pwmduty);
        return map(m_Pwmduty, 0, 255, 0, 100);
    }
} // car