#include "UtilityHelper.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "+======================================+" << std::endl;
    std::cout << "  USBtingoCansend utility application   " << std::endl;
    std::cout << "+======================================+" << std::endl;

    std::cout << " This application sends CAN messages" << std::endl
              << " with a connected USBtingo device." << std::endl
              << std::endl;

    bool fd_on = false;
    auto bus = createBus(fd_on);
    if (!bus)
        return 0;

    std::cout << "+======================================+" << std::endl;
    while (true)
    {    
        std::cout << " Enter a CAN message"
                  << " Format : <id>#<data>"
                  << std::endl;
        std::cout << " > ";

        std::string msg_string;
        usbtingo::bus::Message msg;

        std::cin >> msg_string;
        
        if(parseCanFrame(msg_string, msg, fd_on))
        {
            if (!bus->send(msg))
            {
                std::cout << " Error while sending message";
            }
        }else{
            std::cout << " Error while parsing message";
        }
        std::cout << std::endl;
    }

    return (bus != nullptr) ? 1 : 0;
}