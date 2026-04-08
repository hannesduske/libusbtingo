#pragma once

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "../DeviceProtocol.hpp"
#include "usbtingo/device/Device.hpp"

#include "UniversalHandle.hpp"

namespace usbtingo {

namespace device {

class UniversalDevice : public Device {
public:
  UniversalDevice(std::uint32_t serial, libusb_device* dev);

  ~UniversalDevice() override;

  static std::unique_ptr<Device> create_device(std::uint32_t serial);

  static std::vector<std::uint32_t> detect_available_devices();

  bool open() override;

  bool close() override;

  bool is_open() const override;

  bool cancel_async_can_request() override;

  bool cancel_async_logic_request() override;

  bool cancel_async_status_request() override;

  std::future<bool> request_can_async() override;

  std::future<bool> request_logic_async() override;

  std::future<bool> request_status_async() override;

  bool receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) override;

  bool receive_logic_async(LogicFrame& logic_frame) override;

  bool receive_status_async(StatusFrame& status_frame) override;

private:
  bool allocate_transfers();
  void free_transfers();
  void setup_transfer(libusb_transfer* transfer, std::uint8_t endpoint, unsigned char* buffer, int length, libusb_transfer_type type);

  UniversalHandle m_device_data;

  libusb_transfer* m_async_status = nullptr;
  libusb_transfer* m_async_logic  = nullptr;
  libusb_transfer* m_async_can    = nullptr;

  // Mutex to protect promise access and prevent race conditions
  std::mutex m_promise_mutex;
  std::promise<bool> m_promise_status;
  std::promise<bool> m_promise_logic;
  std::promise<bool> m_promise_can;

  void handle_can_async_callback(libusb_transfer* transfer);
  void handle_logic_async_callback(libusb_transfer* transfer);
  void handle_status_async_callback(libusb_transfer* transfer);

  static std::map<std::uint32_t, libusb_device*> detect_usbtingos();

  /**
   * @brief Parse a serial number string to uint32_t with exception handling.
   * @param buffer Buffer containing the serial number string
   * @param serial Output serial number
   * @return true if parsing succeeded, false otherwise
   */
  static bool parse_serial_number(const unsigned char* buffer, std::uint32_t& serial);

  bool read_usbtingo_serial(std::uint32_t& serial) override;

  bool write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len) override;
  bool read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len) override;

  bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx) override;
  bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data) override;
  bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len) override;

  bool read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len) override;
};

} // namespace device

} // namespace usbtingo