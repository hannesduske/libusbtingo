#pragma once

#include <iomanip>
#include <iostream>
#include <usbtingo/basic_bus/BasicListener.hpp>
#include <usbtingo/can/Dlc.hpp>


using namespace usbtingo;

class MinimalBasicListener : public usbtingo::bus::BasicListener {
public:
  void on_can_receive(const usbtingo::bus::Message msg) override {
    // Do something with the received message, e.g. print it to the command line
    std::cout << "Got message: "
              << " 0x" << std::hex << msg.id << " [" << msg.data.size() << "] ";

    for (const auto& element : msg.data) {
      std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(element) << " ";
    }
    std::cout << std::endl;
  }
};