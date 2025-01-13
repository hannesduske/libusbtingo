#pragma once

#include "usbtingo/device/Device.hpp"

#include "DeviceData.hpp"
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
    
    ~WinDevice();

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

/*
    bool clear_errors() override;

    bool disable_all_filters() override;

    bool add_std_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) override;

    bool add_ext_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) override;
*/

    bool read_status(StatusFrame& status) override;

    void receive_status(StatusFrame& status) override;

    bool send_can(const CanTxFrame& tx_frame) override;
    
    bool send_can(const std::vector<CanTxFrame>& tx_frames) override;

    bool receive_can(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

private:
    DeviceData m_device_data;

    static std::map<unsigned long, std::string> m_usbtingos;

    static bool detect_usbtingos();

    static HRESULT detect_usb_devices(std::vector<std::string>& devices, std::uint16_t vid, std::uint16_t pid);

    static HRESULT read_usbtingo_info(const DeviceData& device_data, DeviceInfo& device_info);

    static HRESULT read_usbtingo_serial(const DeviceData& device_data, std::uint32_t& serial);

    static HRESULT read_usb_descriptor(const DeviceData& device_data, const std::uint8_t index, const std::uint16_t languageID, std::string& value);

    static HRESULT open_usb_device(DeviceData& device_data);

    static HRESULT close_usb_device(DeviceData& device_data);

    static bool write_bulk(const DeviceData& device_data, std::uint8_t endpoint, const BulkBuffer& buffer, std::size_t len) ;

    static bool read_bulk(const DeviceData& device_data, std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len) ;

    static bool write_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx);

    static bool write_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, const std::vector<std::uint8_t>& data);

    static bool write_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, const std::uint8_t* data, std::uint16_t len);

    static bool read_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len);

};

}

}