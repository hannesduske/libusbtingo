#include "WinDevice.hpp"

#include <SetupAPI.h>
#include <Usbiodef.h>

namespace usbtingo{

namespace device{

// init static class member
std::map<unsigned long, std::string> WinDevice::m_usbtingos = { };


WinDevice::WinDevice(std::uint32_t serial, std::string path) :
    Device(serial)
{
    m_device_data.DevicePath = path;
    WinDevice::open_usb_device(m_device_data);
    WinDevice::read_usbtingo_info(m_device_data, m_device_info);
}

std::unique_ptr<Device> WinDevice::create_device(std::uint32_t serial)
{
    WinDevice::detect_usbtingos();
    const auto it = m_usbtingos.find(serial);
    
    if (it != m_usbtingos.end()) {
        auto device = std::make_unique<WinDevice>(serial, it->second);
        if (device->is_alive()) {
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

bool WinDevice::is_alive() const
{
    if (!static_cast<bool>(m_device_data.HandlesOpen)) return false;

    std::uint32_t serial = 0;

    if (FAILED(WinDevice::read_usbtingo_serial(m_device_data, serial)))
    {
        return false;
    }

    return m_serial == serial;
}

bool WinDevice::set_mode(Mode mode)
{
    return write_control(m_device_data, USBTINGO_CMD_SET_MODE, static_cast<std::uint8_t>(mode), 0);
}

bool WinDevice::set_protocol(Protocol protocol)
{
    return write_control(m_device_data, USBTINGO_CMD_SET_PROTOCOL, static_cast<std::uint8_t>(protocol), 0);
}

bool WinDevice::set_baudrate(std::uint32_t baudrate)
{
    return set_baudrate(baudrate, baudrate);
}

bool WinDevice::set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data)
{
    bool success = true;

    success = write_control(m_device_data, USBTINGO_CMD_SET_BAUDRATE, 0, 0, reinterpret_cast<std::uint8_t*>(&baudrate), 4);
    if (!success) return false;

    success = write_control(m_device_data, USBTINGO_CMD_SET_BAUDRATE, 1, 0, reinterpret_cast<std::uint8_t*>(&baudrate), 4);
    return success;
}

void WinDevice::send_can(const can::Message& msg)
{

}

void WinDevice::receive_can(can::Message& msg)
{

}

void WinDevice::receive_status(Status& status)
{

}

/**
 *   @brief Get a vector of the serial numbers of all connected USBtingo devices.
 *   @return Vector of serial numbers of all connected USBtingo devices.
 */
std::vector<std::uint32_t> WinDevice::detect_available_devices()
{
    std::vector<std::uint32_t> serial_vec;

    HRESULT hr = S_OK;
    hr = WinDevice::detect_usbtingos();
    
    if(FAILED(hr)) return serial_vec;

    for (const auto& usbtingo : m_usbtingos) {
        serial_vec.push_back(usbtingo.first);
    }
    
    return serial_vec;
}

/**
 * @brief Updates the static private map of the serial numbers of all connected USBtingo devices and their device paths.
 * @return true if operation succeeded
 */
bool WinDevice::detect_usbtingos()
{
    HRESULT hr;
    m_usbtingos.clear();

    std::vector<std::string> devices;
    hr = WinDevice::detect_usb_devices(devices, USBTINGO_VID, USBTINGO_PID);

    // Failed to fetch USB Devices
    if (FAILED(hr)) return false;

    // No device with the USBtingo vid and pid found
    if (devices.empty()) return false;

    for (const auto& dev : devices) {
        std::uint32_t serial = 0;

        DeviceData device_data;
        device_data.DevicePath = dev;

        hr &= WinDevice::open_usb_device(device_data);
        hr &= WinDevice::read_usbtingo_serial(device_data, serial);
        hr &= WinDevice::close_usb_device(device_data);

        if (SUCCEEDED(hr)) m_usbtingos.emplace(serial, dev);
    }

    return false;
}

/**
 * @brief Get the serial number from a USBtingo device.
 * @param[in] device_data Device data structure
 * @return HRESULT
 */
HRESULT WinDevice::read_usbtingo_serial(const DeviceData& device_data, std::uint32_t& serial)
{
    serial = 0;
    if (!device_data.HandlesOpen) return E_ABORT;

    BOOL                    bResult;
    HRESULT                 hr = S_OK;
    USB_DEVICE_DESCRIPTOR   deviceDesc = { 0 };
    ULONG                   lengthReceived;

    // Get device descriptor
    bResult = WinUsb_GetDescriptor(device_data.WinusbHandle,
        USB_DEVICE_DESCRIPTOR_TYPE,
        0,
        0,
        (PBYTE)&deviceDesc,
        sizeof(deviceDesc),
        &lengthReceived);

    // Unable to get device description
    if (FALSE == bResult) {
        return GetLastError();
    }

    // Wrong buffer length received
    if (lengthReceived != sizeof(deviceDesc)) {
        return E_FAIL;
    }

    std::string manufacturer, product, serialNumber;

    // Get Manufacturer String
    if (FAILED(WinDevice::read_usb_descriptor(device_data, deviceDesc.iManufacturer, USBTINGO_LANGUAGEID, manufacturer))) {
        return GetLastError();
    }

    // Get Product String
    if (FAILED(WinDevice::read_usb_descriptor(device_data, deviceDesc.iProduct, USBTINGO_LANGUAGEID, product))) {
        return GetLastError();
    }

    // Check if device is a USBtingo
    if (manufacturer == USBTINGO_MANUFACTURER && product == USBTINGO_PRODUCT) {

        // Get Serial Number String
        if (FAILED(WinDevice::read_usb_descriptor(device_data, deviceDesc.iSerialNumber, USBTINGO_LANGUAGEID, serialNumber))) {
            return GetLastError();
        }

        // Strip trailing "F" characters
        serialNumber.erase(std::remove(serialNumber.begin(), serialNumber.end(), L'F'), serialNumber.end());
        serial = std::stoul(serialNumber);
    }

    return hr;
}

/**
 * @brief Get the string value of the WinUsb descriptor at the specified index.
 * @param[in] device_data Device data structure
 * @param[in] index Index of the descriptor. Usually specified in the device descriptor.
 * @param[in] languageID Language ID of the descriptor.
 * @param[out] value String returning the value of the device descriptor. Max buffer length is hardcoded to 256.
 * @return HRESULT
 */
HRESULT WinDevice::read_usb_descriptor(const DeviceData& device_data, const std::uint8_t index, const std::uint16_t languageID, std::string& value)
{
    if (!device_data.HandlesOpen) return E_ABORT;

    BOOL bResult;
    WCHAR buffer[256] = { 0 };
    ULONG lengthTransferred;
    ULONG bufferLength = sizeof(buffer);

    value.clear();

    bResult = WinUsb_GetDescriptor(device_data.WinusbHandle,
        USB_STRING_DESCRIPTOR_TYPE,
        index,
        languageID,
        (PBYTE)buffer,
        bufferLength,
        &lengthTransferred);
    
    if(!bResult){
        return GetLastError();
    }

    // Std implementation, but causes deprecation warning. Requires headers <locale> and <codecvt>.
    //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    //value = converter.to_bytes(buffer);
    
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
    if (bufferSize == 0) return E_FAIL;

    value.resize(bufferSize - 1);
    int result = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &value[0], bufferSize, nullptr, nullptr);
    if (result == 0) return E_FAIL;

    if (value.size() >= 2) {
        //// Convert from raw buffer to string, skip the first 2 bytes
        value.erase(0, 2);
    }
    else {
        return E_FAIL;
    }

    return S_OK;
}

/**
 * @brief Open all needed handles to interact with a WinUsb device.
 * @param[in] device_data Device data structure
 * @return HRESULT
 */
HRESULT WinDevice::open_usb_device(DeviceData& device_data)
{
    HRESULT hr = S_OK;
    BOOL    bResult;

    device_data.HandlesOpen = FALSE;

    device_data.DeviceHandle = CreateFile(device_data.DevicePath.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);

    if (INVALID_HANDLE_VALUE == device_data.DeviceHandle) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    bResult = WinUsb_Initialize(device_data.DeviceHandle,
        &device_data.WinusbHandle);

    if (FALSE == bResult) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(device_data.DeviceHandle);
        return hr;
    }

    //wprintf(L"opening device: %s\n", device_data.DevicePath);
    device_data.HandlesOpen = TRUE;
    return hr;
}

/**
 * @brief Close all handles of a WinUsb device.
 * @param[in] device_data Device data structure
 */
HRESULT WinDevice::close_usb_device(DeviceData& device_data)
{
    if (FALSE == device_data.HandlesOpen) {
        // Called on an uninitialized device_data
        return E_FAIL;
    }

    //wprintf(L"closing device: %s\n", Devdevice_dataiceData->DevicePath);
    WinUsb_Free(device_data.WinusbHandle);
    CloseHandle(device_data.DeviceHandle);
    device_data.HandlesOpen = FALSE;

    return S_OK;
}

/**
 * @brief Find all connected USB Devices with the specified vid and pid. If vid or pid is NULL the respective field is handled as wildcard.
 * @param[in] vid
 * @param[in] pid
 * @return HRESULT
 */
HRESULT WinDevice::detect_usb_devices(std::vector<std::string>& devices, std::uint16_t vid, std::uint16_t pid)
{
    HRESULT hr = S_OK;
    devices.clear();

    HDEVINFO deviceInfoSet;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData;
    DWORD requiredSize = 0;
    GUID guid = GUID_DEVINTERFACE_USB_DEVICE;
    int deviceIndex = 0;

    // Get a handle to the device information set
    deviceInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        return E_HANDLE;
    }

    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Enumerate the device interfaces
    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &guid, deviceIndex++, &deviceInterfaceData)) {
        // Get the required size for the detail data
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        deviceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!deviceDetailData) {
            return E_OUTOFMEMORY;
        }
        deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // Get the device interface detail
        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceDetailData, requiredSize, NULL, NULL)) {

            const auto device_path = std::string(deviceDetailData->DevicePath);
            const std::size_t vid_pos = device_path.find("vid_");
            const std::size_t pid_pos = device_path.find("pid_");

            //if (vidPtr && pidPtr) {
            if((vid_pos != std::string::npos) && (pid_pos != std::string::npos)){

                std::uint16_t vid_tmp = static_cast<uint16_t>(std::stoi(device_path.substr(vid_pos + 4, 4), nullptr, 16));
                std::uint16_t pid_tmp = static_cast<uint16_t>(std::stoi(device_path.substr(pid_pos + 4, 4), nullptr, 16));

                // Check if the vid and pid match the current device.
                bool match = true;
                if (vid != 0) match &= (vid_tmp == vid);
                if (pid != 0) match &= (pid_tmp == pid);

                if (match) {
                    //printf("Found device\n");
                    devices.emplace_back(deviceDetailData->DevicePath);
                }
            }
        }
        else {
            return E_FAIL;
        }

        free(deviceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return hr;
}

