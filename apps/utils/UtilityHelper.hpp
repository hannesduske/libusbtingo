#pragma once

#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/basic_bus/BasicBus.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>


void printMessage(const usbtingo::bus::Message msg, bool verbose)
{
    if(verbose){
        std::cout << " Std ID:  0x" << std::hex << msg.id << std::endl;
        std::cout << " DLC: " << std::to_string(msg.data.size()) << " Bytes" << std::endl;
        std::cout << " Data: ";

        for (std::size_t i = 0; i < usbtingo::can::Dlc::dlc_to_bytes(static_cast<std::uint8_t>(msg.data.size())); i++) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
        }
        std::cout << std::endl << std::endl;
    }
    else{
        std::cout << " 0x" << std::hex << msg.id;
        std::cout << " [" << std::to_string(msg.data.size()) << "] ";

        for (std::size_t i = 0; i < usbtingo::can::Dlc::dlc_to_bytes(static_cast<std::uint8_t>(msg.data.size())); i++) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
        }
        std::cout << std::endl;
    }
}

void printDevices(const std::vector<std::uint32_t> &serial_vec)
{
    // Print the serial numbers of all connected USBtingo devices
    std::cout << "+======================================+" << std::endl;
    if (serial_vec.size() > 0)
    {
        std::cout << " Currently connected devices:" << std::endl
                  << std::endl;
        std::size_t idx = 0;
        for (const auto &serial : serial_vec)
        {
            std::cout << " #" << idx << " USBtingo "<< serial << " (0x" << std::hex << serial << std::dec << ")" << std::endl;
            idx++;
        }
    }
    else
    {
        std::cout << "No USBtingos connected." << std::endl
                  << "Exit program." << std::endl;
    }
    std::cout << std::endl;
}

std::unique_ptr<usbtingo::bus::BasicBus> createBus(bool& fd_on)
{
    // Detect all connected USBtingo devices.
    auto serial_vec = usbtingo::device::DeviceFactory::detect_available_devices();
    printDevices(serial_vec);

    int idx = 0;
    bool valid_input = true;
    std::size_t device_count = serial_vec.size();

    if (device_count == 0)
    {
        return nullptr;
    }
    else if (device_count == 1)
    {
        std::cout << " Only one USBtingos connected" << std::endl
                  << " > Selecting this one" << std::endl;
    }
    else
    {
        do
        {
            if (!valid_input)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                valid_input = true;
            }
            std::cout << " Select a device:" << std::endl;
            std::cout << " > #";
            if (!(std::cin >> idx))
                valid_input = false;
            std::cout << std::endl;
        } while (!valid_input || idx >= device_count || idx < 0);
    }

    int input = 0;
    do
    {
        if (!valid_input)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            valid_input = true;
        }
        std::cout << " Select a protocol:" << std::endl;
        std::cout << " [0] CAN 2.0" << std::endl;
        std::cout << " [1] CAN FD" << std::endl;
        std::cout << " [2] CAN FD Non-ISO" << std::endl;
        std::cout << " > ";
        if (!(std::cin >> input))
            valid_input = false;
        std::cout << std::endl;
    } while (!valid_input || input > 2 || input < 0);
    usbtingo::device::Protocol protocol = static_cast<usbtingo::device::Protocol>(input);
    fd_on = (protocol == usbtingo::device::Protocol::CAN_2_0) ? false : true;

    int baudrate = 0;
    do
    {
        if (!valid_input)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            valid_input = true;
        }
        std::cout << " Enter a baudrate:" << std::endl;
        std::cout << " > ";
        if (!(std::cin >> baudrate))
            valid_input = false;
        std::cout << std::endl;
    } while (!valid_input);

    int data_baudrate = baudrate;
    if (fd_on)
    {
        do
        {
            if (!valid_input)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                valid_input = true;
            }
            std::cout << " Enter a data baudrate:" << std::endl;
            std::cout << " > ";
            if (!(std::cin >> data_baudrate))
                valid_input = false;
            std::cout << std::endl;
        } while (!valid_input);
    }

    auto dev = usbtingo::device::DeviceFactory::create(serial_vec.at(idx));
    if (!dev)
        return nullptr;
    if (!dev->is_alive())
        return nullptr;

    dev->set_mode(usbtingo::device::Mode::OFF);
    dev->set_baudrate(data_baudrate, data_baudrate);
    dev->set_protocol(protocol);
    dev->set_mode(usbtingo::device::Mode::ACTIVE);

    return std::move(std::make_unique<usbtingo::bus::BasicBus>(std::move(dev)));
}

bool parseCanFrame(const std::string &input, usbtingo::bus::Message &frame, bool fd_on)
{
    size_t pos = input.find('#');
    if (pos == std::string::npos || pos == 0 || pos > 8)
    {
        std::cerr << "Invalid Can frame format." << std::endl;
        return false;
    }

    std::stringstream id_stream(input.substr(0, pos));
    id_stream >> std::hex >> frame.id;

    std::string data_string = input.substr(pos + 1);
    std::size_t data_length = data_string.length() / 2;
    if (data_length > 8 && !fd_on)
    {
        std::cerr << "Data length exceeds 8 bytes." << std::endl;
        return false;
    }
    else if(data_length > 64)
    {
        std::cerr << "Data length exceeds 64 bytes." << std::endl;
        return false;
    }

    frame.data.resize(data_length);
    for (size_t i = 0; i < data_length; ++i)
    {
        std::stringstream byte_stream;
        byte_stream << std::hex << data_string.substr(i * 2, 2);
        int byte_value;
        byte_stream >> byte_value;
        frame.data.at(i) = static_cast<uint8_t>(byte_value);
    }

    return true;
}