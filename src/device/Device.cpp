#include "usbtingo/device/Device.hpp"
#include "DeviceProtocol.hpp"

#include <cmath>

namespace usbtingo{

namespace device{

Device::Device(std::uint32_t serial)
    : m_serial(serial),
    m_device_info({ 0 }),
    m_mode(Mode::OFF),
    m_protocol(Protocol::CAN_2_0),
    m_flags(0),
    m_baudrate(0),
    m_baudrate_data(0),
    m_samplerate_hz(0),
    m_shutdown_status{AsyncIoState::IDLE},
    m_shutdown_logic{AsyncIoState::IDLE},
    m_shutdown_can{AsyncIoState::IDLE},
    m_buffer_status({ 0 }),
    m_buffer_logic({ 0 }),
    m_buffer_can({ 0 }),
    m_logic_stream_active(false)
{
    
}

Device::~Device()
{
    // cancel_async_can_request();
    // cancel_async_status_request();
    // close();
}

std::uint32_t Device::get_serial() const
{
    return m_serial;
}

DeviceInfo Device::get_device_info() const
{
    return m_device_info;
}

bool Device::is_alive()
{
    std::uint32_t serial = 0;
    if (!read_usbtingo_serial(serial)) return false;

    return m_serial == serial;
}

bool Device::stop_logic_stream() 
{
    if(!m_logic_stream_active) return false;
    m_logic_stream_active = !write_control(USBTINGO_CMD_LOGIC_SETCONFIG, 0, 0);

    return !m_logic_stream_active;
}
    
bool Device::start_logic_stream(std::uint32_t samplerate_hz)
{
    if(m_logic_stream_active) return false;

    if(samplerate_hz == 0)
    {
        samplerate_hz = (m_protocol == Protocol::CAN_2_0) ? m_baudrate * 10 : m_baudrate_data * 10;
    }

    std::uint8_t prescaler = static_cast<std::uint8_t>(
        std::clamp(
            static_cast<int>(std::round(120000000.0F / samplerate_hz)),
            3,      // lower limit
            0xff    // upper limit
        )
    );
    
    m_samplerate_hz = static_cast<std::uint32_t>(std::round(120000000.0F / prescaler));
    m_logic_stream_active = write_control(USBTINGO_CMD_LOGIC_SETCONFIG, static_cast<std::uint16_t>(0 | (prescaler << 8)), 0);

    return m_logic_stream_active;
}

bool Device::process_can_buffer(const std::uint8_t* rx_buffer, std::size_t rx_len, std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames)
{
    
    std::size_t msg_idx = 0, package_counter = 0;

    // package_counter as timeout, theoretically max. 42 tx_event messages per transfer
    while (msg_idx < rx_len && package_counter < USB_BULK_MAX_MESSAGE_COUNT) {

        if (rx_buffer[msg_idx] == USBTINGO_RXMSG_TYPE_CAN) {
            CanRxFrame rx_frame = { 0 };
            if (CanRxFrame::deserialize_can_frame(&rx_buffer[msg_idx], rx_frame)) rx_frames.push_back(rx_frame);
        }
        else if (rx_buffer[msg_idx] == USBTINGO_RXMSG_TYPE_TXEVENT) {
            TxEventFrame tx_event_frame = { 0 };
            if (TxEventFrame::deserialize_tx_event(&rx_buffer[msg_idx], tx_event_frame)) tx_event_frames.push_back(tx_event_frame);
        }
        else if (rx_buffer[msg_idx] == USBTINGO_RXMSG_TYPE_PADDING) {

        }

        msg_idx += USBTINGO_HEADER_SIZE_BYTES + (rx_buffer[msg_idx + 1] * 4);
        package_counter++;
    }

    return (package_counter < USB_BULK_MAX_MESSAGE_COUNT) ? true : false;
}

bool Device::set_mode(Mode mode)
{
    m_mode = mode;
    return write_control(USBTINGO_CMD_SET_MODE, static_cast<std::uint16_t>(mode), 0);
}

bool Device::set_protocol(Protocol protocol, std::uint8_t flags)
{
    m_flags = flags;
    m_protocol = protocol;
    return write_control(USBTINGO_CMD_SET_PROTOCOL, static_cast<std::uint16_t>(static_cast<std::uint8_t>(protocol) | flags << 8), 0);
}

bool Device::set_baudrate(std::uint32_t baudrate)
{
    m_baudrate = baudrate;
    m_baudrate_data = baudrate;
    return set_baudrate(baudrate, baudrate);
}

bool Device::set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data)
{
    bool success = true;
    m_baudrate = baudrate;
    m_baudrate_data = baudrate_data;

    success = write_control(USBTINGO_CMD_SET_BAUDRATE, 0, 0, reinterpret_cast<std::uint8_t*>(&baudrate), 4);
    if (!success) return false;

    success = write_control(USBTINGO_CMD_SET_BAUDRATE, 1, 0, reinterpret_cast<std::uint8_t*>(&baudrate_data), 4);
    return success;
}

bool Device::clear_errors() {
    return write_control(USBTINGO_CMD_CLEAR_ERRORFLAGS, 0xffff, 0);
}

bool Device::read_status(StatusFrame& status)
{
    std::vector<std::uint8_t> status_buffer(64);
    if(!read_control(USBTINGO_CMD_GET_STATUSREPORT, 0, 0, status_buffer, static_cast<uint16_t>(status_buffer.size()))) return false;
    return StatusFrame::deserialize_status(status_buffer.data(), status);
}

bool Device::send_can(const device::CanTxFrame& tx_frame)
{
    const std::size_t msg_size = CanTxFrame::buffer_size_bytes(tx_frame);

    BulkBuffer tx_buffer = { 0 };
    if (!CanTxFrame::serialize_can_frame(tx_buffer.data(), tx_frame)) return false;
    return write_bulk(USBTINGO_EP3_CANMSG_OUT, tx_buffer, msg_size);
}

bool Device::send_can(const std::vector<device::CanTxFrame>& tx_frames)
{
    std::size_t msg_size = 0, current_msg_size = 0;
    BulkBuffer tx_buffer = { 0 };

    for (const auto& tx_frame : tx_frames) {
        current_msg_size = CanTxFrame::buffer_size_bytes(tx_frame);

        // Send current buffer if the next message doesn't fit in
        if ((msg_size + current_msg_size) > USB_BULK_BUFFER_SIZE) {
            if(!write_bulk(USBTINGO_EP3_CANMSG_OUT, tx_buffer, msg_size)) return false;
            msg_size = 0;
        }

        // Add next message to the buffer
        if (!CanTxFrame::serialize_can_frame(tx_buffer.data() + msg_size, tx_frame)) return false;
        msg_size += current_msg_size;
    }
    
    return write_bulk(USBTINGO_EP3_CANMSG_OUT, tx_buffer, msg_size);
}

bool Device::receive_can(std::vector<device::CanRxFrame>& rx_frames, std::vector<device::TxEventFrame>& tx_event_frames)
{
    BulkBuffer rx_buffer = { 0 };
    std::size_t rx_len = USB_BULK_BUFFER_SIZE;
    
    // synchronous operation, blocks until data is available
    if(!read_bulk(USBTINGO_EP3_CANMSG_IN, rx_buffer, rx_len)) return false;

    return process_can_buffer(reinterpret_cast<std::uint8_t*>(&rx_buffer), rx_len, rx_frames, tx_event_frames);
}

bool Device::read_usbtingo_info()
{
    std::vector<std::uint8_t> data;
    if (!read_control(USBTINGO_CMD_GET_DEVICEINFO, 0, 0, data, 12)) return false;

    m_device_info.fw_minor = data.at(0);
    m_device_info.fw_major = data.at(1);
    m_device_info.hw_model = data.at(2);
    m_device_info.channels = data.at(3);
    m_device_info.uniqe_id = serialize_uint32(data.at(4), data.at(5), data.at(6), data.at(7));
    m_device_info.clock_hz = serialize_uint32(data.at(8), data.at(9), data.at(10), data.at(11));

    return true;
}

bool Device::read_usbtingo_serial(std::uint32_t& serial)
{
    serial = 0;
    return false;
}

bool Device::write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len)
{
    return false;
}

