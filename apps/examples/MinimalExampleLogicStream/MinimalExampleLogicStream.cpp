#include <chrono>
#include <cstdint>
#include <iostream>

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

#include "MinimalLogicListener.hpp"


using namespace usbtingo;
using namespace std::literals::chrono_literals;

// Set the samplerate for the logic data stream
constexpr std::size_t device_index    = 0;
constexpr std::uint32_t samplerate_hz = 1000000;

/**
 * @brief Minimal example of a program that opens a the logic data stream and prints it to the command line.
 */
int main(int /*argc*/, char* /*argv*/[]) {
  // Get all connected USBtingo devices
  std::cout << "Get all connected USBtingo devices" << std::endl;

  auto serial_vec = device::DeviceFactory::detect_available_devices();
  if (serial_vec.size() <= device_index) {
    std::cout << "No USBtingos connected" << std::endl << "Exiting program" << std::endl;
    return 0;
  }

  // Create one USBtingo according to the index
  std::cout << "Create USBtingo device" << std::endl;

  auto serial = serial_vec.at(device_index);
  auto device = device::DeviceFactory::create(serial);

  // Check if the device object is valid
  if (!device) {
    std::cout << "Error creating the device" << std::endl << "Exiting program" << std::endl;
    return 0;
  }

  // Create a Bus object
  std::cout << "Create CAN Bus" << std::endl;
  auto bus = std::make_unique<bus::Bus>(std::move(device));

  // Register an observer that gets notified when new messages arrive
  MinimalLogicListener listener;
  bus->add_listener(reinterpret_cast<usbtingo::bus::LogicListener*>(&listener));

  // Start the logic data stream with the specified sample rate
  bus->start_logic_stream(samplerate_hz);

  std::cin.get();

  // Stop the logic data stream
  bus->stop_logic_stream();

  std::cout << std::endl << "Exiting program" << std::endl;
  return 1;
}