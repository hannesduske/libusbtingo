#include "usbtingo/basic_bus/BasicBus.hpp"
#include "usbtingo/basic_bus/Message.hpp"

#include "MinimalBasicListener.hpp"

#include <cstdint>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

using namespace usbtingo;
using namespace std::literals::chrono_literals;

// Setup of the CAN parameters
constexpr std::size_t                 device_index    = 0;
constexpr device::Protocol            protocol        = device::Protocol::CAN_FD;
constexpr std::uint32_t               baudrate        = 1000000;
constexpr std::uint32_t               data_baudrate   = 1000000;

// Data for a CAN test message
constexpr std::uint32_t               testid          = 42;
constexpr std::array<std::uint8_t, 5> testdata        = { 0, 1, 2, 3, 4 };

// Small helper
std::atomic<bool> shutdown(false);
void waitForKeyPress();
void printMessage(const usbtingo::bus::Message msg);

/**
 * @brief Minimal example of a program that opens a BasicBus to send and receive CAN messages.
 */
int main(int argc, char *argv[])
{
    // Create one USBtingo according to the index
    std::cout << "Create Can Bus" << std::endl;
    auto bus = bus::BasicBus::create(device_index, baudrate, data_baudrate, protocol);

    // Check if the device object is valid
    if(!bus)
    {
        std::cout << "Error creating the Can Bus" << std::endl << "Exiting program" << std::endl;
        return 0;
    }

    // Register an observer that gets notified when new messages arrive
    MinimalBasicListener listener;
    bus->add_listener(reinterpret_cast<usbtingo::bus::BasicListener *>(&listener));
    
    // Create a tx message with the Message class.
    bus::Message tx_msg(testid, std::vector<std::uint8_t>(testdata.begin(), testdata.end()));

    // Send a message every second until ENTER is pressed
    std::cout << "Start sending CAN messages until ENTER is pressed..." << std::endl << std::endl;
    int i = 0;

    std::thread inputThread(waitForKeyPress);
    while (!shutdown.load())
    {
        // Send message
        std::cout << "Sending CAN message " << "#" << std::dec << i << ": ";
        printMessage(bus::Message(tx_msg));
        i++;
        
        bus->send(tx_msg);   
        std::this_thread::sleep_for(1000ms);
    }

    std::cout << std::endl << "Exiting program" << std::endl;
    return 1;
}

/**
 * @brief Wait for keyboard input
 */
void waitForKeyPress()
{
    std::cin.get();
    shutdown.store(true);
}

/**
 * @brief Print a usbtingo::device::Message to the command line
 */
void printMessage(const usbtingo::bus::Message msg)
{
    std::cout << " 0x" << std::hex << msg.id;
    std::cout << " [" << std::to_string(msg.data.size()) << "] ";

    for (std::size_t i = 0; i < usbtingo::can::Dlc::dlc_to_bytes(static_cast<std::uint8_t>(msg.data.size())); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
    }
    std::cout << std::endl;
}