#ifndef ICONTROLLER
#define ICONTROLLER

#include "common.hpp"
class Icontroller
{
public:
    virtual void TurnLeft(const char percentage) = 0;
    virtual void TurnRight(const char percentage) = 0;
    virtual void SetSpeed(const char speed) = 0;
    virtual void SetDriveMode(DriveMode mode) = 0;
    virtual const DriveMode CurrentDriveMode() = 0;
    virtual const std::string CurrentDriveModeStr() = 0;
    virtual ~Icontroller(){};
};

#endif // ICONTROLLER