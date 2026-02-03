#include "UniversalDevice.hpp"

#include <libusb-1.0/libusb.h>
#include <stdexcept>
#include <string>

#include "UsbLoader.hpp"

namespace usbtingo {

namespace device {

std::mutex& UniversalDevice::get_existing_devs_mutex() {
  static auto* s_mutex = new std::mutex;
  return *s_mutex;
}

std::set<std::uint32_t>& UniversalDevice::get_existing_devs() {
  static auto* s_existing_devs = new std::set<std::uint32_t>;
  return *s_existing_devs;
}

bool UniversalDevice::parse_serial_number(const unsigned char* buffer, std::uint32_t& serial) {
  try {
    serial = static_cast<std::uint32_t>(std::stoul(reinterpret_cast<const char*>(buffer), nullptr, 16));
    return true;
  } catch (const std::invalid_argument&) {
    return false;
  } catch (const std::out_of_range&) {
    return false;
  }
}

bool UniversalDevice::allocate_transfers() {
  m_async_can = libusb_alloc_transfer(0);
  if (!m_async_can) {
    return false;
  }

  m_async_logic = libusb_alloc_transfer(0);
  if (!m_async_logic) {
    libusb_free_transfer(m_async_can);
    m_async_can = nullptr;
    return false;
  }

  m_async_status = libusb_alloc_transfer(0);
  if (!m_async_status) {
    libusb_free_transfer(m_async_can);
    libusb_free_transfer(m_async_logic);
    m_async_can   = nullptr;
    m_async_logic = nullptr;
    return false;
  }

  return true;
}

void UniversalDevice::free_transfers() {
  // Cancel any active transfers before freeing
  if (m_async_can) {
    if (m_shutdown_can.load() == AsyncIoState::REQUEST_ACTIVE) {
      libusb_cancel_transfer(m_async_can);
      // Wait briefly for cancellation to complete
      for (int i = 0; i < 100 && m_shutdown_can.load() == AsyncIoState::REQUEST_ACTIVE; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }
    libusb_free_transfer(m_async_can);
    m_async_can = nullptr;
  }

  if (m_async_logic) {
    if (m_shutdown_logic.load() == AsyncIoState::REQUEST_ACTIVE) {
      libusb_cancel_transfer(m_async_logic);
      for (int i = 0; i < 100 && m_shutdown_logic.load() == AsyncIoState::REQUEST_ACTIVE; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }
    libusb_free_transfer(m_async_logic);
    m_async_logic = nullptr;
  }

  if (m_async_status) {
    if (m_shutdown_status.load() == AsyncIoState::REQUEST_ACTIVE) {
      libusb_cancel_transfer(m_async_status);
      for (int i = 0; i < 100 && m_shutdown_status.load() == AsyncIoState::REQUEST_ACTIVE; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }
    libusb_free_transfer(m_async_status);
    m_async_status = nullptr;
  }
}

void UniversalDevice::setup_transfer(libusb_transfer* transfer, std::uint8_t endpoint,
                                     unsigned char* buffer, int length, libusb_transfer_type type) {
  transfer->dev_handle = m_device_data.Handle;
  transfer->endpoint   = endpoint;
  transfer->type       = type;
  transfer->timeout    = 0;
  transfer->buffer     = buffer;
  transfer->length     = length;
  transfer->user_data  = static_cast<void*>(this);
}

UniversalDevice::UniversalDevice(std::uint32_t serial, libusb_device* dev)
    : Device(serial)
    , m_device_data({0}) {

  if (!allocate_transfers()) {
    // Leave in invalid state - is_alive() will return false
    return;
  }

  m_device_data.Device = dev;
  if (open()) {
    UniversalDevice::read_usbtingo_info();

    // Setup CAN transfer
    setup_transfer(m_async_can, USBTINGO_EP3_CANMSG_IN,
                   static_cast<unsigned char*>(m_buffer_can.data()),
                   USB_BULK_BUFFER_SIZE, LIBUSB_TRANSFER_TYPE_BULK);
    m_async_can->callback = [](libusb_transfer* transfer) {
      auto* instance = static_cast<UniversalDevice*>(transfer->user_data);
      if (instance) {
        instance->handle_can_async_callback(transfer);
      }
    };

    // Setup logic transfer
    setup_transfer(m_async_logic, USBTINGO_EP2_LOGIC_IN,
                   static_cast<unsigned char*>(m_buffer_logic.data()),
                   USB_BULK_BUFFER_SIZE, LIBUSB_TRANSFER_TYPE_BULK);
    m_async_logic->callback = [](libusb_transfer* transfer) {
      auto* instance = static_cast<UniversalDevice*>(transfer->user_data);
      if (instance) {
        instance->handle_logic_async_callback(transfer);
      }
    };

    // Setup status transfer (interrupt type)
    setup_transfer(m_async_status, USBTINGO_EP1_STATUS_IN,
                   static_cast<unsigned char*>(m_buffer_status.data()),
                   USB_BULK_BUFFER_SIZE_STATUS, LIBUSB_TRANSFER_TYPE_INTERRUPT);
    m_async_status->callback = [](libusb_transfer* transfer) {
      auto* instance = static_cast<UniversalDevice*>(transfer->user_data);
      if (instance) {
        instance->handle_status_async_callback(transfer);
      }
    };
  }
}

UniversalDevice::~UniversalDevice() {
  close();
  free_transfers();

  // Remove from existing devices set with thread safety
  std::lock_guard<std::mutex> lock(get_existing_devs_mutex());
  auto& existing_devs = get_existing_devs();
  existing_devs.erase(m_serial);
}

std::unique_ptr<Device> UniversalDevice::create_device(std::uint32_t serial) {
  std::lock_guard<std::mutex> lock(get_existing_devs_mutex());
  auto& existing_devs = get_existing_devs();

  // Only one instance per unique device
  if (serial != 0 && existing_devs.find(serial) != existing_devs.end()) {
    return nullptr;
  }

  const auto dev_map = UniversalDevice::detect_usbtingos();
  decltype(dev_map)::const_iterator it;

  // If serial==0, find next free device
  if (serial == 0 && dev_map.size() > existing_devs.size()) {
    it = std::find_if(dev_map.begin(), dev_map.end(), [&existing_devs](const auto& p) {
      return existing_devs.find(p.first) == existing_devs.end();
    });
    if (it == dev_map.end()) {
      return nullptr;
    }
    serial = it->first;
  } else {
    it = dev_map.find(serial);
  }

  if (it == dev_map.end()) {
    return nullptr;
  }

  auto device = std::make_unique<UniversalDevice>(serial, it->second);

  if (!device->is_alive()) {
    return nullptr;
  }

  existing_devs.insert(serial);
  return device;
}

std::vector<std::uint32_t> UniversalDevice::detect_available_devices() {
  std::vector<std::uint32_t> serial_vec;
  const auto dev_map = UniversalDevice::detect_usbtingos();

  for (const auto& it : dev_map) {
    serial_vec.push_back(it.first);
  }

  return serial_vec;
}

std::map<std::uint32_t, libusb_device*> UniversalDevice::detect_usbtingos() {
  std::map<std::uint32_t, libusb_device*> dev_map;

  libusb_device** devs = nullptr;
  const int device_count = libusb_get_device_list(get_usb_ctx(), &devs);

  // Failed to fetch USB Devices
  if (device_count < 0 || !devs) {
    return dev_map;
  }

  for (int i = 0; i < device_count; ++i) {
    libusb_device* dev = devs[i];
    if (!dev) {
      continue;
    }

    libusb_device_descriptor desc;
    if (libusb_get_device_descriptor(dev, &desc) < 0) {
      continue;
    }

    if (desc.idVendor != USBTINGO_VID || desc.idProduct != USBTINGO_PID) {
      continue;
    }

    libusb_device_handle* handle = nullptr;
    if (libusb_open(dev, &handle) < 0) {
      continue;
    }

    unsigned char buffer[16] = {0};
    const int result = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buffer, sizeof(buffer));

    if (result > 0) {
      std::uint32_t serial = 0;
      if (parse_serial_number(buffer, serial)) {
        dev_map.emplace(serial, dev);
      }
    }

    libusb_close(handle);
  }

  libusb_free_device_list(devs, 1);
  return dev_map;
}

bool UniversalDevice::open() {
  if (!m_device_data.HandlesOpen) {
    m_device_data.Handle = NULL;
    if (libusb_open(m_device_data.Device, &m_device_data.Handle) >= 0) {
      m_device_data.HandlesOpen = true;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool UniversalDevice::close() {
  if (m_device_data.HandlesOpen) {
    set_mode(Mode::OFF);
    libusb_close(m_device_data.Handle);
    m_device_data.HandlesOpen = false;
    return true;
  } else {
    return false;
  }
}

bool UniversalDevice::is_open() const {
  return m_device_data.HandlesOpen;
}

bool UniversalDevice::cancel_async_can_request() {
  bool success = m_shutdown_can.load() == AsyncIoState::REQUEST_ACTIVE;
  success &= libusb_cancel_transfer(m_async_can) == 0;

  m_shutdown_can.store(AsyncIoState::SHUTDOWN);

  return success;
}

bool UniversalDevice::cancel_async_logic_request() {
  bool success = m_shutdown_logic.load() == AsyncIoState::REQUEST_ACTIVE;
  success &= libusb_cancel_transfer(m_async_logic) == 0;

  m_shutdown_logic.store(AsyncIoState::SHUTDOWN);

  return success;
}

bool UniversalDevice::cancel_async_status_request() {
  bool success = m_shutdown_status.load() == AsyncIoState::REQUEST_ACTIVE;
  success &= libusb_cancel_transfer(m_async_status) == 0;

  m_shutdown_status.store(AsyncIoState::SHUTDOWN);

  return success;
}

std::future<bool> UniversalDevice::request_can_async() {
  if (!m_device_data.HandlesOpen || !m_async_can) {
    return std::future<bool>();
  }

  if (m_shutdown_can.load() == AsyncIoState::REQUEST_ACTIVE) {
    return std::future<bool>();
  }

  // Create promise before submitting transfer (thread-safe)
  std::lock_guard<std::mutex> lock(m_promise_mutex);
  m_promise_can = std::promise<bool>();
  auto future   = m_promise_can.get_future();

  m_shutdown_can.store(AsyncIoState::REQUEST_ACTIVE);
  if (libusb_submit_transfer(m_async_can) != 0) {
    m_shutdown_can.store(AsyncIoState::IDLE);
    return std::future<bool>();
  }

  return future;
}

std::future<bool> UniversalDevice::request_logic_async() {
  if (!m_device_data.HandlesOpen || !m_async_logic) {
    return std::future<bool>();
  }

  if (m_shutdown_logic.load() == AsyncIoState::REQUEST_ACTIVE) {
    return std::future<bool>();
  }

  // Create promise before submitting transfer (thread-safe)
  std::lock_guard<std::mutex> lock(m_promise_mutex);
  m_promise_logic = std::promise<bool>();
  auto future     = m_promise_logic.get_future();

  m_shutdown_logic.store(AsyncIoState::REQUEST_ACTIVE);
  if (libusb_submit_transfer(m_async_logic) != 0) {
    m_shutdown_logic.store(AsyncIoState::IDLE);
    return std::future<bool>();
  }

  return future;
}

std::future<bool> UniversalDevice::request_status_async() {
  if (!m_device_data.HandlesOpen || !m_async_status) {
    return std::future<bool>();
  }

  if (m_shutdown_status.load() == AsyncIoState::REQUEST_ACTIVE) {
    return std::future<bool>();
  }

  // Create promise before submitting transfer (thread-safe)
  std::lock_guard<std::mutex> lock(m_promise_mutex);
  m_promise_status = std::promise<bool>();
  auto future      = m_promise_status.get_future();

  m_shutdown_status.store(AsyncIoState::REQUEST_ACTIVE);
  if (libusb_submit_transfer(m_async_status) != 0) {
    m_shutdown_status.store(AsyncIoState::IDLE);
    return std::future<bool>();
  }

  return future;
}

bool UniversalDevice::receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) {
  if (m_shutdown_can.load() != AsyncIoState::DATA_AVAILABLE)
    return false;

  bool success = process_can_buffer(reinterpret_cast<std::uint8_t*>(&m_buffer_can), m_async_can->actual_length, rx_frames, tx_event_frames);
  m_shutdown_can.store(AsyncIoState::IDLE);

  return success;
}

bool UniversalDevice::receive_logic_async(LogicFrame& logic_frame) {
  if (m_shutdown_logic.load() != AsyncIoState::DATA_AVAILABLE)
    return false;

  logic_frame.data = m_buffer_logic;
  m_shutdown_logic.store(AsyncIoState::IDLE);

  return true;
}

bool UniversalDevice::receive_status_async(StatusFrame& status_frame) {
  if (m_shutdown_status.load() != AsyncIoState::DATA_AVAILABLE)
    return false;

  bool success = StatusFrame::deserialize_status(reinterpret_cast<std::uint8_t*>(&m_buffer_status), status_frame);
  m_shutdown_status.store(AsyncIoState::IDLE);

  return success;
}

void UniversalDevice::handle_can_async_callback(libusb_transfer* transfer) {
  std::lock_guard<std::mutex> lock(m_promise_mutex);
  try {
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
      m_shutdown_can.store(AsyncIoState::DATA_AVAILABLE);
      m_promise_can.set_value(true);
    } else {
      m_shutdown_can.store(AsyncIoState::SHUTDOWN);
      m_promise_can.set_value(false);
    }
  } catch (const std::future_error&) {
    // Promise already satisfied or no shared state - ignore
    m_shutdown_can.store(AsyncIoState::IDLE);
  }
}

void UniversalDevice::handle_logic_async_callback(libusb_transfer* transfer) {
  std::lock_guard<std::mutex> lock(m_promise_mutex);
  try {
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
      m_shutdown_logic.store(AsyncIoState::DATA_AVAILABLE);
      m_promise_logic.set_value(true);
    } else {
      m_shutdown_logic.store(AsyncIoState::SHUTDOWN);
      m_promise_logic.set_value(false);
    }
  } catch (const std::future_error&) {
    // Promise already satisfied or no shared state - ignore
    m_shutdown_logic.store(AsyncIoState::IDLE);
  }
}

void UniversalDevice::handle_status_async_callback(libusb_transfer* transfer) {
  std::lock_guard<std::mutex> lock(m_promise_mutex);
  try {
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
      m_shutdown_status.store(AsyncIoState::DATA_AVAILABLE);
      m_promise_status.set_value(true);
    } else {
      m_shutdown_status.store(AsyncIoState::SHUTDOWN);
      m_promise_status.set_value(false);
    }
  } catch (const std::future_error&) {
    // Promise already satisfied or no shared state - ignore
    m_shutdown_status.store(AsyncIoState::IDLE);
  }
}

bool UniversalDevice::read_usbtingo_serial(std::uint32_t& serial) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  libusb_device_descriptor desc;
  if (libusb_get_device_descriptor(m_device_data.Device, &desc) < 0) {
    return false;
  }

  unsigned char buffer[16] = {0};
  if (libusb_get_string_descriptor_ascii(m_device_data.Handle, desc.iManufacturer, buffer, sizeof(buffer)) < 0) {
    return false;
  }
  const std::string manufacturer(reinterpret_cast<const char*>(buffer));

  std::fill(buffer, buffer + sizeof(buffer), 0);
  if (libusb_get_string_descriptor_ascii(m_device_data.Handle, desc.iProduct, buffer, sizeof(buffer)) < 0) {
    return false;
  }
  const std::string product(reinterpret_cast<const char*>(buffer));

  if (manufacturer != USBTINGO_MANUFACTURER || product != USBTINGO_PRODUCT) {
    return false;
  }

  std::fill(buffer, buffer + sizeof(buffer), 0);
  if (libusb_get_string_descriptor_ascii(m_device_data.Handle, desc.iSerialNumber, buffer, sizeof(buffer)) < 0) {
    return false;
  }

  return parse_serial_number(buffer, serial);
}

bool UniversalDevice::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  const std::uint8_t request_type = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;

