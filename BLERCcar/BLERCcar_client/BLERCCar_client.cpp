#include <simplebluez/Bluez.h>
#include <simplebluez/Exceptions.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "common.hpp"
SimpleBluez::Bluez bluez;

std::atomic<bool> async_thread_active{true};
void async_thread_function()
{
    while (async_thread_active)
    {
        bluez.run_async();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void millisecond_delay(int ms)
{
    for (int i = 0; i < ms; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void print_byte_array(SimpleBluez::ByteArray &bytes)
{
    for (auto byte : bytes)
    {
        std::cout << std::hex << std::setfill('0') << (uint32_t)((uint8_t)byte) << " ";
        break;
    }
    std::cout << std::endl;
}

std::vector<std::shared_ptr<SimpleBluez::Device>> peripherals;

int main(int argc, char *argv[])
{
    int selection = -1;

    bluez.init();
    std::thread *async_thread = new std::thread(async_thread_function);

    auto adapters = bluez.get_adapters();
    std::cout << "Available adapters:" << std::endl;
    for (int i = 0; i < adapters.size(); i++)
    {
        std::cout << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]"
                  << std::endl;
    }

    std::cout << "Please select an adapter to scan: ";
    std::cin >> selection;
    if (selection < 0 || selection >= adapters.size())
    {
        std::cout << "Invalid selection" << std::endl;
        return 1;
    }

    auto adapter = adapters[selection];
    std::cout << "Scanning " << adapter->identifier() << " [" << adapter->address() << "]" << std::endl;

    SimpleBluez::Adapter::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    adapter->discovery_filter(filter);

    adapter->set_on_device_updated([](std::shared_ptr<SimpleBluez::Device> device)
                                   {
        if (std::find(peripherals.begin(), peripherals.end(), device) == peripherals.end()) {
            std::cout << "Found device: " << device->name() << " [" << device->address() << "]" << std::endl;
            peripherals.push_back(device);
        } });

    adapter->discovery_start();
    millisecond_delay(3000);
    adapter->discovery_stop();

    std::cout << "The following devices were found:" << std::endl;
    int my_devices = -1;
    const std::string my_device{"78:E3:6D:65:45:22"};

    for (int i = 0; i < peripherals.size(); i++)
    {
        if (peripherals[i]->address() == my_device)
        {
            my_devices = i;
            break;
        }
        std::cout << "[" << i << "] " << peripherals[i]->name() << " [" << peripherals[i]->address() << "]"
                  << std::endl;
    }
    if (my_devices < 0)
    {
        std::cout << "Could not find device" << my_device.c_str() << "\n";
        return 0;
    }
    else
    {
        std::cout << "BLE CR Car found: " << my_device.c_str() << "\n";
    }

    auto peripheral = peripherals[my_devices];
    std::cout << "Connecting to " << peripheral->name() << " [" << peripheral->address() << "]" << std::endl;

    for (int attempt = 0; attempt < 5; attempt++)
    {
        try
        {
            peripheral->connect();
            millisecond_delay(1000);
        }
        catch (SimpleDBus::Exception::SendFailed &e)
        {
            millisecond_delay(100);
        }
    }

    if (!peripheral->connected() || !peripheral->services_resolved())
    {
        std::cout << "Failed to connect to " << peripheral->name() << " [" << peripheral->address() << "]" << std::endl;
        return 1;
    }

    // Store all services and characteristics in a vector.
    // struct BLEService_characteristics_pair
    // {
    //     /* data */
    // };

    std::unordered_map<std::string, std::pair<std::shared_ptr<SimpleBluez::Service>, std::shared_ptr<SimpleBluez::Characteristic>>>
        char_map;
    for (auto service : peripheral->services())
    {
        for (auto characteristic : service->characteristics())
        {
            char_map[characteristic->uuid()] = std::make_pair(service, characteristic);
        }
    }

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

    auto drive_modes_characteristic = char_map[CHARACTERISTIC_UUID_DRIVE_MODES].second;
    drive_modes_characteristic->write_command(SimpleBluez::ByteArray("led on"));
    // auto characteristic = char_list[2].second;
    // characteristic->write_command(SimpleBluez::ByteArray("led on"));

    peripheral->disconnect();

    // Sleep for an additional second before returning.
    // If there are any unexpected events, this example will help debug them.
    millisecond_delay(1000);

    async_thread_active = false;
    while (!async_thread->joinable())
    {
        millisecond_delay(10);
    }
    async_thread->join();
    delete async_thread;

    return 0;
}
