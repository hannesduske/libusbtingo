#pragma once

#include <Windows.h>
#include <string>
#include <winusb.h>

namespace usbtingo {

namespace device {

struct WinHandle {
  BOOL HandlesOpen                     = 0;
  WINUSB_INTERFACE_HANDLE WinusbHandle = { 0 };
  HANDLE DeviceHandle                  = { 0 };
  std::string DevicePath;
};

} // namespace device

} // namespace usbtingo