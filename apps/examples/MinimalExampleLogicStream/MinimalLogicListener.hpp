#pragma once

#include <usbtingo/bus/LogicListener.hpp>

#include <iostream>
#include <bitset>

using namespace usbtingo;

class MinimalLogicListener : public usbtingo::bus::LogicListener{
public:
    void on_logic_receive(const device::LogicFrame msg) override
    {
        // Do something with the received message, e.g. print it to the command line
        const auto& data = msg.data;
        for (std::size_t i = 0; i < data.size(); ++i) {
            std::bitset<8> bits(data[i]);

            // Print the bits
            std::cout << bits;

            // Format the output
            if (i != data.size() - 1) {
                std::cout << ' ';
                if ((i + 1) % 16 == 0)
                std::cout << '\n';
            }
        }
        std::cout << std::endl << std::endl;
    }
};