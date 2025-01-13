#include <catch2/catch_test_macros.hpp>

#include "usbtingo/can/Can.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

#include <iostream>
#include <chrono>
#include <thread>

// Convenience
using usbtingo::device::Mode;
using usbtingo::device::Device;
using usbtingo::device::Protocol;
using usbtingo::device::StatusFrame;
using usbtingo::device::DeviceFactory;

using usbtingo::device::CanTxFrame;
using usbtingo::device::CanRxFrame;
using usbtingo::device::StatusFrame;
using usbtingo::device::TxEventFrame;

using usbtingo::can::Dlc;

TEST_CASE("Integration Test Device, Device configuration", "[device]"){

    auto sn_vec = DeviceFactory::detect_available_devices();
    if (sn_vec.size() == 0) {
        SKIP("At least one USBtingo device must be connected to run this test.");
    }
    const auto sn = sn_vec.front();
    auto dev = DeviceFactory::create(sn);
    REQUIRE(dev);
    REQUIRE(dev->is_alive() == true);
    
    SECTION("Instantiate a real device"){
        CHECK(dev->get_serial() == sn);
    }

    SECTION("Open and close a real device") {
        CHECK(dev->is_open() == true);
        CHECK(dev->close() == true);
        CHECK(dev->is_open() == false);
    }

    SECTION("Set Protocol") {
        const std::vector<Protocol> protocol_vec = { Protocol::CAN_2_0, Protocol::CAN_FD, Protocol::CAN_FD_NON_ISO };
        for (const auto& protocol : protocol_vec) {
            CHECK(dev->set_protocol(protocol) == true);
        }
    }

    SECTION("Set Baudrate") {
        const std::vector<std::uint32_t> baud_vec = { 125000, 25000, 500000, 1000000 };
        const std::vector<std::uint32_t> baud_data_vec = { 1000000, 2000000, 4000000, 8000000 };

        for (const auto& baud : baud_vec) {
            CHECK(dev->set_baudrate(baud) == true);
        }

        for (const auto& baud : baud_vec) {
            for (const auto& baud_data : baud_data_vec) {
                CHECK(dev->set_baudrate(baud, baud_data) == true);
            }
        }
    }

    SECTION("Set Mode"){
        const std::vector<Mode> mode_vec = { Mode::OFF, Mode::ACTIVE, Mode::LISTEN_ONLY };
        for (const auto& mode : mode_vec) {
            CHECK(dev->set_mode(mode) == true);

            StatusFrame status;
            dev->read_status(status);
            CHECK(status.get_operation_mode() == mode);
        }
    }

    SECTION("Read status") {
        // Get status and verify Mode::OFF
        StatusFrame status;
        REQUIRE(dev->read_status(status));
        CHECK(status.get_operation_mode() == Mode::OFF);

        // Set Mode::ACTIVE and verify status report
        dev->set_mode(Mode::ACTIVE);
        dev->read_status(status);
        CHECK(status.get_operation_mode() == Mode::ACTIVE);

        // Set Mode::OFF and verify status report
        dev->set_mode(Mode::LISTEN_ONLY);
        CHECK(dev->read_status(status));
        CHECK(status.get_operation_mode() == Mode::LISTEN_ONLY);

    }
}



