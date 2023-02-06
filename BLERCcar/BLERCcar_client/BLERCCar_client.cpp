#include "BLERCCar_client.hpp"

BLERCCar_client::BLERCCar_client(/* args */)
{

    m_Bluez.init();
    m_Async_thread_arr["BluezKeepAlive"] = new std::thread([&]()
                                                           { 
        while (m_AsyncThreadActive){
        m_Bluez.run_async();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        } });

    m_ClientLogger = spdlog::basic_logger_mt("BLE_RC_Car_Client", "logs/basic-log.txt", true);
    m_ClientLogger->set_level(spdlog::level::info);
    m_ClientLogger->info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
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
    m_ClientLogger->info("Available adapters:");
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
        m_ClientLogger->info(ss.str());
        m_ClientLogger->info("Please select an adapter to scan: ");
        std::cin >> selection;
    }

    if (selection < 0 || selection >= adapters.size())
    {
        m_ClientLogger->error("invalid selection");
        return false;
    }

    auto adapter = adapters[selection];
    m_ClientLogger->info("Scanning {} [{}]", adapter->identifier(), adapter->address());

    SimpleBluez::Adapter::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    adapter->discovery_filter(filter);

    adapter->set_on_device_updated([&](std::shared_ptr<SimpleBluez::Device> device)
                                   {
        if (std::find(m_Peripherals.begin(), m_Peripherals.end(), device) == m_Peripherals.end()) {
             m_ClientLogger->info("Found device: {} [{}]",device->name(),device->address());
            m_Peripherals.push_back(device);
        } });

    adapter->discovery_start();
    millisecond_delay(3000);
    adapter->discovery_stop();

    m_ClientLogger->info("The following devices were found:");
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
    m_ClientLogger->info(ss.str());

    if (server_idx < 0)
    {
        m_ClientLogger->error("Could not find device {}", server.c_str());
        return false;
    }
    else
    {
        m_ClientLogger->info("Find device {}", server.c_str());
    }

    m_Peripheral = m_Peripherals[server_idx];
    m_ClientLogger->info("Connecting to {} [{}]", m_Peripheral->name(), m_Peripheral->address());

    for (int attempt = 0; attempt < m_ConnectionAttempts; attempt++)
    {
        try
        {
            m_Peripheral->connect();
            millisecond_delay(1000);
        }
        catch (SimpleDBus::Exception::SendFailed &e)
        {
            m_ClientLogger->info("Retrying [{}]", m_ConnectionAttempts - attempt);
            millisecond_delay(100);
        }
    }

    if (!m_Peripheral->connected() || !m_Peripheral->services_resolved())
    {
        m_ClientLogger->error("Failed to connect to {} [{}]", m_Peripheral->name(), m_Peripheral->address());
        return false;
    }
    initRssiReadThread();
    mapCharacteristics();
    attachDistanceMeasurementsCallback();
    attachNewDriveModeCallback();
    return true;
}

void BLERCCar_client::attachNewDriveModeCallback()
{
    auto drive_modes_characteristic = m_CharMap[CHARACTERISTIC_UUID_CURRENT_DRIVE].second;
    drive_modes_characteristic->set_on_value_changed([&](SimpleBluez::ByteArray driveMode)
                                                     {
                                                         BLEDrivePacket p{driveMode.c_str()};
                                                         m_DriveMode = p.GetDriveMode();
                                                         // memcpy(&m_DistanceMeasurements.FrontLeft, leftSensorReadings.c_str(), sizeof(double));
                                                         m_ClientLogger->debug("New DriveMode received: {}",mode_to_str( p.GetDriveMode())); });
    drive_modes_characteristic->start_notify();
}

