#pragma once
#include <simplebluez/Bluez.h>
#include <simplebluez/Exceptions.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "spdlog/spdlog.h"
#include "common.hpp"
#include "Icontroller.hpp"

static void millisecond_delay(int ms)
{
    for (int i = 0; i < ms; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

static void print_byte_array(SimpleBluez::ByteArray &bytes)
{
    for (auto byte : bytes)
    {
        std::cout << std::hex << std::setfill('0') << (uint32_t)((uint8_t)byte) << " ";
        break;
    }
    std::cout << std::endl;
}

class BLERCCar_client : public Icontroller
{
private:
    SimpleBluez::Bluez m_Bluez;
    const int m_ConnectionAttempts{5};
    std::atomic<bool> m_AsyncThreadActive{true};
    std::atomic<int16_t> m_ConnectionRssi{-1};
    std::unordered_map<std::string, std::thread *> m_Async_thread_arr;
    std::vector<std::shared_ptr<SimpleBluez::Device>> m_Peripherals; // BLE related
    std::unordered_map<std::string, std::pair<std::shared_ptr<SimpleBluez::Service>, std::shared_ptr<SimpleBluez::Characteristic>>>
        m_CharMap;
    /**
     * @brief The BLE peripheral object represent the Car BLE service
     *
     */
    std::shared_ptr<SimpleBluez::Device> m_Peripheral;
    void mapCharacteristics();
    void disconnect();

    /**
     * @brief Read the peripheral RSSI
     *
     */
    void initRssiReadThread();

public:
    BLERCCar_client(/* args */);
    ~BLERCCar_client();
    /**
     * @brief Connect to BLERC car server
     *
     * @param server mac address of the server
     * @return true connected successfully
     * @return false
     */
    bool Connect(const std::string &server);
    void Disconnect();

    void TurnLeft(const uint8_t percentage) override;
    void TurnRight(const uint8_t percentage) override{};
    void SetSpeed(const uint8_t speed) override{};
    void SetDriveMode(DriveMode mode) override{};
    const DriveMode CurrentDriveMode() override { return DriveMode::Stop; };
    const std::string CurrentDriveModeStr() override { return mode_to_str((DriveMode)99); };
};
