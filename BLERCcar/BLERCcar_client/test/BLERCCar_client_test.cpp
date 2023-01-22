#include "gtest/gtest.h"
#include "BLERCCar_client.hpp"
#include <chrono>
//----------------------------------------------------------------
//
// These are "wet" tests, they require real BLE central.
// No mocking here. So test result might Not be deterministic
//
//----------------------------------------------------------------

TEST(TestBLERCcar, ConnectionTestOnly)
{
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};
    ASSERT_TRUE(car_client.Connect(server));
}

TEST(TestBLERCcar, MinimalFlow)
{
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(server));

    car_client.TurnLeft(100);
}

TEST(TestBLERCcar, TurnLeftIncrementally)
{
    using namespace std::chrono_literals;
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(server));

    int MAX{100};
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.TurnLeft(i);
        std::this_thread::sleep_for(500ms);
    }
}

TEST(TestBLERCcar, TurnRightIncrementally)
{
    using namespace std::chrono_literals;
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(server));

    int MAX{100};
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.TurnRight(i);
        std::this_thread::sleep_for(500ms);
    }
}

TEST(TestBLERCcar, SetDriveModeForward)
{
    using namespace std::chrono_literals;
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(server));

    car_client.SetDriveMode(DriveMode::Forward);
}

TEST(TestBLERCcar, SetSpeedBackwardInclemently)
{
    using namespace std::chrono_literals;
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(server));

    int MAX{100};
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.SetSpeed(DriveMode::Backward, i);
        std::this_thread::sleep_for(500ms);
    }
}

// TEST(TestBLERCcar, GetCurrentDrivingMode)
// {
//     using namespace std::chrono_literals;
//     auto server{"78:E3:6D:65:45:22"};
//     BLERCCar_client car_client{};

//     ASSERT_TRUE(car_client.Connect(server));

//     int MAX{100};
//     for (int i = 0; i < MAX; i += 5)
//     {
//         car_client.TurnRight(i);
//         std::this_thread::sleep_for(500ms);
//     }
// }

// TEST(TestBLERCcar, CheckIfDriveModeIsBeingUpdatedWhenInternalEventOccured)
// {
//     using namespace std::chrono_literals;
//     auto server{"78:E3:6D:65:45:22"};
//     BLERCCar_client car_client{};

//     ASSERT_TRUE(car_client.Connect(server));

//     int MAX{100};
//     for (int i = 0; i < MAX; i += 5)
//     {
//         car_client.TurnRight(i);
//         std::this_thread::sleep_for(500ms);
//     }
// }