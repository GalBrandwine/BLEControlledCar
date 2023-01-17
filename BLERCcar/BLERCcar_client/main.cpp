#include "BLERCCar_client.hpp"
int main(int argc, char *argv[])
{
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};
    if (!car_client.Connect(server))
    {
        spdlog::error("Couldn't connect to car server on: {}", server);
        return 1;
    }
    car_client.TurnLeft(100);
    // Store all services and characteristics in a vector.
    // struct BLEService_characteristics_pair
    // {
    //     /* data */
    // };

    std::cout << "The following services and characteristics were found:" << std::endl;
    // for (int i = 0; i < char_map.size(); i++)
    // {
    //     std::cout << "[" << i << "] " << char_list[i].first->uuid() << " " << char_list[i].second->uuid() << std::endl;
    // }

    // std::cout << "Please select a characteristic to read: ";
    // std::cin >> selection;

    // if (selection >= 0 && selection < char_list.size())
    // {
    //     SimpleBluez::ByteArray value = char_list[selection].second->read();
    //     std::cout << "Characteristic contents were: ";
    //     print_byte_array(value);
    // }

    // auto drive_modes_characteristic = char_map[CHARACTERISTIC_UUID_DRIVE_MODES].second;
    // drive_modes_characteristic->write_command(SimpleBluez::ByteArray("led on"));
    // auto characteristic = char_list[2].second;
    // characteristic->write_command(SimpleBluez::ByteArray("led on"));

    // peripheral->disconnect();

    // Sleep for an additional second before returning.
    // If there are any unexpected events, this example will help debug them.
    millisecond_delay(1000);

    // async_thread_active = false;
    // while (!async_thread->joinable())
    // {
    //     millisecond_delay(10);
    // }
    // async_thread->join();
    // delete async_thread;

    return 0;
}