void BLERCCar_client::attachDistanceMeasurementsCallback()
{
    auto front_left_distance_reading_characteristic = m_CharMap[CHARACTERISTIC_UUID_FRONT_LEFT_DISTANCE_CM].second;
    front_left_distance_reading_characteristic->set_on_value_changed([&](SimpleBluez::ByteArray leftSensorReadings)
                                                                     {
                                                                         memcpy(&m_DistanceMeasurements.FrontLeft, leftSensorReadings.c_str(), sizeof(double));
                                                                          m_ClientLogger->debug("leftSensorReadings,{}", m_DistanceMeasurements.FrontLeft); });
    front_left_distance_reading_characteristic->start_notify();

    auto front_right_distance_reading_characteristic = m_CharMap[CHARACTERISTIC_UUID_FRONT_RIGHT_DISTANCE_CM].second;
    front_right_distance_reading_characteristic->set_on_value_changed([&](SimpleBluez::ByteArray rightSensorReadings)
                                                                      {
                                                                         memcpy(&m_DistanceMeasurements.FrontRight, rightSensorReadings.c_str(), sizeof(double));
                                                                          m_ClientLogger->debug("rightSensorReadings,{}", m_DistanceMeasurements.FrontRight); });
    front_right_distance_reading_characteristic->start_notify();
}

void BLERCCar_client::initRssiReadThread()
{
    m_Async_thread_arr["ConnectionRssi"] = new std::thread([&]()
                                                           {
        while (m_AsyncThreadActive)
        {
            auto rssi = m_Peripheral->rssi();
            m_ConnectionRssi = rssi;
            m_ClientLogger->debug("connectionRSSI,{}", rssi);
            millisecond_delay(500);
        } });
}
void BLERCCar_client::disconnect()
{
    m_ClientLogger->debug("Start {}", __PRETTY_FUNCTION__);
    m_Peripheral->disconnect();

    millisecond_delay(1000);

    m_AsyncThreadActive = false;
    for (auto &&async_t : m_Async_thread_arr)
    {
        while (!async_t.second->joinable())
        {
            m_ClientLogger->info("Disconnecting from {}. Waiting for {} to stop working", m_Peripheral->name(), async_t.first);
            millisecond_delay(10);
        }
        async_t.second->join();
        delete async_t.second;
    }
    m_ClientLogger->debug("Finish {}", __PRETTY_FUNCTION__);
}
void BLERCCar_client::Disconnect()
{
    disconnect();
}

void BLERCCar_client::TurnLeft(const char percentage)
{
    m_ClientLogger->debug("Start {}", __PRETTY_FUNCTION__);
    auto steering_characteristic = m_CharMap[CHARACTERISTIC_UUID_STEERING].second;
    BLESteerLeftPacket packet{percentage};
    steering_characteristic->write_command(SimpleBluez::ByteArray(packet.GetPayload()));
    m_ClientLogger->debug("Finish {}", __PRETTY_FUNCTION__);
}

void BLERCCar_client::TurnRight(const char percentage)
{
    m_ClientLogger->debug("Start {}", __PRETTY_FUNCTION__);
    auto steering_characteristic = m_CharMap[CHARACTERISTIC_UUID_STEERING].second;
    BLESteerRightPacket packet{percentage};
    steering_characteristic->write_command(SimpleBluez::ByteArray(packet.GetPayload()));
    m_ClientLogger->debug("Finish {}", __PRETTY_FUNCTION__);
}

void BLERCCar_client::SetDriveMode(DriveMode mode)
{
    m_ClientLogger->debug("Start {}", __PRETTY_FUNCTION__);
    auto characteristic = m_CharMap[CHARACTERISTIC_UUID_SET_DRIVE_MODES].second;
    BLEDrivePacket packet{mode, 0};
    characteristic->write_command(SimpleBluez::ByteArray(packet.GetPayload()));
    // auto got = characteristic->read();
    m_ClientLogger->debug("Finish {}", __PRETTY_FUNCTION__);
}

void BLERCCar_client::SetSpeed(const DriveMode &mode, const char speed)
{
    m_ClientLogger->debug("Start {}", __PRETTY_FUNCTION__);
    auto characteristic = m_CharMap[CHARACTERISTIC_UUID_SET_DRIVE_MODES].second;

    // Do we care about Current DriveMode?
    // auto got = characteristic->read();

    BLEDrivePacket packet{mode, speed};
    characteristic->write_command(SimpleBluez::ByteArray(packet.GetPayload()));

    m_ClientLogger->debug("Finish {}", __PRETTY_FUNCTION__);
}