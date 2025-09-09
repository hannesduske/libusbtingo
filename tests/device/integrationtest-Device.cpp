#include <catch2/catch_test_macros.hpp>

#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"
#include "usbtingo/device/McanProtocol.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <iomanip>

// Convenience
using namespace usbtingo::device;

using usbtingo::device::Mode;
using usbtingo::device::Device;
using usbtingo::device::Protocol;
using usbtingo::device::DeviceFactory;

using usbtingo::device::CanTxFrame;
using usbtingo::device::CanRxFrame;
using usbtingo::device::StatusFrame;
using usbtingo::device::TxEventFrame;

using usbtingo::can::Dlc;

TEST_CASE("Integration Test Device, Device configuration", "[device]"){

    auto sn_vec = DeviceFactory::detect_available_devices();
    if (sn_vec.size() == 0) {
        FAIL("At least one USBtingo device must be connected to run this test.");
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

    SECTION("Start and stop logic stream") {
        REQUIRE(dev->start_logic_stream());
        REQUIRE(dev->start_logic_stream() == false);
        REQUIRE(dev->stop_logic_stream());
        REQUIRE(dev->stop_logic_stream() == false);
    }
}

TEST_CASE("Integration Test Device, MCAN configuration", "[device]"){

    auto sn_vec = DeviceFactory::detect_available_devices();
    if (sn_vec.size() == 0) {
        FAIL("At least one USBtingo device must be connected to run this test.");
    }
    const auto sn = sn_vec.front();
    auto dev = DeviceFactory::create(sn);
    REQUIRE(dev);
    REQUIRE(dev->is_alive() == true);

    SECTION("Read endianness test register"){
        // Checks if data is read correctly and if the endianess of the received data is correct
        // Expected value of register 0x04 is 0x87654321 (see datasheet)

        constexpr std::uint32_t testvalue = 0x87654321;

        std::vector<std::uint32_t> data;
        REQUIRE(dev->read_mcan_registers(mcan::MCAN_REG_ENDN, data, 1));
        REQUIRE(data.size() == 1);
        REQUIRE(data.at(0)  == 0x87654321);
    }

    SECTION("Automatically set flag register"){
        // Check if the flags set by the USBTingo API correctly translate to the corresponding MCAN register
        // The values set by the USBTingo API should match the values in the MCAN register

        std::uint8_t flags = 0;
        flags |= 1 << 0; // Bit rate switching for transmissions if CAN Fd is activated
        flags |= 1 << 1; // Transmit pause
        flags |= 1 << 2; // Edge filtering during bus integration
        flags |= 1 << 3; // Protocol exception handling disable
        flags |= 1 << 4; // Disable automatic retransmission

        REQUIRE(dev->set_protocol(Protocol::CAN_2_0, flags));

        std::vector<std::uint32_t> data;
        REQUIRE(dev->read_mcan_registers(mcan::MCAN_REG_CCCR, data, 1));
        REQUIRE(data.size() == 1);

        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_BRSE) & 0x01) == 1);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 1);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_EFBI) & 0x01) == 1);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 1);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 1);

        flags = 0x00;
        REQUIRE(dev->set_protocol(Protocol::CAN_2_0, flags));

        data.clear();
        REQUIRE(dev->read_mcan_registers(mcan::MCAN_REG_CCCR, data, 1));
        REQUIRE(data.size() == 1);

        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_BRSE) & 0x01) == 0);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 0);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_EFBI) & 0x01) == 0);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 0);
        CHECK(((data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 0);    
    }

    SECTION("Manually set flag register"){
        // Check if manually setting and resetting the control register works

        std::vector<std::uint32_t> r_data;
        REQUIRE(dev->read_mcan_registers(mcan::MCAN_REG_CCCR, r_data, 1));

        std::uint32_t flags_u32 = r_data.at(0);
        flags_u32 |= 1 << mcan::MCAN_REG_CCCR_BRSE; // Bit rate switching for transmissions if CAN Fd is activated
        flags_u32 |= 1 << mcan::MCAN_REG_CCCR_TXP;  // Transmit pause
        flags_u32 |= 1 << mcan::MCAN_REG_CCCR_EFBI; // Edge filtering during bus integration
        flags_u32 |= 1 << mcan::MCAN_REG_CCCR_PXHD; // Protocol exception handling disable
        flags_u32 |= 1 << mcan::MCAN_REG_CCCR_DAR;  // Disable automatic retransmission

        r_data.clear();
        std::vector<std::uint32_t> w_data = {flags_u32};
        REQUIRE(dev->write_mcan_registers(mcan::MCAN_REG_CCCR, w_data));
        REQUIRE(dev->read_mcan_registers(mcan::MCAN_REG_CCCR, r_data, 1));
        REQUIRE(r_data.size() == 1);

        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_BRSE) & 0x01) == 1);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 1);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_EFBI) & 0x01) == 1);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 1);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 1);

        flags_u32 = r_data.at(0);
        flags_u32 &= ~(1 << mcan::MCAN_REG_CCCR_BRSE); // Bit rate switching for transmissions if CAN Fd is activated
        flags_u32 &= ~(1 << mcan::MCAN_REG_CCCR_TXP);  // Transmit pause
        flags_u32 &= ~(1 << mcan::MCAN_REG_CCCR_EFBI); // Edge filtering during bus integration
        flags_u32 &= ~(1 << mcan::MCAN_REG_CCCR_PXHD); // Protocol exception handling disable
        flags_u32 &= ~(1 << mcan::MCAN_REG_CCCR_DAR);  // Disable automatic retransmission

        r_data.clear();
        w_data.at(0) = flags_u32;
        REQUIRE(dev->write_mcan_registers(mcan::MCAN_REG_CCCR, w_data));
        REQUIRE(dev->read_mcan_registers(mcan::MCAN_REG_CCCR, r_data, 1));
        REQUIRE(r_data.size() == 1);

        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_BRSE) & 0x01) == 0);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 0);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_EFBI) & 0x01) == 0);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 0);
        CHECK(((r_data.at(0) >> mcan::MCAN_REG_CCCR_TXP)  & 0x01) == 0);
    }
    
}

