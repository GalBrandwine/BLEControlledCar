#ifndef DISTANCE_SENSOR
#define DISTANCE_SENSOR

// define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

namespace environment_sensing
{
    struct DistanceMeasurements
    {
        long FrontLeft{0}, FrontRight{0};
    };

    class DistanceSensor
    {

    private:
        int m_TrigPin; // = 36; // 14; // 36
        int m_EchoPin; // = 39; // 12; // 39
        String m_TaskName;

        unsigned long m_RawDistance{0};

    public:
        DistanceSensor(const char *SensorName, const int TrigPin, const int EchoPin);
        ~DistanceSensor();
        const unsigned long GetCurrentDistanceInCm();
    };

    DistanceSensor::~DistanceSensor()
    {
    }

    DistanceSensor::DistanceSensor(const char *SensorName, const int TrigPin, const int EchoPin)
        : m_TaskName(SensorName),
          m_TrigPin(TrigPin),
          m_EchoPin(EchoPin)
    {
        Serial.printf("Distance sensor %s initializing...\n", m_TaskName.c_str());
        Serial.printf("TrigPin: %d\nEchoPin: %s", m_TrigPin, m_EchoPin);
        pinMode(m_TrigPin, OUTPUT); // Sets the trigPin as an Output
        pinMode(m_EchoPin, INPUT);  // Sets the echoPin as an Input
        Serial.println("Distance sensor pins are set...");
    }

    /**
     * @brief Return distance in CM
     *
     * @return const unsigned long Distance. or 0 if not running
     */
    const unsigned long DistanceSensor::GetCurrentDistanceInCm()
    {
        // Serial.println(__PRETTY_FUNCTION__);
        // Clears the trigPin
        digitalWrite(m_TrigPin, LOW);
        delayMicroseconds(2);
        // Sets the trigPin on HIGH state for 10 micro seconds
        digitalWrite(m_TrigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(m_TrigPin, LOW);
        // Reads the echoPin, returns the sound wave travel time in microseconds
        long duration = pulseIn(m_EchoPin, HIGH);
        // Calculate the distance
        m_RawDistance = duration * SOUND_SPEED;
        return m_RawDistance / 2.0f;
    }

} // environment_sensing
#endif // DISTANCE_SENSOR