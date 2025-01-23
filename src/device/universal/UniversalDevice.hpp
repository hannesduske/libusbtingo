#pragma once

#include "usbtingo/device/Device.hpp"

#include <vector>

namespace usbtingo{

namespace device{

class UniversalDevice : public Device{
public:
    UniversalDevice(std::uint32_t serial, std::string path);

    ~UniversalDevice() override;

    static std::unique_ptr<Device> create_device(std::uint32_t serial);

    static std::vector<std::uint32_t> detect_available_devices();

    bool open() override;

    bool close() override;

    bool is_open() const override;
    
    bool is_alive() const override;

    bool set_mode(Mode mode) override;

    bool set_protocol(Protocol protocol, std::uint8_t flags = 0) override;

    bool set_baudrate(std::uint32_t baudrate) override;

    bool set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data) override;

    bool clear_errors() override;

/*
    bool clear_errors() override;

    bool disable_all_filters() override;

    bool add_std_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) override;

    bool add_ext_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) override;
*/

    bool read_status(StatusFrame& status) override;

    bool send_can(const CanTxFrame& tx_frame) override;
    
    bool send_can(const std::vector<CanTxFrame>& tx_frames) override;

    bool receive_can(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

    bool cancel_async_can_request() override;

    std::future<bool> request_can_async() override;

    bool receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

    bool cancel_async_status_request() override;

    std::future<bool> request_status_async() override;

    bool receive_status_async(StatusFrame& status_frame) override;

};

}

}