#include "usbtingo/basic_bus/BasicBus.hpp"

#include "usbtingo/device/DeviceFactory.hpp"

namespace usbtingo {

namespace bus {

BasicBus::BasicBus(std::unique_ptr<device::Device> device)
    : Bus(std::move(device)) {
}

std::unique_ptr<BasicBus> BasicBus::create(std::uint32_t baudrate, std::uint32_t data_baudrate, device::Protocol protocol, device::Mode mode) {
  return create(0, baudrate, data_baudrate, protocol, mode);
}

std::unique_ptr<BasicBus> BasicBus::create(std::size_t idx, std::uint32_t baudrate, std::uint32_t data_baudrate, device::Protocol protocol, device::Mode mode) {
  auto serial_vec = device::DeviceFactory::detect_available_devices();
  if (serial_vec.size() <= idx)
    return nullptr;

  auto device = device::DeviceFactory::create(serial_vec.at(idx));
  if (!device)
    return nullptr;

  device->set_mode(device::Mode::OFF);
  device->set_protocol(protocol, 0b00010000); // disable automatic retransmission of failed messages
  device->set_baudrate(baudrate, data_baudrate);
  device->set_mode(mode);

  return std::unique_ptr<BasicBus>(new BasicBus(std::move(device)));
}

bool BasicBus::add_listener(bus::BasicListener* listener) {
  return Bus::add_listener(reinterpret_cast<bus::CanListener*>(listener));
}

bool BasicBus::remove_listener(const bus::BasicListener* listener) {
  return Bus::remove_listener(reinterpret_cast<const bus::CanListener*>(listener));
}

bool BasicBus::send(const bus::Message msg) {
  // auto is_fd = !m_pimpl->device->get_protocol() == Protocol::CAN_2_0;
  return Bus::send(msg.to_CanTxFrame());
}

} // namespace bus

} // namespace usbtingo