bool Device::read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len)
{
    return false;
}

//virtual bool Device::request_bulk_async(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len){}

//virtual bool Device::read_bulk_async(std::size_t& len){}

bool Device::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx)
{
    return false;
}

bool Device::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data)
{
    return false;
}

bool Device::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len)
{
    return false;
}

bool Device::read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len)
{
    return false;
}

bool Device::write_mcan_registers(std::uint16_t address, std::vector<uint32_t>& data)
{
    // assumes little endian device
    auto raw_size = data.size() * sizeof(std::uint32_t);
    return write_control(USBTINGO_CMD_MCAN_REG_WRITE, address, 0, reinterpret_cast<std::uint8_t*>(data.data()), static_cast<std::uint16_t>(raw_size));
}

bool Device::read_mcan_registers(std::uint16_t address, std::vector<uint32_t>& data, std::size_t len)
{
    data.clear();
    std::vector<std::uint8_t> data_u8;
    auto len_u8 = len * sizeof(std::uint32_t);

    if(!read_control(USBTINGO_CMD_MCAN_REG_READ, address, 0, data_u8, static_cast<std::uint16_t>(len_u8)))
        return false;

    data.reserve(len);
    for (size_t i = 0; i + 3 < data_u8.size(); i += 4) {
        std::uint32_t value = serialize_uint32(data_u8[i + 0], data_u8[i + 1], data_u8[i + 2], data_u8[i + 3]);
        data.push_back(value);
    }

    return true;
}

}

}