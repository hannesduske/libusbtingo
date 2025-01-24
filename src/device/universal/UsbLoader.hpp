#pragma once

#include <libusb-1.0/libusb.h>

namespace usbtingo{

namespace device{

class UsbLoader {
public:
    UsbLoader() {
        libusb_init_context(NULL, NULL, 0);
    }

    ~UsbLoader() {
        libusb_exit(NULL);
    }

};

// Static instance ensures the constructor and destructor are called
static UsbLoader loader;

}

}