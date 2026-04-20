#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <memory>
#include <net/if.h>
#include <poll.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "usbtingo/bus/Bus.hpp"
#include "usbtingo/bus/CanListener.hpp"
#include "usbtingo/can/Dlc.hpp"
#include "usbtingo/device/Device.hpp"
#include "usbtingo/device/DeviceFactory.hpp"

static std::atomic<bool> g_shutdown{ false };

static void signal_handler(int /*signum*/) {
  g_shutdown.store(true);
}

/**
 * @brief Configuration for the SocketCAN bridge.
 */
struct BridgeConfig {
  std::string interface               = "vcan0";
  std::uint32_t serial                = 0;
  std::uint32_t baudrate              = 500000;
  std::uint32_t data_baud             = 0;
  usbtingo::device::Protocol protocol = usbtingo::device::Protocol::CAN_FD;
};

/**
 * @brief Print usage information.
 */
static void print_usage(const char* program_name) {
  std::cout << "Usage: " << program_name << " [options]" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -i <interface>      SocketCAN interface name (default: vcan0)" << std::endl;
  std::cout << "  -s <serial>         USBtingo serial number (auto-detect if omitted)" << std::endl;
  std::cout << "  -b <baudrate>       CAN baudrate in bit/s (default: 500000)" << std::endl;
  std::cout << "  -d <data_baudrate>  CAN FD data baudrate in bit/s (default: same as -b)" << std::endl;
  std::cout << "  -p <protocol>       Protocol: can20, canfd, canfd-noniso (default: canfd)" << std::endl;
  std::cout << "  -h                  Print this help message" << std::endl;
}

/**
 * @brief Parse command line arguments into a BridgeConfig.
 * @param[in] argc Argument count.
 * @param[in] argv Argument values.
 * @param[out] config Configuration to populate.
 * @return Returns true if parsing succeeds.
 */
static bool parse_args(int argc, char* argv[], BridgeConfig& config) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-h") {
      print_usage(argv[0]);
      return false;
    }

    if (i + 1 >= argc) {
      std::cerr << "Missing value for " << arg << std::endl;
      return false;
    }

    std::string val = argv[++i];

    if (arg == "-i") {
      config.interface = val;
    } else if (arg == "-s") {
      config.serial = static_cast<std::uint32_t>(std::stoul(val, nullptr, 0));
    } else if (arg == "-b") {
      config.baudrate = static_cast<std::uint32_t>(std::stoul(val));
    } else if (arg == "-d") {
      config.data_baud = static_cast<std::uint32_t>(std::stoul(val));
    } else if (arg == "-p") {
      if (val == "can20")
        config.protocol = usbtingo::device::Protocol::CAN_2_0;
      else if (val == "canfd")
        config.protocol = usbtingo::device::Protocol::CAN_FD;
      else if (val == "canfd-noniso")
        config.protocol = usbtingo::device::Protocol::CAN_FD_NON_ISO;
      else {
        std::cerr << "Unknown protocol: " << val << std::endl;
        return false;
      }
    } else {
      std::cerr << "Unknown option: " << arg << std::endl;
      print_usage(argv[0]);
      return false;
    }
  }

  if (config.data_baud == 0)
    config.data_baud = config.baudrate;

  return true;
}

/**
 * @brief Open a SocketCAN raw socket and bind it to the given interface.
 * @param[in] interface_name Name of the SocketCAN interface (e.g. "vcan0").
 * @param[in] enable_fd Enable CAN FD frame support on the socket.
 * @return File descriptor on success, -1 on failure.
 */
static int open_socketcan(const std::string& interface_name, bool enable_fd) {
  int sock_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sock_fd < 0) {
    std::cerr << "Failed to create CAN socket: " << std::strerror(errno) << std::endl;
    return -1;
  }

  struct ifreq ifr = {};
  std::strncpy(ifr.ifr_name, interface_name.c_str(), IFNAMSIZ - 1);
  if (ioctl(sock_fd, SIOCGIFINDEX, &ifr) < 0) {
    std::cerr << "Failed to find interface " << interface_name << ": " << std::strerror(errno) << std::endl;
    close(sock_fd);
    return -1;
  }

  if (enable_fd) {
    int enable = 1;
    if (setsockopt(sock_fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable, sizeof(enable)) < 0) {
      std::cerr << "Failed to enable CAN FD on socket: " << std::strerror(errno) << std::endl;
      close(sock_fd);
      return -1;
    }
  }

  struct sockaddr_can addr = {};
  addr.can_family          = AF_CAN;
  addr.can_ifindex         = ifr.ifr_ifindex;

  if (bind(sock_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    std::cerr << "Failed to bind CAN socket: " << std::strerror(errno) << std::endl;
    close(sock_fd);
    return -1;
  }

  return sock_fd;
}

