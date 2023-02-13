#include "BLERCCar_client.hpp"
#include "conio.h"

std::atomic_bool keep_running = true;
void print_thread(std::shared_ptr<BLERCCar_client> car_client, const std::string server)
{
    while (keep_running)
    {
        if (car_client->Connected())
        {
            auto measurements = car_client->GetDistanceMeasurements();
            auto rssi = car_client->ConnectionRSSI();
            spdlog::info("DriveMode={},RSSI={},FrontLeft={}[cm],FrontRight={}[cm]", car_client->CurrentDriveModeStr(), rssi, measurements.FrontLeft, measurements.FrontRight);
        }
        else
        {
            spdlog::info("Car is disconnected, Reconnecting");
            if (!car_client->Connect(server))
            {
                spdlog::error("Couldn't connect to car server on: {}", server);
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    spdlog::info("Thread is dead");
}

int main(int argc, char *argv[])
{
    auto server{"E0:E2:E6:0C:4A:8A"};
    // auto server{"78:E3:6D:65:45:22"};
    std::shared_ptr<BLERCCar_client> car_client = std::make_shared<BLERCCar_client>(true);
    if (!car_client->Connect(server))
    {
        spdlog::error("Couldn't connect to car server on: {}", server);
        return 1;
    }

    std::thread t1(print_thread, car_client, server);

    while (keep_running)
    {

        if (_kbhit())
        {
            auto key_code = _getche();
            switch (key_code)
            {
            case 'a':
                car_client->TurnLeft(100);
                break;
            case 'd':
                car_client->TurnRight(100);
                break;
            case 'w':
                car_client->TurnLeft(0);
                car_client->SetSpeed(DriveMode::Forward, 80);
                break;
            case 's':
                car_client->SetSpeed(DriveMode::Backward, 80);
                break;
            case 32: // Space
                car_client->SetDriveMode(DriveMode::Stop);
                break;
            case 'q':
                keep_running = false;
            default:
                break;
            }
            spdlog::info(key_code);
        }

        // do stuff depending on key_code
        else
            continue;
        // while (1)
        // {
        //     char input;
        //     cin >> input;
        //     switch (input)
        //     {
        //     case 'a':
        //         car_client.TurnLeft(100);
        //         break;

        //     default:
        //         break;
        //     }
        // }
        // return 0;
    }

    t1.join();
    while (t1.joinable())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}