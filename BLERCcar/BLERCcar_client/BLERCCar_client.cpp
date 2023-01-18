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
    spdlog::info("Available adapters:");
    if (adapters.size() == 1)
    {
        selection = 0;
    }
    else
    {
        std::stringstream ss;
        for (int i = 0; i < adapters.size(); i++)
        {
            ss << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]"
               << std::endl;
        }
        spdlog::info(ss.str());
        spdlog::info("Please select an adapter to scan: ");
        std::cin >> selection;
    }

    if (selection < 0 || selection >= adapters.size())
    {
        spdlog::error("invalid selection");
        return false;
    }

    auto adapter = adapters[selection];
    spdlog::info("Scanning {} [{}]", adapter->identifier(), adapter->address());

    SimpleBluez::Adapter::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    adapter->discovery_filter(filter);

    adapter->set_on_device_updated([&](std::shared_ptr<SimpleBluez::Device> device)
                                   {
        if (std::find(m_Peripherals.begin(), m_Peripherals.end(), device) == m_Peripherals.end()) {
             spdlog::info("Found device: {} [{}]",device->name(),device->address());
            m_Peripherals.push_back(device);
        } });

    adapter->discovery_start();
    millisecond_delay(3000);
    adapter->discovery_stop();

    spdlog::info("The following devices were found:");
    int server_idx = -1;
    std::stringstream ss;

    for (int i = 0; i < m_Peripherals.size(); i++)
    {

        ss << "[" << i << "] " << m_Peripherals[i]->name() << " [" << m_Peripherals[i]->address() << "]"
           << std::endl;

        if (m_Peripherals[i]->address() == server)
        {
            server_idx = i;
            break;
        }
    }
    spdlog::info(ss.str());

    if (server_idx < 0)
    {
        spdlog::error("Could not find device {}", server.c_str());
        return false;
    }
    else
    {
        spdlog::info("Find device {}", server.c_str());
    }

    m_Peripheral = m_Peripherals[server_idx];
    spdlog::info("Connecting to {} [{}]", m_Peripheral->name(), m_Peripheral->address());

    for (int attempt = 0; attempt < m_ConnectionAttempts; attempt++)
    {
        try
        {
            m_Peripheral->connect();
            millisecond_delay(1000);
        }
        catch (SimpleDBus::Exception::SendFailed &e)
        {
            spdlog::info("Retrying [{}]", m_ConnectionAttempts - attempt);
            millisecond_delay(100);
        }
    }

    if (!m_Peripheral->connected() || !m_Peripheral->services_resolved())
    {
        spdlog::error("Failed to connect to {} [{}]", m_Peripheral->name(), m_Peripheral->address());
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