/**
 * @brief Convert a CanRxFrame from the USBtingo to a SocketCAN canfd_frame and write it to the socket.
 * @param[in] rx_frame Received CAN frame from the USBtingo.
 * @param[in] sock_fd SocketCAN file descriptor.
 * @return Returns true if the write succeeds.
 */
static bool write_socketcan_frame(const usbtingo::device::CanRxFrame& rx_frame, int sock_fd) {
  struct canfd_frame frame = {};

  frame.can_id = rx_frame.id;
  if (rx_frame.xtd)
    frame.can_id |= CAN_EFF_FLAG;
  if (rx_frame.rtr)
    frame.can_id |= CAN_RTR_FLAG;

  frame.len = static_cast<__u8>(usbtingo::can::Dlc::dlc_to_bytes(rx_frame.dlc));

  frame.flags = 0;
  if (rx_frame.brs)
    frame.flags |= CANFD_BRS;
  if (rx_frame.esi)
    frame.flags |= CANFD_ESI;

  std::memcpy(frame.data, rx_frame.data.data(), frame.len);

  std::size_t write_size = (rx_frame.fdf) ? sizeof(struct canfd_frame) : sizeof(struct can_frame);
  ssize_t nbytes         = write(sock_fd, &frame, write_size);

  return nbytes == static_cast<ssize_t>(write_size);
}

/**
 * @brief Convert a SocketCAN canfd_frame to a CanTxFrame for the USBtingo.
 * @param[in] frame SocketCAN frame received from the socket.
 * @param[in] is_fd Whether CAN FD protocol is active.
 * @param[out] tx_frame USBtingo TX frame to populate.
 */
static void socketcan_to_txframe(const struct canfd_frame& frame, bool is_fd, usbtingo::device::CanTxFrame& tx_frame) {
  tx_frame              = {};
  tx_frame.message_type = 1;
  tx_frame.id           = frame.can_id & CAN_EFF_MASK;
  tx_frame.xtd          = (frame.can_id & CAN_EFF_FLAG) ? 1 : 0;
  tx_frame.rtr          = (frame.can_id & CAN_RTR_FLAG) ? 1 : 0;
  tx_frame.fdf          = is_fd ? 1 : 0;
  tx_frame.brs          = (frame.flags & CANFD_BRS) ? 1 : 0;
  tx_frame.esi          = (frame.flags & CANFD_ESI) ? 1 : 0;
  tx_frame.dlc          = usbtingo::can::Dlc::bytes_to_dlc(frame.len);

  std::memcpy(tx_frame.data.data(), frame.data, frame.len);
  tx_frame.message_size = static_cast<std::uint8_t>(usbtingo::device::CanTxFrame::buffer_size_bytes(tx_frame));
}

/**
 * @brief CanListener that forwards received CAN frames to a SocketCAN socket.
 */
class BridgeListener : public usbtingo::bus::CanListener {
public:
  BridgeListener(int sock_fd)
      : m_sock_fd(sock_fd) {}

  void on_can_receive(const usbtingo::device::CanRxFrame& msg) override { write_socketcan_frame(msg, m_sock_fd); }

private:
  int m_sock_fd;
};

/**
 * @brief TX thread function that reads CAN frames from the SocketCAN socket and sends them to the USBtingo.
 * @param[in] sock_fd SocketCAN file descriptor.
 * @param[in] bus Pointer to the Bus instance.
 * @param[in] is_fd Whether CAN FD protocol is active.
 */
