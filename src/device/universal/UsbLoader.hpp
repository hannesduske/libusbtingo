#pragma once

#include <libusb-1.0/libusb.h>

#include "SingletonTemplate.hpp"
#include "../DeviceProtocol.hpp"

#include <thread>
#include <atomic>
#include <mutex>

namespace usbtingo{

namespace device{

/**
 * @brief Helper class that initializes the libusb when the library is loaded and cleans up the libusb when the library is unloaded. This helper class is needed because there are static methods in the UniversalDevice which might be called before one instance of the UniversalDevice is created. Initializing the libusb in the UniversalDevice constructor therefore doesn't work.
 */
class UsbLoader : public Singleton<UsbLoader> {
public:
    ~UsbLoader()
    {
        m_handler_running.store(0);
        libusb_interrupt_event_handler(m_ctx);
        if(m_event_handler.joinable())
            m_event_handler.join();
        libusb_exit(m_ctx);
    }

    libusb_context* get_ctx()
    {
        return m_ctx;
    }

private:
    friend class Singleton<UsbLoader>;
    UsbLoader()
    {
        libusb_init_context(&m_ctx, NULL, 0);
        m_event_handler = std::thread(&UsbLoader::event_loop, this);
    }

    void event_loop()
    {
        m_handler_running.store(1);
        while(m_handler_running.load()){
            {
                std::lock_guard<std::mutex> lock(m_mtx);
                libusb_handle_events(m_ctx);
            }
            std::this_thread::sleep_for(USBTINGO_THREAD_DELAY);
        }
        m_handler_running.store(-1);
        return;
    }

private:
    libusb_context* m_ctx;

    std::mutex m_mtx;
    std::thread m_event_handler;
    std::atomic<int> m_handler_running;
};

inline libusb_context* get_usb_ctx() {
    return UsbLoader::getInstance()->get_ctx();
}

}

}