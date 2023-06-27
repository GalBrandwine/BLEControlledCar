#include "gtest/gtest.h"
#include "BLERCCar_client.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
//----------------------------------------------------------------
//
// These are "wet" tests, they require real BLE central.
// No mocking here. So test result might Not be deterministic
//
//----------------------------------------------------------------
const std::string SERVER_MAC{"E0:E2:E6:0C:4A:8A"};
using namespace std::chrono_literals;
TEST(TestBLERCcar, SampleSensorsRawData)
{
    // Setup
    BLERCCar_client car_client{true};
    ASSERT_TRUE(car_client.Connect(SERVER_MAC));
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "logs/%d-%m-%Y %H-%M-%S_SampleSensorsRawData.csv");
    auto str = oss.str();

    auto ClientLogger = spdlog::basic_logger_mt("SampleSensorsRawData", oss.str(), true);
    int MAX{10000};

    // Write Header
    ClientLogger->info("FrontLeft,FrontRight");

    // Run
    environment_sensing::DistanceMeasurements measurement;
    // Sampling is 10hrz (i.e. 1 new sample every 100 ms) Expecting 2 repeating samples every print
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.GetDistanceMeasurements(measurement);
        ClientLogger->info("{},{}", measurement.FrontLeft, measurement.FrontRight);
        std::this_thread::sleep_for(50ms);
    }

    // Test
}
TEST(TestBLERCcar, ConnectionTestOnly)
{
    BLERCCar_client car_client{true};
    ASSERT_TRUE(car_client.Connect(SERVER_MAC));
    std::this_thread::sleep_for(1000ms);
}

TEST(TestBLERCcar, MinimalFlow)
{
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(SERVER_MAC));

    car_client.TurnLeft(100);
}

TEST(TestBLERCcar, TurnLeftIncrementally)
{
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(SERVER_MAC));

    int MAX{100};
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.TurnLeft(i);
        std::this_thread::sleep_for(500ms);
    }
}

TEST(TestBLERCcar, TurnRightIncrementally)
{
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(SERVER_MAC));

    int MAX{100};
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.TurnRight(i);
        std::this_thread::sleep_for(500ms);
    }
}

TEST(TestBLERCcar, SetDriveModeForward)
{
    BLERCCar_client car_client{};

    ASSERT_TRUE(car_client.Connect(SERVER_MAC));
    auto expected_drive_mode = DriveMode::Forward;
    car_client.SetDriveMode(expected_drive_mode);

    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(car_client.CurrentDriveMode(), expected_drive_mode);
    std::this_thread::sleep_for(500ms);
}

TEST(TestBLERCcar, SetSpeedBackwardInclemently)
{
    BLERCCar_client car_client{true};

    ASSERT_TRUE(car_client.Connect(SERVER_MAC));

    auto expected_drive_mode = DriveMode::Backward;
    int MAX{100};
    for (int i = 0; i < MAX; i += 5)
    {
        car_client.SetSpeed(expected_drive_mode, i);
        std::this_thread::sleep_for(500ms);
    }
    ASSERT_EQ(car_client.CurrentDriveMode(), expected_drive_mode);
    std::this_thread::sleep_for(500ms);
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