static void tx_thread_func(int sock_fd, usbtingo::bus::Bus* bus, bool is_fd) {
  struct canfd_frame frame = {};
  struct pollfd pfd        = {};
  pfd.fd                   = sock_fd;
  pfd.events               = POLLIN;

  while (!g_shutdown.load()) {
    int ret = poll(&pfd, 1, 100);
    if (ret < 0) {
      if (errno == EINTR)
        continue;
      break;
    }
    if (ret == 0)
      continue;

    ssize_t nbytes = read(sock_fd, &frame, sizeof(struct canfd_frame));
    if (nbytes < 0) {
      if (errno == EINTR)
        continue;
      break;
    }

    if (nbytes < static_cast<ssize_t>(sizeof(struct can_frame)))
      continue;

    usbtingo::device::CanTxFrame tx_frame;
    socketcan_to_txframe(frame, is_fd, tx_frame);
    bus->send(tx_frame);
  }
}

/**
 * @brief USBtingo to SocketCAN bridge. Bridges a USBtingo CAN device to a Linux SocketCAN virtual interface, allowing
 * standard CAN tools (candump, cansend, etc.) to communicate through the USBtingo.
 *
 * Usage: USBtingoSocketCANBridge [-i interface] [-s serial] [-b baudrate] [-d data_baudrate] [-p protocol]
 */
int main(int argc, char* argv[]) {
  std::cout << "+======================================+" << std::endl;
  std::cout << " USBtingo SocketCAN Bridge utility      " << std::endl;
  std::cout << "+======================================+" << std::endl;

  // Parse command line arguments
  BridgeConfig config;
  if (!parse_args(argc, argv, config))
    return 1;

  bool is_fd = (config.protocol != usbtingo::device::Protocol::CAN_2_0);

  // Detect and select USBtingo device
  auto serial_vec = usbtingo::device::DeviceFactory::detect_available_devices();
  if (serial_vec.empty()) {
    std::cerr << "No USBtingo devices found." << std::endl;
    return 1;
  }

  std::uint32_t serial = config.serial;
  if (serial == 0) {
    serial = serial_vec.at(0);
    if (serial_vec.size() > 1) {
      std::cout << " Multiple devices found, using first: " << serial << std::endl;
      std::cout << " Use -s <serial> to select a specific device." << std::endl;
    }
  }

  std::cout << " Device:    USBtingo " << serial << " (0x" << std::hex << serial << std::dec << ")" << std::endl;
  std::cout << " Interface: " << config.interface << std::endl;
  std::cout << " Baudrate:  " << config.baudrate << " bit/s" << std::endl;
  if (is_fd)
    std::cout << " Data baud: " << config.data_baud << " bit/s" << std::endl;
  std::cout << " Protocol:  " << (is_fd ? "CAN FD" : "CAN 2.0") << std::endl;
  std::cout << std::endl;

  // Create and configure USBtingo device
  auto device = usbtingo::device::DeviceFactory::create(serial);
  if (!device) {
    std::cerr << "Failed to open USBtingo device." << std::endl;
    return 1;
  }

  if (!device->is_alive()) {
    std::cerr << "USBtingo device is not responding." << std::endl;
    return 1;
  }

  device->set_mode(usbtingo::device::Mode::OFF);
  device->set_protocol(config.protocol);
  device->set_baudrate(config.baudrate, config.data_baud);
  device->set_mode(usbtingo::device::Mode::ACTIVE);

  // Create Bus
  auto bus = std::make_unique<usbtingo::bus::Bus>(std::move(device));

  // Open SocketCAN socket
  int sock_fd = open_socketcan(config.interface, is_fd);
  if (sock_fd < 0)
    return 1;

  // Register RX bridge listener
  BridgeListener listener(sock_fd);
  bus->add_listener(&listener);

  // Start TX bridge thread
  std::thread tx_thread(tx_thread_func, sock_fd, bus.get(), is_fd);

  // Install signal handlers for clean shutdown
  struct sigaction sa = {};
  sa.sa_handler       = signal_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);

  std::cout << "+======================================+" << std::endl;
  std::cout << " Bridge is running." << std::endl;
  std::cout << " Press Ctrl+C to stop." << std::endl;
  std::cout << "+======================================+" << std::endl;

  // Wait for shutdown signal
  while (!g_shutdown.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << std::endl << " Shutting down..." << std::endl;

  // Shutdown: join TX thread (poll timeout lets it exit), then close socket
  if (tx_thread.joinable())
    tx_thread.join();
  close(sock_fd);

  bus->remove_listener(&listener);
  bus->stop();
  bus.reset();

  std::cout << " Bridge stopped." << std::endl;
  return 0;
}
