#include "UniversalDevice.hpp"

namespace usbtingo{

namespace device{

UniversalDevice::UniversalDevice(std::uint32_t serial, std::string path)
    : Device(serial)
{

}

UniversalDevice::~UniversalDevice()
{

}

std::unique_ptr<Device> UniversalDevice::create_device(std::uint32_t serial)
{
    return std::unique_ptr<Device>();
}

std::vector<std::uint32_t> UniversalDevice::detect_available_devices()
{
    return std::vector<std::uint32_t>();
}

bool UniversalDevice::open()
{
    return false;
}

bool UniversalDevice::close()
{
    return false;
}

bool UniversalDevice::is_open() const
{
    return false;
}

bool UniversalDevice::is_alive() const
{
    return false;
}

bool UniversalDevice::set_mode(Mode mode)
{
    return false;
}

bool UniversalDevice::set_protocol(Protocol protocol, std::uint8_t flags)
{
    return false;
}

bool UniversalDevice::set_baudrate(std::uint32_t baudrate)
{
    return false;
}

bool UniversalDevice::set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data)
{
    return false;
}

bool UniversalDevice::clear_errors()
{
    return false;
}

bool UniversalDevice::read_status(StatusFrame& status)
{
    return false;
}

bool UniversalDevice::send_can(const CanTxFrame& tx_frame)
{
    return false;
}

bool UniversalDevice::send_can(const std::vector<CanTxFrame>& tx_frames)
{
    return false;
}

bool UniversalDevice::receive_can(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames)
{
    return false;
}

bool UniversalDevice::cancel_async_can_request()
{
    return false;
}

std::future<bool> UniversalDevice::request_can_async()
{
    return std::future<bool>();
}

bool UniversalDevice::receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames)
{
    return false;
}

bool UniversalDevice::cancel_async_status_request()
{
    return false;
}

std::future<bool> UniversalDevice::request_status_async()
{
    return std::future<bool>();
}

bool UniversalDevice::receive_status_async(StatusFrame& status_frame)
{
    return false;
}

}

}