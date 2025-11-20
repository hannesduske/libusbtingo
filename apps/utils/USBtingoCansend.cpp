#include <iostream>

#include "UtilityHelper.hpp"


/**
 * @brief Minimal example of a command line program that sends CAN messages. After the configuration, the program sends all entered messages on the CAN Bus.
 * @warning Only one of the utility application can access a USBtingo device at a time. It is currently not possible to run the USBtingoCansend and USBtingoCandump example side by side.
 */
int main(int argc, char* argv[]) {
  std::cout << "+======================================+" << std::endl;
  std::cout << "  USBtingoCansend utility application   " << std::endl;
  std::cout << "+======================================+" << std::endl;

  std::cout << " This application sends CAN messages" << std::endl << " with a connected USBtingo device." << std::endl << std::endl;

  bool fd_on = false;
  auto bus   = createBus(fd_on);
  if (!bus) {
    std::cout << "Could not open device" << std::endl << "Exiting program..." << std::endl;
    return 0;
  }

  std::cout << "+======================================+" << std::endl;
  std::cout << " Enter a CAN message" << std::endl << " Format: <id>#<data>" << std::endl << " (Enter \"q\" to exit)" << std::endl << std::endl;
  bool shutdown = false;
  while (!shutdown) {
    std::cout << " > ";

    std::string msg_string;
    usbtingo::bus::Message msg;

    std::cin >> msg_string;

    if (msg_string == "q") {
      shutdown = true;
    } else if (parseCanFrame(msg_string, msg, fd_on)) {
      if (!bus->send(msg)) {
        std::cout << " Error while sending message";
      } else {
        std::cout << " Successfully sent message";
      }
    } else {
      std::cout << " Error while parsing message";
    }
    std::cout << std::endl;
  }

  return 1;
}