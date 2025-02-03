#include "usbtingo/device/DeviceFactory.hpp"

#include <algorithm>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std::literals::chrono_literals;

std::atomic<bool> shutdown(false);

void waitForKeyPress()
{
    std::cin.get();
    shutdown.store(true);
}

/**
 * @brief Minimal example of a command line program that lists the serial numbers of all connected USBtingo devices. At the start all currently connected USBtingo serial numbers are printed. Subsequently, all connection and disconnection events of USBtingo devices are printed.
 */
int main(int argc, char *argv[])
{
    std::cout << "+======================================+" << std::endl;
    std::cout << "   USBtingoDetect utility application   " << std::endl;
    std::cout << "+======================================+" << std::endl;

    std::cout << " This application detects and lists all" << std::endl
              << " connected USBtingo devices." << std::endl
              << std::endl;

    // Detect all connected USBtingo devices.
    auto serial_vec = usbtingo::device::DeviceFactory::detect_available_devices();

    // Print the serial numbers of all connected USBtingo devices
    std::cout << "+======================================+" << std::endl;
    if (serial_vec.size() > 0)
    {
        std::cout << " Currently connected devices:" << std::endl
                  << std::endl;
        std::size_t idx = 0;
        for (const auto &serial : serial_vec)
        {
            std::cout << " #" << idx << " USBtingo " << serial << std::endl;
            idx++;
        }
    }
    else
    {
        std::cout << " There are no USBtingo devices currently" << std::endl
                  << " connected." << std::endl;
    }

    std::cout << std::endl;

    // Loop and check for new devices until ENTER is pressed.
    std::cout << "+======================================+" << std::endl;
    std::cout << " Watch for newly connected devices:" << std::endl
              << " (Press ENTER to exit)" << std::endl
              << std::endl;

    std::thread inputThread(waitForKeyPress);
    while (!shutdown.load())
    {
        const auto current_vec = usbtingo::device::DeviceFactory::detect_available_devices();

        // Check for disconnected devices
        for (const auto &serial : serial_vec)
        {
            if (std::none_of(current_vec.begin(), current_vec.end(),
                             [&serial](std::uint32_t x)
                             { return serial == x; }))
            {
                std::cout << " USBtingo " << serial << " disconnected." << std::endl;
            }
        }

        // Check for newly connected devices
        for (const auto &serial : current_vec)
        {
            if (std::none_of(serial_vec.begin(), serial_vec.end(),
                             [&serial](std::uint32_t x)
                             { return serial == x; }))
            {
                std::cout << " USBtingo " << serial << " connected." << std::endl;
            }
        }

        serial_vec = current_vec;
        std::this_thread::sleep_for(100ms);
    }

    inputThread.join();
    return 0;
}