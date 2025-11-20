#pragma once

#include <libusb-1.0/libusb.h>
#include <optional>

namespace usbtingo {

namespace device {

struct UniversalHandle {
  bool HandlesOpen             = 0;
  libusb_device_handle* Handle = nullptr;
  libusb_device* Device        = nullptr;
};

} // namespace device

} // namespace usbtingo