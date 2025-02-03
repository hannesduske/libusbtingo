#include "UtilityHelper.hpp"
#include "UtilityListener.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    UtilityListener listener;

    std::cout << "+======================================+" << std::endl;
    std::cout << "  USBtingoCansend utility application   " << std::endl;
    std::cout << "+======================================+" << std::endl;

    std::cout << " This application sends CAN messages" << std::endl
              << " with a connected USBtingo device." << std::endl
              << std::endl;

    bool fd_on = false;
    auto bus = createBus(fd_on);
    if (!bus)
    {
        std::cout << "Could not open device" << std::endl << "Exiting program..." << std::endl;
        return 0;
    }
        
    bus->add_listener(reinterpret_cast<usbtingo::bus::BasicListener*>(&listener));

    std::cout << "+======================================+" << std::endl
              << "Start listening for Can messages..." << std::endl
              << "Message format: <id> [dlc] <data>" << std::endl
              << "(Press ENTER to exit)" << std::endl << std::endl;
    
    std::cin.ignore();
    while (std::cin.get() != '\n') {}
    return 1;
}