#ifndef DISTANCE_SENSOR_DATA
#define DISTANCE_SENSOR_DATA

// define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

namespace environment_sensing
{
    struct DistanceMeasurements
    {
        double FrontLeft{0}, FrontRight{0};
    };

} // environment_sensing
#endif // DISTANCE_SENSOR