HRESULT WinDevice::read_usbtingo_info(const DeviceData& device_data, DeviceInfo& device_info)
{
    if(!device_data.HandlesOpen) return E_FAIL;

    std::vector<std::uint8_t> data;
    if(!read_control(device_data, USBTINGO_CMD_GET_DEVICEINFO, 0, 0, data, 12)) return E_FAIL;

    device_info.fw_minor = data.at(0);
    device_info.fw_major = data.at(1);
    device_info.hw_model = data.at(2);
    device_info.channels = data.at(3);
    device_info.uniqe_id = (data.at(4) << 0) | (data.at(5) << 8) | (data.at(6) << 16) | (data.at(7) << 24);
    device_info.clock_hz = (data.at(8) << 0) | (data.at(9) << 8) | (data.at(10) << 16) | (data.at(11) << 24);

    return S_OK;
}

/**
 * @brief Write data to the control endpoint of a WinUsb Device
 * @param[in] cmd Command field
 * @param[in] val Value field
 * @param[in] idx Index field
 * @return true if operation succeeded
 */
bool WinDevice::write_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx)
{
    if(!device_data.HandlesOpen) return false;

    WINUSB_SETUP_PACKET setup_packet = { 0 };
    setup_packet.RequestType = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
    setup_packet.Request = cmd;
    setup_packet.Value = val;
    setup_packet.Index = idx;
    setup_packet.Length = 0;

    return static_cast<bool>(WinUsb_ControlTransfer(device_data.WinusbHandle, setup_packet, NULL, NULL, NULL, NULL));
}

