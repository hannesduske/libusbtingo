#include "UniversalDevice.hpp"

#include "../DeviceProtocol.hpp"

#include <libusb-1.0/libusb.h>
#include <string>

namespace usbtingo{

namespace device{

// init static class member
std::map<std::uint32_t, libusb_device*> UniversalDevice::m_usbtingos = { };

UniversalDevice::UniversalDevice(std::uint32_t serial, std::string path) :
    Device(serial)
{
    open();
}

UniversalDevice::~UniversalDevice()
{
    close();
}

std::unique_ptr<Device> UniversalDevice::create_device(std::uint32_t serial)
{
    return std::unique_ptr<Device>();
}

std::vector<std::uint32_t> UniversalDevice::detect_available_devices()
{   
    std::vector<std::uint32_t> serial_vec;

   if(!UniversalDevice::detect_usbtingos()) return serial_vec;

    for (const auto& usbtingo : m_usbtingos) {
        serial_vec.push_back(usbtingo.first);
    }
    
    return serial_vec;
}

bool UniversalDevice::detect_usbtingos()
{

    int r = 0;
    libusb_device** devs;
    r = libusb_get_device_list(NULL, &devs);

    // Failed to fetch USB Devices
    if (r < 0) return false;

    libusb_device* dev;
    int i = 0;
    
    while((dev = devs[i++]) != NULL){

       struct libusb_device_descriptor desc;
		r = libusb_get_device_descriptor(dev, &desc);
		if (r >= 0){
            if (desc.idVendor == USBTINGO_VID && desc.idProduct == USBTINGO_PID){
                
                //printf("Found device\n");
                libusb_device_handle* handle = NULL;
                r = libusb_open(dev, &handle);

                if(r >= 0){

                    unsigned char buffer[8] = {0};
                    r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buffer, sizeof(buffer));
                    std::uint32_t serial = static_cast<uint32_t>(std::stoi(reinterpret_cast<const char*>(buffer)));
                        if(r > 0) m_usbtingos.emplace(serial, dev);
                }

                libusb_close(handle);
            }
        }
    }

    libusb_free_device_list(devs, 1);
    return true;
}

bool UniversalDevice::open()
{
    return false;
}

bool UniversalDevice::close()
{
    return true;
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