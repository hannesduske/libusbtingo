#pragma once

#include <libusb-1.0/libusb.h>


namespace usbtingo {

namespace device {

struct UniversalHandle {
    bool                    HandlesOpen = 0;
    libusb_device_handle*   UniversalHandle = nullptr;
    libusb_device*          Device = nullptr;
};

}

}