/**
 * @brief Write data to the control endpoint of a WinUsb Device
 * @param[in] cmd Command field
 * @param[in] val Value field
 * @param[in] idx Index field
 * @param[in] data Data to transmit to control endpoint
 * @return true if operation succeeded
 */
bool WinDevice::write_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, const std::vector<std::uint8_t>& data)
{
    if(!device_data.HandlesOpen) return false;

    ULONG lengthReceived = 0;
    WINUSB_SETUP_PACKET setup_packet = { 0 };
    setup_packet.RequestType = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
    setup_packet.Request = cmd;
    setup_packet.Value = val;
    setup_packet.Index = idx;
    setup_packet.Length = static_cast<std::uint8_t>(data.size());

    return static_cast<bool>(WinUsb_ControlTransfer(device_data.WinusbHandle, setup_packet, (PBYTE)data.data(), setup_packet.Length, &lengthReceived, NULL));
}

/**
 * @brief Write data to the control endpoint of a WinUsb Device
 * @param[in] cmd Command field
 * @param[in] val Value field
 * @param[in] idx Index field
 * @param[in] data Data buffer to transmit to control endpoint
 * @param[in] len Number of bytes to transmit from data buffer
 * @return true if operation succeeded
 */
bool WinDevice::write_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, const std::uint8_t* data, std::uint16_t len)
{
    if(!device_data.HandlesOpen) return false;

    ULONG lengthReceived = 0;
    WINUSB_SETUP_PACKET setup_packet = { 0 };
    setup_packet.RequestType = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
    setup_packet.Request = cmd;
    setup_packet.Value = val;
    setup_packet.Index = idx;
    setup_packet.Length = len;

    return static_cast<bool>(WinUsb_ControlTransfer(device_data.WinusbHandle, setup_packet, (PBYTE)data, setup_packet.Length, &lengthReceived, NULL));
}