TEST_CASE("Integration Test Device, I/O Operation", "[device]") {

    auto sn_vec = DeviceFactory::detect_available_devices();
    if (sn_vec.size() == 0) {
        FAIL("At least one USBtingo device must be connected to run this test.");
    }
    const auto sn = sn_vec.front();
    auto dev = DeviceFactory::create(sn);
    REQUIRE(dev);
    REQUIRE(dev->is_alive() == true);

    SECTION("Send a single CAN 2.0 frame") {
        
        const std::uint32_t baud = 1000000;

        CanTxFrame tx_frame;
        tx_frame.id = 42;
        tx_frame.dlc = static_cast<std::uint8_t>(Dlc::DLC_6_BYTES);
        tx_frame.data.at(0) = 0x48; // H
        tx_frame.data.at(1) = 0x65; // e
        tx_frame.data.at(2) = 0x6c; // l
        tx_frame.data.at(3) = 0x6c; // l
        tx_frame.data.at(4) = 0x6f; // o
        tx_frame.data.at(5) = 0x21; // !
        tx_frame.fdf = 1;
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

        dev->clear_errors();
        dev->read_status(status);
        
#ifdef ENABLE_INTERACTIVE_TESTS
        std::string response;
        std::cin.ignore();
        std::cout << "Sending ONE CAN 2.0 message at a baudrate of " << baud << " after pressing ENTER ..." << std::endl;
        while (std::cin.get() != '\n') { }
#endif
        CHECK(dev->send_can(tx_frame));

        // Check if tx counter has been increased by one
        dev->read_status(status);
        CHECK(status.nr_std_frames - msg_idx == 1);

#ifdef ENABLE_INTERACTIVE_TESTS
        std::cout << "Has ONE message been sent with ID " << tx_frame.id << " and data \"Hello!\" ? (y / n) : " << std::endl;
        std::cin >> response;
        CHECK(response == "y");
#else
        SKIP("Skipping interactive checks of this test. This test has been turned off with Cmake Variable \"ENABLE_INTERACTIVE_TESTS\"");
#endif
    }

    SECTION("Send multiple CAN 2.0 frames #1") {

        const std::uint32_t baud = 1000000;

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

#ifdef ENABLE_INTERACTIVE_TESTS
        std::string response;
        std::cin.ignore();
        std::cout << "Sending TWO CAN 2.0 messages at a baudrate of " << baud << " after pressing ENTER..." << std::endl;
        while (std::cin.get() != '\n') {}
#endif
        CHECK(dev->send_can(tx_frames));

        // Check if tx counter has been increased by one
        dev->read_status(status);
        CHECK(status.nr_std_frames - msg_idx == 2);

#ifdef ENABLE_INTERACTIVE_TESTS
        std::cout << "Have TWO messages been sent with IDs " << tx_frame1.id << " & " << tx_frame2.id << " and data \"Hello\" & \"World!\" ? (y / n) : " << std::endl;
        std::cin >> response;
        CHECK(response == "y");
#else
        SKIP("Skipping interactive checks of this test. This test has been turned off with Cmake Variable \"ENABLE_INTERACTIVE_TESTS\"");
#endif
    }

    SECTION("Send multiple CAN 2.0 frames #2") {

        const std::size_t nr_of_msg = 256;
        const std::uint32_t baud = 1000000;
        std::vector<CanTxFrame> tx_frames(nr_of_msg);

        std::uint8_t i = 0;
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

    SECTION("Send a single CAN FD frame") {

        const std::uint32_t baud = 1000000;

        CanTxFrame tx_frame;
        tx_frame.id = 42;
        tx_frame.dlc = static_cast<std::uint8_t>(Dlc::DLC_12_BYTES);
        tx_frame.data.at(0) = 0x48; // H
        tx_frame.data.at(1) = 0x65; // e
        tx_frame.data.at(2) = 0x6c; // l
        tx_frame.data.at(3) = 0x6c; // l
        tx_frame.data.at(4) = 0x6f; // o
        tx_frame.data.at(5) = 0x20; //
        tx_frame.data.at(6) = 0x57; // W
        tx_frame.data.at(7) = 0x72; // o
        tx_frame.data.at(8) = 0x6f; // r
        tx_frame.data.at(9) = 0x6c; // l
        tx_frame.data.at(10) = 0x64; // d
        tx_frame.data.at(11) = 0x21; // !
        tx_frame.fdf = 1;
        tx_frame.efc = 1;
        tx_frame.esi = 1;
        tx_frame.txmm = 123;

        dev->set_baudrate(baud);
        dev->set_protocol(Protocol::CAN_FD, 0b00010000);
        dev->set_mode(Mode::ACTIVE);

        std::size_t msg_idx;
        StatusFrame status;
        dev->read_status(status);
        msg_idx = status.nr_std_frames;

        CHECK(dev->send_can(tx_frame));

        // Check if tx counter has been increased by one
        dev->read_status(status);
        CHECK(status.nr_std_frames - msg_idx == 1);
    }

    SECTION("Send a single CAN FD frame and check tx_event") {

        const std::uint32_t baud = 1000000;

#ifdef ENABLE_TESTS_WITH_OTHER_DEVICES
        WARN("This test requires another device that acknowledges the transmisison of the CAN message. I.e. needs to be configured to CAN FD protocol and a baudrate of " << baud << " .");
#else
        SKIP("This test has been turned off with Cmake Variable \"ENABLE_TEST_WITH_OTHER_DEVICES\"");
#endif

        CanTxFrame tx_frame;
        tx_frame.id = 42;
        tx_frame.dlc = static_cast<std::uint8_t>(Dlc::DLC_12_BYTES);
        tx_frame.data.at(0) = 0x48; // H
        tx_frame.data.at(1) = 0x65; // e
        tx_frame.data.at(2) = 0x6c; // l
        tx_frame.data.at(3) = 0x6c; // l
        tx_frame.data.at(4) = 0x6f; // o
        tx_frame.data.at(5) = 0x20; //
        tx_frame.data.at(6) = 0x57; // W
        tx_frame.data.at(7) = 0x72; // o
        tx_frame.data.at(8) = 0x6f; // r
        tx_frame.data.at(9) = 0x6c; // l
        tx_frame.data.at(10) = 0x64; // d
        tx_frame.data.at(11) = 0x21; // !
        tx_frame.fdf = 1;
        tx_frame.efc = 1;
        tx_frame.esi = 1;
        tx_frame.txmm = 123;

        dev->set_baudrate(baud);
        dev->set_protocol(Protocol::CAN_FD, 0b00010000);
        dev->set_mode(Mode::ACTIVE);

        dev->send_can(tx_frame);

        std::vector<CanRxFrame> rx_frames;
        std::vector<TxEventFrame> tx_event_frames;

        std::future_status result;

        do {
            auto future = dev->request_can_async();
            REQUIRE(future.valid());

            result = future.wait_for(std::chrono::milliseconds(100));

            if ((result == std::future_status::ready) && future.get()) {
                dev->receive_can_async(rx_frames, tx_event_frames);
            }
            else {
                dev->cancel_async_can_request();
            }

        } while (result == std::future_status::ready);

        REQUIRE(tx_event_frames.size() > 0);
        CHECK(tx_event_frames.back().esi == 1); // Error state indicator passive
        CHECK(tx_event_frames.back().id == tx_frame.id); // can id must match can id of sent message
        CHECK(tx_event_frames.back().txmm == tx_frame.txmm); // unique id must match undique id of sent message
    }

    SECTION("Receive a CAN FD frame drom another device") {

        const std::uint32_t baud = 1000000;

#ifdef ENABLE_TESTS_WITH_OTHER_DEVICES
        WARN("This test requires another device to send a CAN FD message at a baudrate of " << baud << " .");
#else
        SKIP("This test has been turned off with Cmake Variable \"ENABLE_TEST_WITH_OTHER_DEVICES\"");
#endif

        dev->set_baudrate(baud);
        dev->set_protocol(Protocol::CAN_FD, 0b00010000);
        dev->set_mode(Mode::ACTIVE);

        std::future_status result;
        std::vector<CanRxFrame> rx_frames;
        std::vector<TxEventFrame> tx_event_frames;
        
        auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(30));
        WARN("Waiting for " << std::chrono::duration_cast<std::chrono::seconds>(timeout).count() << " seconds to receive a CAN FD message.");

        do {
            auto future = dev->request_can_async();
            if(!future.valid()) FAIL("Failed to request CAN message transfer from usb device.");
    
            result = future.wait_for(timeout);

            if ((result == std::future_status::ready) && future.get()) {
                dev->receive_can_async(rx_frames, tx_event_frames);
            }
            else {
                dev->cancel_async_can_request();
            }

            // wait a little while longer in case more messages arrive
            if (result == std::future_status::ready) {
                timeout = std::chrono::milliseconds(200);
            }

        } while (result != std::future_status::timeout);

        REQUIRE(rx_frames.size() > 0);

        std::cout << "Got " << rx_frames.size()  << " CAN FD message(s) : " << std::endl;

        for (const auto& msg : rx_frames) {
            std::cout << "    Std ID:  0x" << std::hex << msg.id << std::endl;
            std::cout << "    DLC: " << std::to_string(Dlc::dlc_to_bytes(msg.dlc)) << " Bytes" << std::endl;
            std::cout << "    Data: ";

            for (size_t i = 0; i < Dlc::dlc_to_bytes(msg.dlc); i++) {
                std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.data.at(i)) << " ";
            }
            std::cout << std::endl << std::endl;
        }

#ifdef ENABLE_INTERACTIVE_TESTS
        std::string response;
        std::cin.ignore();
        std::cout << std::dec << "Is the received data correct? (y / n) : " << std::endl;
        std::cin >> response;
        while (std::cin.get() != '\n') {}
        CHECK(response == "y");
#else
        SKIP("Skipping interactive checks of this test. This test has been turned off with Cmake Variable \"ENABLE_INTERACTIVE_TESTS\"");
#endif
    }
}