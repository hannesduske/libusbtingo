#pragma once

#include "usbtingo/device/Device.hpp"

#include "WinHandle.hpp"
#include "../DeviceProtocol.hpp"

#include <Windows.h>
#include <winusb.h>

#include <string>
#include <vector>
#include <array>
#include <map>

namespace usbtingo{

namespace device{

class WinDevice : public Device{
public:
    WinDevice(std::uint32_t serial, std::string path);
    
    ~WinDevice() override;

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

    bool send_can(const CanTxFrame& tx_frame) override;
    
    bool send_can(const std::vector<CanTxFrame>& tx_frames) override;

    bool receive_can(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

    bool cancel_async_can_request() override;

    bool cancel_async_logic_request() override;

    bool cancel_async_status_request() override;

    std::future<bool> request_can_async() override;

    std::future<bool> request_logic_async() override;

    std::future<bool> request_status_async() override;
    
    bool receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

    bool receive_logic_async(LogicFrame& logic_frame) override;
    
    bool receive_status_async(StatusFrame& status_frame) override;

private:
    WinHandle m_device_data;

    static std::map<unsigned long, std::string> detect_usbtingos();

    OVERLAPPED m_async_status;
    OVERLAPPED m_async_logic;
    OVERLAPPED m_async_can;

    static HRESULT detect_usb_devices(std::vector<std::string>& devices, std::uint16_t vid, std::uint16_t pid);

    bool read_usbtingo_serial(std::uint32_t& serial) override;

    static HRESULT read_usb_descriptor(const WinHandle& device_data, const std::uint8_t index, const std::uint16_t languageID, std::string& value);

    static HRESULT open_usb_device(WinHandle& device_data);

    static HRESULT close_usb_device(WinHandle& device_data);

    bool write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len) override;

    bool read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len) override;

    static bool request_bulk_async(const WinHandle& device_data, std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len, OVERLAPPED& async);

    static bool read_bulk_async(const WinHandle& device_data, std::size_t& len, OVERLAPPED& async);

    bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx) override;

    bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data) override;

    bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len) override;

    bool read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len) override;

};

}

}