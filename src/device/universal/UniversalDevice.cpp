#include "UniversalDevice.hpp"

#include <libusb-1.0/libusb.h>
#include <string>

namespace usbtingo{

namespace device{
    
// init static class member
std::vector<std::uint32_t> UniversalDevice::m_existing_devs = { };

UniversalDevice::UniversalDevice(std::uint32_t serial, libusb_device* dev) :
    Device(serial)
{
    m_async_can = libusb_alloc_transfer(0);
    m_async_status = libusb_alloc_transfer(0);

    m_device_data.Device = dev;
    if(open()){
        UniversalDevice::read_usbtingo_info();

        m_async_can->dev_handle   = m_device_data.Handle;
        m_async_can->endpoint     = USBTINGO_EP3_CANMSG_IN;
        m_async_can->type         = LIBUSB_TRANSFER_TYPE_BULK;
        m_async_can->timeout      = 0;
        m_async_can->buffer       = static_cast<unsigned char*>(m_buffer_can.data());
        m_async_can->length       = USB_BULK_BUFFER_SIZE;
        m_async_can->user_data    = static_cast<void*>(this);
        m_async_can->callback     = [](libusb_transfer* transfer)
        {
            auto* instance = static_cast<UniversalDevice*>(transfer->user_data);
            if(instance) instance->handle_can_async_callback(transfer);
        };
        
        m_async_status->dev_handle   = m_device_data.Handle;
        m_async_status->endpoint     = USBTINGO_EP1_STATUS_IN;
        m_async_status->type         = LIBUSB_TRANSFER_TYPE_INTERRUPT;
        m_async_status->timeout      = 0;
        m_async_status->buffer       = static_cast<unsigned char*>(m_buffer_status.data());
        m_async_status->length       = USB_BULK_BUFFER_SIZE_STATUS;
        m_async_status->user_data    = static_cast<void*>(this);
        m_async_status->callback     = [](libusb_transfer* transfer)
        {
            auto* instance = static_cast<UniversalDevice*>(transfer->user_data);
            if(instance) instance->handle_status_async_callback(transfer);
        };
    }
}

UniversalDevice::~UniversalDevice()
{
    close();
    libusb_free_transfer(m_async_can);
    libusb_free_transfer(m_async_status);
    const auto it = std::find(m_existing_devs.begin(), m_existing_devs.end(), m_serial);
    m_existing_devs.erase(it);
}

std::unique_ptr<Device> UniversalDevice::create_device(std::uint32_t serial)
{
    // only one instance per unique device
    if(std::any_of(m_existing_devs.begin(), m_existing_devs.end(), [serial](std::uint32_t sn){return sn == serial;})) return nullptr;

    const auto dev_map = UniversalDevice::detect_usbtingos();
    const auto it = dev_map.find(serial);

    if (it != dev_map.end()) {
        auto device = std::make_unique<UniversalDevice>(serial, it->second);
        if (device->is_alive()) {
            m_existing_devs.push_back(serial);
            return std::move(device);
        }
        else {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
}

std::vector<std::uint32_t> UniversalDevice::detect_available_devices()
{   
    std::vector<std::uint32_t> serial_vec;
    const auto dev_map = UniversalDevice::detect_usbtingos();

    for (const auto& it : dev_map) {
        serial_vec.push_back(it.first);
    }
    
    return serial_vec;
}

std::map<std::uint32_t, libusb_device*> UniversalDevice::detect_usbtingos()
{

    std::map<std::uint32_t, libusb_device*> dev_map;

    int r = 0;
    libusb_device** devs;
    r = libusb_get_device_list(NULL, &devs);

    // Failed to fetch USB Devices
    if (r < 0) return dev_map;
    
    int i = 0;
    libusb_device* dev;
    while((dev = devs[i++]) != NULL){

       struct libusb_device_descriptor desc;
		r = libusb_get_device_descriptor(dev, &desc);
		if (r >= 0){
            if (desc.idVendor == USBTINGO_VID && desc.idProduct == USBTINGO_PID){
                
                //printf("Found device\n");
                libusb_device_handle* handle = NULL;
                r = libusb_open(dev, &handle);

                if(r >= 0){

                    unsigned char buffer[16] = {0};
                    r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buffer, sizeof(buffer));
                    std::uint32_t serial = static_cast<uint32_t>(std::stoi(reinterpret_cast<const char*>(buffer)));
                        if(r > 0) dev_map.emplace(serial, dev);
                }

                libusb_close(handle);
            }
        }
    }

    libusb_free_device_list(devs, 1);
    return dev_map;
}

bool UniversalDevice::open()
{
    if (!m_device_data.HandlesOpen) {
        m_device_data.Handle = NULL;
        if(libusb_open(m_device_data.Device, &m_device_data.Handle) >= 0){
            m_device_data.HandlesOpen = true;
            return true;
        }else{
            return false;
        }
    }
    else {
        return false;
    }
}

bool UniversalDevice::close()
{
    if (m_device_data.HandlesOpen) {
        set_mode(Mode::OFF);
        libusb_close(m_device_data.Handle);
        m_device_data.HandlesOpen = false;
        return true;
    }
    else {
        return false;
    }
}

bool UniversalDevice::is_open() const
{
    return m_device_data.HandlesOpen;
}

bool UniversalDevice::cancel_async_can_request()
{
    bool success = m_shutdown_can.load() == AsyncIoState::REQUEST_ACTIVE;
    success &= libusb_cancel_transfer(m_async_can) == 0;

    m_shutdown_can.store(AsyncIoState::SHUTDOWN);
    
    return success;
}

bool UniversalDevice::cancel_async_status_request()
{
    bool success = m_shutdown_status.load() == AsyncIoState::REQUEST_ACTIVE;
    success &= libusb_cancel_transfer(m_async_status) == 0;

    m_shutdown_status.store(AsyncIoState::SHUTDOWN);
    
    return success;
}

std::future<bool> UniversalDevice::request_can_async()
{
    if ((!m_device_data.HandlesOpen) || (m_shutdown_can.load() == AsyncIoState::REQUEST_ACTIVE)) return std::future<bool>();

    if(libusb_submit_transfer(m_async_can) != 0) return std::future<bool>();

    m_promise_can = std::promise<bool>();
    return m_promise_can.get_future();
}

std::future<bool> UniversalDevice::request_status_async() //ToDo: Generalize async request to use with can, logic and status
{
    if ((!m_device_data.HandlesOpen) || (m_shutdown_status.load() == AsyncIoState::REQUEST_ACTIVE)) return std::future<bool>();

    if(libusb_submit_transfer(m_async_status) != 0) return std::future<bool>();

    m_promise_status = std::promise<bool>();
    return m_promise_status.get_future();
}

bool UniversalDevice::receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames)
{
    if (m_shutdown_can.load() != AsyncIoState::DATA_AVAILABLE) return false;

    bool success = process_can_buffer(reinterpret_cast<std::uint8_t*>(&m_buffer_can), m_async_can->actual_length, rx_frames, tx_event_frames);
    m_shutdown_can.store(AsyncIoState::IDLE);

    return success;
}

bool UniversalDevice::receive_status_async(StatusFrame& status_frame)
{
    if (m_shutdown_status.load() != AsyncIoState::DATA_AVAILABLE) return false;

    bool success = StatusFrame::deserialize_status(reinterpret_cast<std::uint8_t*>(&m_buffer_status), status_frame);
    m_shutdown_status.store(AsyncIoState::IDLE);

    return success;
}

void UniversalDevice::handle_can_async_callback(libusb_transfer* transfer)
{
    if(transfer->status == LIBUSB_TRANSFER_COMPLETED){
        m_shutdown_can.store(AsyncIoState::DATA_AVAILABLE);
        m_promise_can.set_value(true);
    }else{
        m_shutdown_can.store(AsyncIoState::SHUTDOWN);
        m_promise_can.set_value(false);
    }
}

void UniversalDevice::handle_status_async_callback(libusb_transfer* transfer)
{
    if(transfer->status == LIBUSB_TRANSFER_COMPLETED){
        m_shutdown_status.store(AsyncIoState::DATA_AVAILABLE);
        m_promise_status.set_value(true);
    }else{
        m_shutdown_status.store(AsyncIoState::SHUTDOWN);
        m_promise_status.set_value(false);
    }
}

bool UniversalDevice::read_usbtingo_serial(std::uint32_t& serial)
{
    if (!m_device_data.HandlesOpen) return false;

    struct libusb_device_descriptor desc;
    if(libusb_get_device_descriptor(m_device_data.Device, &desc) < 0) return false;
    
    unsigned char buffer[16] = {0};
    if(libusb_get_string_descriptor_ascii(m_device_data.Handle, desc.iManufacturer, buffer, sizeof(buffer)) < 0) return false;
    std::string manufacturer = reinterpret_cast<const char*>(buffer);
    
    std::fill(buffer, buffer + sizeof(buffer), 0);
    if(libusb_get_string_descriptor_ascii(m_device_data.Handle, desc.iProduct, buffer, sizeof(buffer)) < 0) return false;
    std::string product = reinterpret_cast<const char*>(buffer);

    if (manufacturer != USBTINGO_MANUFACTURER || product != USBTINGO_PRODUCT) return false;

    if(libusb_get_string_descriptor_ascii(m_device_data.Handle, desc.iSerialNumber, buffer, sizeof(buffer)) < 0) return false;

    serial = static_cast<uint32_t>(std::stoi(reinterpret_cast<const char*>(buffer)));

    return true;
}

bool UniversalDevice::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx)
{
    if(!m_device_data.HandlesOpen) return false;

    uint8_t request_type = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
    unsigned char* data = nullptr;
    uint16_t length = 0;
    unsigned int timeout = 0;

    return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx, data, length, timeout) >= 0;
}

