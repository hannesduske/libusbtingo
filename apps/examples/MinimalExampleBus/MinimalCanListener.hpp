#pragma once

#include <iomanip>
#include <iostream>
#include <usbtingo/bus/CanListener.hpp>
#include <usbtingo/can/Dlc.hpp>


using namespace usbtingo;

class MinimalCanListener : public usbtingo::bus::CanListener {
public:
  void on_can_receive(const device::CanRxFrame msg) override {
    // Do something with the received message, e.g. print it to the command line
    auto len = can::Dlc::dlc_to_bytes(static_cast<std::uint8_t>(msg.dlc));

    std::cout << "Got message: "
              << " 0x" << std::hex << msg.id << " [" << static_cast<int>(len) << "] ";

    for (std::size_t i = 0; i < len; i++) {
      std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
    }
    std::cout << std::endl;
  }
};