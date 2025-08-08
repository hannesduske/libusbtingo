#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/basic_bus/Message.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

#include "MinimalCanListener.hpp"

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
 * @brief Minimal example of a program that opens a Bus to send and receive CAN messages.
 */
int main(int argc, char *argv[])
{
    // Get all connected USBtingo devices
    std::cout << "Get all connected USBtingo devices" << std::endl;

    auto serial_vec = device::DeviceFactory::detect_available_devices();
    if(serial_vec.size() <= device_index)
    {
        std::cout << "No USBtingos connected" << std::endl << "Exiting program" << std::endl;
        return 0;
    }

    // Create one USBtingo according to the index
    std::cout << "Create USBtingo device" << std::endl;

    auto serial = serial_vec.at(device_index);
    auto device = device::DeviceFactory::create(serial);

    // Check if the device object is valid
    if(!device)
    {
        std::cout << "Error creating the device" << std::endl << "Exiting program" << std::endl;
        return 0;
    }

    // Configure the device
    device->set_mode(device::Mode::OFF);                // Device has to be in Mode::OFF for the configuration
    device->set_baudrate(baudrate, data_baudrate);      // Set baudrate
    device->set_protocol(protocol, 0b00010000);         // Set protocol and disable automatic retransmission of failed messages
    device->set_mode(device::Mode::ACTIVE);             // Activate device before passing it to the Bus

    // Create a Bus object
    std::cout << "Create CAN Bus" << std::endl;
    auto bus = std::make_unique<bus::Bus>(std::move(device));

    // Register an observer that gets notified when new messages arrive
    MinimalCanListener listener;
    bus->add_listener(reinterpret_cast<usbtingo::bus::CanListener *>(&listener));
    
    // Variant 1: Manually create a tx message.
    device::CanTxFrame tx_msg1;
    tx_msg1.id = testid;
    tx_msg1.dlc = can::Dlc::bytes_to_dlc(testdata.size());
    tx_msg1.fdf = (protocol == device::Protocol::CAN_2_0) ? false : true;
    std::copy(testdata.begin(), testdata.end(), tx_msg1.data.data());

    // Variant 2: Create a tx message with the Message class.
    bus::Message tx_msg2(testid, std::vector<std::uint8_t>(testdata.begin(), testdata.end()));

    // Send a message every second until ENTER is pressed
    std::cout << "Start sending CAN messages until ENTER is pressed..." << std::endl << std::endl;
    int i = 0;

    std::thread inputThread(waitForKeyPress);
    while (!shutdown.load())
    {
        // Send message with variant 1
        std::cout << "Sending CAN message #" << std::dec << i << ": ";
        printMessage(bus::Message(tx_msg1));
        i++;
        
        bus->send(tx_msg1);   
        std::this_thread::sleep_for(1000ms);
        
        // Send message with variant 2
        std::cout << "Sending CAN message #" << std::dec << i << ": ";
        printMessage(bus::Message(tx_msg2));
        i++;
        
        bus->send(tx_msg2.to_CanTxFrame());   
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