/**
 * @brief Read data from the control endpoint of a WinUsb Device
 * @param[in] cmd Command field
 * @param[in] val Value field
 * @param[in] idx Index field
 * @param[out] data Data buffer to store data from control endpoint
 * @return true if operation succeeded
 */
bool WinDevice::read_control(const DeviceData& device_data, std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len)
{
    if(!device_data.HandlesOpen) return false;

    data.clear();
    data.resize(len);

    ULONG lengthReceived = 0;
    WINUSB_SETUP_PACKET setup_packet = { 0 };
    setup_packet.RequestType = USB_REQUEST_DEVICE2HOST | USB_REQUEST_TYPE_VENDOR;
    setup_packet.Request = cmd;
    setup_packet.Value = val;
    setup_packet.Index = idx;
    setup_packet.Length = len;

    return static_cast<bool>(WinUsb_ControlTransfer(device_data.WinusbHandle, setup_packet, (PBYTE)data.data(), setup_packet.Length, &lengthReceived, NULL));
}

/**
 * @brief Write data to the bulk endpoint of a WinUsb Device
 * @param[in] endpoint Endpoint ID bEndpoint
 * @param[in] buffer Data buffer to transmit to control endpoint
 * @param[in] len Number of bytes to transmit from data buffer
 * @return true if operation succeeded
 */
bool WinDevice::write_bulk(const DeviceData& device_data, std::uint8_t endpoint, const BulkBuffer& buffer, std::size_t len)
{
    if(!device_data.HandlesOpen) return false;
    return static_cast<bool>(WinUsb_ReadPipe(device_data.WinusbHandle, endpoint, (PBYTE)buffer.data(), static_cast<ULONG>(len), NULL, NULL));
}

/**
 * @brief Read data from the bulk endpoint of a WinUsb Device
 * @param[in] endpoint Endpoint ID bEndpoint
 * @param[in] buffer Data buffer to store data from bulk endpoint
 * @param[in] len Number of bytes to transmit from data buffer
 * @return true if operation succeeded
 */
bool WinDevice::read_bulk(const DeviceData& device_data, std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len)
{
    if(!device_data.HandlesOpen) return false;
    ULONG lengthReceived = 0;
    return static_cast<bool>(WinUsb_ReadPipe(device_data.WinusbHandle, endpoint, (PBYTE)buffer.data(), static_cast<ULONG>(len), &lengthReceived, NULL));
}

}

}