#ifndef DISTANCE_SENSOR
#define DISTANCE_SENSOR

namespace environment_sensing
{

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
        Serial.print("Distance sensor ");
        Serial.print(m_TaskName);
        Serial.println("initializing...");
        Serial.print("TrigPin: ");
        Serial.println(m_TrigPin);
        Serial.print("EchoPin: ");
        Serial.println(m_EchoPin);
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
        Serial.print(m_TaskName.c_str());
        Serial.print(" Trig pin: ");
        Serial.print(m_TrigPin);
        Serial.print(" Echo pin: ");
        Serial.print(m_EchoPin);
        Serial.println();
        // Serial.println(" sensor getting measurement...");
        // Clears the trigPin
        digitalWrite(m_TrigPin, LOW);
        delayMicroseconds(2);

        // Sets the trigPin on HIGH state for 10 micro seconds
        digitalWrite(m_TrigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(m_TrigPin, LOW);
        // Reads the echoPin, returns the sound wave travel time in microseconds
        // long duration = pulseIn(m_EchoPin, HIGH,300000);// wait 300ms for the pulse to start
        long duration = pulseIn(m_EchoPin, HIGH); // wait 300ms for the pulse to start
        // Calculate the distance
        m_RawDistance = duration * SOUND_SPEED;
        return m_RawDistance / 2.0f;
    }

} // environment_sensing
#endif // DISTANCE_SENSOR