TEST_CASE("Integration Test Device, I/O Operation", "[device]") {

    auto sn_vec = DeviceFactory::detect_available_devices();
    if (sn_vec.size() == 0) {
        SKIP("At least one USBtingo device must be connected to run this test.");
    }
    const auto sn = sn_vec.front();
    auto dev = DeviceFactory::create(sn);
    REQUIRE(dev);
    REQUIRE(dev->is_alive() == true);

    SECTION("Send a single CAN frame") {
        
        const std::uint32_t baud = 500000;

        CanTxFrame tx_frame;
        tx_frame.id = 42;
        tx_frame.dlc = static_cast<std::uint8_t>(Dlc::DLC_6_BYTES);
        tx_frame.data.at(0) = 0x48; // H
        tx_frame.data.at(1) = 0x65; // e
        tx_frame.data.at(2) = 0x6c; // l
        tx_frame.data.at(3) = 0x6c; // l
        tx_frame.data.at(4) = 0x6f; // o
        tx_frame.data.at(5) = 0x21; // !
        tx_frame.efc = 1;
        tx_frame.esi = 1;
        tx_frame.txmm = 123;
        
        dev->set_baudrate(baud);
        dev->set_protocol(Protocol::CAN_2_0, 0b00010000);
        dev->set_mode(Mode::ACTIVE);

        std::size_t msg_idx;
        StatusFrame status;
        dev->read_status(status);
        msg_idx = status.nr_std_frames;
        
        std::string response;
        //std::cin.ignore();
        std::cout << "Sending ONE CAN 2.0 message at a baudrate of " << baud << " after pressing ENTER ..." << std::endl;
        while (std::cin.get() != '\n') { }
        CHECK(dev->send_can(tx_frame));

        // Check if tx counter has been increased by one
        dev->read_status(status);
        CHECK(status.nr_std_frames - msg_idx == 1);

        std::cout << "Has ONE message been sent with ID " << tx_frame.id << " and data \"Hello!\" ? (y / n) : " << std::endl;
        std::cin >> response;
        CHECK(response == "y");

        //std::vector<CanRxFrame> rx_frames;
        //std::vector<TxEventFrame> tx_event_frames;
        //WARN("The following CHECK is potentially blocking");
        //CHECK(dev->receive_can(rx_frames, tx_event_frames));
        //CHECK(tx_event_frames.size() == 1);
        //CHECK(tx_event_frames.at(0).txmm == identifier);
    }

    SECTION("Send multiple CAN frames #1") {

        const std::uint32_t baud = 500000;

        CanTxFrame tx_frame1, tx_frame2;
        tx_frame1.id = 42;
        tx_frame1.dlc = static_cast<std::uint8_t>(Dlc::DLC_5_BYTES);
        tx_frame1.data.at(0) = 0x48; // H
        tx_frame1.data.at(1) = 0x65; // e
        tx_frame1.data.at(2) = 0x6c; // l
        tx_frame1.data.at(3) = 0x6c; // l
        tx_frame1.data.at(4) = 0x6f; // o
        tx_frame1.efc = 1;
        tx_frame1.esi = 1;

        tx_frame2.id = 43;
        tx_frame2.dlc = static_cast<std::uint8_t>(Dlc::DLC_6_BYTES);
        tx_frame2.data.at(0) = 0x57; // W
        tx_frame2.data.at(1) = 0x6f; // o
        tx_frame2.data.at(2) = 0x72; // r
        tx_frame2.data.at(3) = 0x6c; // l
        tx_frame2.data.at(4) = 0x64; // d
        tx_frame2.data.at(5) = 0x21; // !
        tx_frame2.efc = 1;
        tx_frame2.esi = 1;

        std::vector<CanTxFrame> tx_frames;
        tx_frames.push_back(tx_frame1);
        tx_frames.push_back(tx_frame2);

        dev->set_baudrate(baud);
        dev->set_protocol(Protocol::CAN_2_0, 0b00010000);
        dev->set_mode(Mode::ACTIVE);
        
        std::size_t msg_idx;
        StatusFrame status;
        dev->read_status(status);
        msg_idx = status.nr_std_frames;

        std::string response;
        std::cin.ignore();
        std::cout << "Sending TWO CAN 2.0 messages at a baudrate of " << baud << " after pressing ENTER..." << std::endl;
        while (std::cin.get() != '\n') {}
        CHECK(dev->send_can(tx_frames));

        // Check if tx counter has been increased by one
        dev->read_status(status);
        CHECK(status.nr_std_frames - msg_idx == 2);

        std::cout << "Have TWO messages been sent with IDs " << tx_frame1.id << " & " << tx_frame2.id << " and data \"Hello\" & \"World!\" ? (y / n) : " << std::endl;
        std::cin >> response;
        CHECK(response == "y");

        //std::vector<CanRxFrame> rx_frames;
        //std::vector<TxEventFrame> tx_event_frames;
        //WARN("The following CHECK is potentially blocking");
        //CHECK(dev->receive_can(rx_frames, tx_event_frames));
        //CHECK(tx_event_frames.size() == 1);
    }

    SECTION("Send multiple CAN frames #2") {

        const std::size_t nr_of_msg = 256;
        const std::uint32_t baud = 500000;
        std::vector<CanTxFrame> tx_frames(nr_of_msg);

        std::size_t i = 0;
        for (auto& tx_frame : tx_frames) {
            tx_frame.id = 42;
            tx_frame.dlc = static_cast<std::uint8_t>(Dlc::DLC_1_BYTES);
            tx_frame.data.at(0) = i;
            tx_frame.efc = 1;
            tx_frame.esi = 1;

            i++;
        }

        dev->set_baudrate(baud);
        dev->set_protocol(Protocol::CAN_2_0, 0b00010000);
        dev->set_mode(Mode::ACTIVE);

        std::size_t msg_idx;
        StatusFrame status;
        dev->read_status(status);
        msg_idx = status.nr_std_frames;

        CHECK(dev->send_can(tx_frames));
        // Check if tx counter has been increased by one (~50ms for 255 msgs @ 500000 baud)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        dev->read_status(status);
        CHECK(status.nr_std_frames - msg_idx == nr_of_msg);
    }
}