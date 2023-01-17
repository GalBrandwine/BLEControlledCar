#include "BLERCCar_client.hpp"

BLERCCar_client::BLERCCar_client(/* args */)
{
    // Runtime log levels
    spdlog::set_level(spdlog::level::debug); // Set global log level to info

    m_Bluez.init();
    m_async_thread = new std::thread([&]()
                                     { 
        while (m_AsyncThreadActive){
        m_Bluez.run_async();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        } });

    spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
}

BLERCCar_client::~BLERCCar_client()
{
    disconnect();
}

void BLERCCar_client::mapCharacteristics()
{

    for (auto service : m_Peripheral->services())
    {
        for (auto characteristic : service->characteristics())
        {
            m_CharMap[characteristic->uuid()] = std::make_pair(service, characteristic);
        }
    }
}

bool BLERCCar_client::Connect(const std::string &server)
{
    int selection{-1};
    auto adapters = m_Bluez.get_adapters();
    std::cout << "Available adapters:" << std::endl;
    if (adapters.size() == 1)
    {
        selection = 0;
    }
    else
    {
        for (int i = 0; i < adapters.size(); i++)
        {
            std::cout << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]"
                      << std::endl;
        }

        std::cout << "Please select an adapter to scan: ";
        std::cin >> selection;
    }

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

    adapter->set_on_device_updated([&](std::shared_ptr<SimpleBluez::Device> device)
                                   {
        if (std::find(m_Peripherals.begin(), m_Peripherals.end(), device) == m_Peripherals.end()) {
             std::cout << "Found device: " << device->name() << " [" << device->address() << "]" << std::endl;
            m_Peripherals.push_back(device);
        } });

    adapter->discovery_start();
    millisecond_delay(3000);
    adapter->discovery_stop();

    std::cout << "The following devices were found:" << std::endl;
    int server_idx = -1;
    // const std::string my_device{"78:E3:6D:65:45:22"};

    for (int i = 0; i < m_Peripherals.size(); i++)
    {
        if (m_Peripherals[i]->address() == server)
        {
            server_idx = i;
            break;
        }
        std::cout << "[" << i << "] " << m_Peripherals[i]->name() << " [" << m_Peripherals[i]->address() << "]"
                  << std::endl;
    }
    if (server_idx < 0)
    {
        std::cout << "Could not find device" << server.c_str() << "\n";
        return 0;
    }
    else
    {
        std::cout << "BLE CR Car found: " << server.c_str() << "\n";
    }

    m_Peripheral = m_Peripherals[server_idx];
    std::cout << "Connecting to " << m_Peripheral->name() << " [" << m_Peripheral->address() << "]" << std::endl;

    for (int attempt = 0; attempt < 5; attempt++)
    {
        try
        {
            m_Peripheral->connect();
            millisecond_delay(1000);
        }
        catch (SimpleDBus::Exception::SendFailed &e)
        {
            millisecond_delay(100);
        }
    }

    if (!m_Peripheral->connected() || !m_Peripheral->services_resolved())
    {
        std::cout << "Failed to connect to " << m_Peripheral->name() << " [" << m_Peripheral->address() << "]" << std::endl;
        return false;
    }

    mapCharacteristics();
    return true;
}

void BLERCCar_client::disconnect()
{
    m_Peripheral->disconnect();

    millisecond_delay(1000);

    m_AsyncThreadActive = false;
    while (!m_async_thread->joinable())
    {
        spdlog::info("Disconnecting from {}\n", m_Peripheral->name());
        millisecond_delay(10);
    }
    m_async_thread->join();
    delete m_async_thread;
}
void BLERCCar_client::Disconnect()
{
    disconnect();
}

void BLERCCar_client::TurnLeft(int percentage)
{
    spdlog::debug("Start {}\n", __PRETTY_FUNCTION__);
    auto steering_characteristic = m_CharMap[CHARACTERISTIC_UUID_STEERING].second;
    auto ba = SimpleBluez::ByteArray("L" + percentage);
    steering_characteristic->write_command(ba);
    spdlog::debug("Finish {}\n", __PRETTY_FUNCTION__);
}