bool UniversalDevice::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data)
{
    if(!m_device_data.HandlesOpen) return false;

    uint8_t request_type = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
    uint16_t length = static_cast<std::uint8_t>(data.size());;
    unsigned int timeout = 0;

    return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx, static_cast<unsigned char*>(data.data()), length, timeout) >= 0;
}

bool UniversalDevice::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len)
{
    if(!m_device_data.HandlesOpen) return false;

    uint8_t request_type = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
    unsigned int timeout = 0;

    return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx, reinterpret_cast<unsigned char*>(data), len, timeout) >= 0;
}

bool UniversalDevice::read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len)
{
    if(!m_device_data.HandlesOpen) return false;

    data.clear();
    data.resize(len);

    uint8_t request_type = USB_REQUEST_DEVICE2HOST | USB_REQUEST_TYPE_VENDOR;

    return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx, reinterpret_cast<unsigned char*>(data.data()), len, 0) >= 0;;
}

bool UniversalDevice::write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len)
{
    if(!m_device_data.HandlesOpen) return false;
    
    return libusb_bulk_transfer(m_device_data.Handle, endpoint, reinterpret_cast<unsigned char*>(buffer.data()), len, NULL, 0) == 0;
}

bool UniversalDevice::read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len)
{
    if(!m_device_data.HandlesOpen) return false;
    return libusb_bulk_transfer(m_device_data.Handle, endpoint, reinterpret_cast<unsigned char*>(buffer.data()), len, reinterpret_cast<int*>(&len), 0) == 0;
}

}

}