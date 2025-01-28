#pragma once

#include <libusb-1.0/libusb.h>
#include "../DeviceProtocol.hpp"
#include <thread>
#include <atomic>
#include <mutex>

namespace usbtingo{

namespace device{

/**
 * @brief Helper class that initializes the libusb when the library is loaded and cleans up the libusb when the library is unloaded. This helper class is needed because there are static methods in the UniversalDevice which might be called before one instance of the UniversalDevice is created. Initializing the libusb in the UniversalDevice constructor therefore doesn't work.
 */
class UsbLoader {
public:
    UsbLoader() {
        libusb_init_context(NULL, NULL, 0);

        m_event_handler = std::thread([this](){
            m_handler_running.store(1);
            while(m_handler_running.load()){
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    libusb_handle_events(NULL);
                }
                std::this_thread::sleep_for(USBTINGO_THREAD_DELAY);
            }
            m_handler_running.store(-1);
            return;
        });
    }

    ~UsbLoader() {
        m_handler_running.store(0);
        libusb_interrupt_event_handler(NULL);
        while(m_handler_running.load() >= 0){
            std::this_thread::yield();
        }
        if(m_event_handler.joinable())
            m_event_handler.join();
        libusb_exit(NULL);
    }

private:
    std::mutex mtx;
    std::thread m_event_handler;
    std::atomic<int> m_handler_running;
};

static UsbLoader loader;

}

}