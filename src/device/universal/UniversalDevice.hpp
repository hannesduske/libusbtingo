#pragma once

#include "usbtingo/device/Device.hpp"

#include "../DeviceProtocol.hpp"
#include "UniversalHandle.hpp"
#include "UsbLoader.hpp"

#include <vector>
#include <string>
#include <map>

namespace usbtingo{

namespace device{

class UniversalDevice : public Device{
public:
    UniversalDevice(std::uint32_t serial, libusb_device* dev);

    ~UniversalDevice() override;

    static std::unique_ptr<Device> create_device(std::uint32_t serial);

    static std::vector<std::uint32_t> detect_available_devices();

    bool open() override;

    bool close() override;

    bool is_open() const override;

/*
    bool clear_errors() override;

    bool disable_all_filters() override;

    bool add_std_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) override;

    bool add_ext_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) override;
*/

    bool cancel_async_can_request() override;

    std::future<bool> request_can_async() override;

    bool receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

    bool cancel_async_status_request() override;

    std::future<bool> request_status_async() override;

    bool receive_status_async(StatusFrame& status_frame) override;

private:
    static std::vector<std::uint32_t> m_existing_devs;

    UniversalHandle m_device_data;

    libusb_transfer* m_async_status;
    //libusb_transfer* m_async_logic;
    libusb_transfer* m_async_can;
    
    std::promise<bool> m_promise_status;
    std::promise<bool> m_promise_logic;
    std::promise<bool> m_promise_can;

    void handle_can_async_callback(libusb_transfer* transfer);

    void handle_status_async_callback(libusb_transfer* transfer);

    static std::map<std::uint32_t, libusb_device*> detect_usbtingos();

    bool read_usbtingo_serial(std::uint32_t& serial) override;

    bool write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len) override;

    bool read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len) override;

    bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx) override;

    bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data) override;

    bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len) override;

    bool read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len) override;

};

}

}