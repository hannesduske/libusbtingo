#include "usbtingo/bus/Bus.hpp"

#include "BusImpl.hpp"


namespace usbtingo {

namespace bus {

Bus::Bus(std::unique_ptr<device::Device> device)
    : m_pimpl(std::make_unique<BusImpl>(std::move(device))) {
}

Bus::~Bus() = default;

Bus::Bus(Bus&&)            = default;
Bus& Bus::operator=(Bus&&) = default;

bool Bus::start() {
  return m_pimpl->start();
}
bool Bus::stop() {
  return m_pimpl->stop();
}

bool Bus::add_listener(bus::CanListener* listener) {
  return m_pimpl->add_listener(listener);
}
bool Bus::add_listener(bus::LogicListener* listener) {
  return m_pimpl->add_listener(listener);
}
bool Bus::add_listener(bus::StatusListener* listener) {
  return m_pimpl->add_listener(listener);
}
bool Bus::remove_listener(const bus::CanListener* listener) {
  return m_pimpl->remove_listener(listener);
}
bool Bus::remove_listener(const bus::LogicListener* listener) {
  return m_pimpl->remove_listener(listener);
}
bool Bus::remove_listener(const bus::StatusListener* listener) {
  return m_pimpl->remove_listener(listener);
}

bool Bus::send(const device::CanTxFrame msg) {
  return m_pimpl->send(msg);
}

bool Bus::start_logic_stream(std::uint32_t samplerate_hz) {
  return m_pimpl->start_logic_stream(samplerate_hz);
}
bool Bus::stop_logic_stream() {
  return m_pimpl->stop_logic_stream();
}

}; // namespace bus

}; // namespace usbtingo