  return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx, nullptr, 0, 0) >= 0;
}

bool UniversalDevice::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  const std::uint8_t request_type  = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;
  const std::uint16_t length       = static_cast<std::uint16_t>(data.size());
  constexpr unsigned int timeout   = 0;

  return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx,
                                 static_cast<unsigned char*>(data.data()), length, timeout) >= 0;
}

bool UniversalDevice::write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  const std::uint8_t request_type = USB_REQUEST_HOST2DEVICE | USB_REQUEST_TYPE_VENDOR;

  return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx,
                                 reinterpret_cast<unsigned char*>(data), len, 0) >= 0;
}

bool UniversalDevice::read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx,
                                   std::vector<std::uint8_t>& data, std::uint16_t len) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  data.clear();
  data.resize(len);

  const std::uint8_t request_type = USB_REQUEST_DEVICE2HOST | USB_REQUEST_TYPE_VENDOR;

  return libusb_control_transfer(m_device_data.Handle, request_type, cmd, val, idx,
                                 reinterpret_cast<unsigned char*>(data.data()), len, 0) >= 0;
}

bool UniversalDevice::write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  return libusb_bulk_transfer(m_device_data.Handle, endpoint,
                              reinterpret_cast<unsigned char*>(buffer.data()),
                              static_cast<int>(len), nullptr, 0) == 0;
}

bool UniversalDevice::read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len) {
  if (!m_device_data.HandlesOpen) {
    return false;
  }

  int actual_len = 0;
  const int result = libusb_bulk_transfer(m_device_data.Handle, endpoint,
                                          reinterpret_cast<unsigned char*>(buffer.data()),
                                          static_cast<int>(len), &actual_len, 0);
  len = static_cast<std::size_t>(actual_len);
  return result == 0;
}

} // namespace device

} // namespace usbtingo