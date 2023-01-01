#ifndef ICONTROLLER
#define ICONTROLLER

#include "/home/gal/dev/BLEControlledCar/BLERCcar/common/common.hpp"
class Icontroller
{
public:
    virtual void TurnLeft(int percentage) = 0;
    virtual void TurnRight(int percentage) = 0;
    virtual void SetSpeed(int speed) = 0;
    virtual void SetDriveMode(DriveMode mode) = 0;
    virtual const DriveMode CurrentDriveMode() = 0;
    virtual const std::string CurrentDriveModeStr() = 0;
    virtual ~Icontroller(){};
};

#endif // ICONTROLLER