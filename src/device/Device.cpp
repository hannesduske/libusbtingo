#include "usbtingo/device/Device.hpp"

#include "DeviceProtocol.hpp"

namespace usbtingo{

namespace device{

Device::Device(std::uint32_t serial) : m_serial(serial){
    
}

std::uint32_t Device::get_serial() const{
    return m_serial;
}

DeviceInfo Device::get_device_info() const{
    return m_device_info;
}

bool Device::process_can_buffer(const std::uint8_t* rx_buffer, std::size_t rx_len, std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) {
    
    std::size_t msg_idx = 0, package_counter = 0;

    // package_counter as timeout, theroretically max. 42 tx_event messages per transfer
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

}

}