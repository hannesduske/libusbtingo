#include <iostream>

#include "UtilityHelper.hpp"
#include "UtilityListener.hpp"


/**
 * @brief Minimal example of a command line program that prints out all received CAN messages. After the configuration, a listener is registered as an observer of the CAN Bus instance that gets notified asynchronously when new messages
 * arrive.
 * @warning Only one of the utility application can access a USBtingo device at a time. It is currently not possible to run the USBtingoCansend and USBtingoCandump example side by side.
 */
int main(int /*argc*/, char* /*argv*/[]) {
  UtilityListener listener;

  std::cout << "+======================================+" << std::endl;
  std::cout << "  USBtingoCansend utility application   " << std::endl;
  std::cout << "+======================================+" << std::endl;

  std::cout << " This application lists received CAN" << std::endl << " messages from a USBtingo device." << std::endl << std::endl;

  bool fd_on = false;
  auto bus   = createBus(fd_on);
  if (!bus) {
    std::cout << "Could not open device" << std::endl << "Exiting program..." << std::endl;
    return 0;
  }

  bus->add_listener(reinterpret_cast<usbtingo::bus::BasicListener*>(&listener));

  std::cout << "+======================================+" << std::endl << "Start listening for CAN messages..." << std::endl << "Message format: <id> [dlc] <data>" << std::endl << "(Press ENTER to exit)" << std::endl << std::endl;

  std::cin.ignore();
  while (std::cin.get() != '\n') {
  }
  return 1;
}