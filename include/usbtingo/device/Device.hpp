#pragma once

#include "usbtingo/device/DeviceHelper.hpp"
#include "usbtingo/platform/UsbtingoExport.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <future>
#include <thread>
#include <atomic>
#include <functional>

namespace usbtingo{

namespace device{

// Forward declaration
// ToDo: Hide from interface
enum class AsyncIoState;

class USBTINGO_API Device{
public:
    /**
     * @brief Default destructor
     */
    virtual ~Device(); // required that the destructor of derived classes is called

    /**
     * @brief Get the serial number of the device
     * @return Serial number of the device
     */
	std::uint32_t get_serial() const;

    /**
     * @brief Get the DeviceInfo data of the device. The info is automatically fetched in the constructor of the Device.
     * @return DeviceInfo of the device
     */
    DeviceInfo get_device_info() const;

    /**
     * @brief Open the device handle. This method is automatically called in the constructor of the device.
     * @return Returns true if operation succeeds. Returns false if device handle is already opened.
     */
    virtual bool open() = 0;
    
    /**
     * @brief Close the device handle. This method is automatically called in the destructor of the device.
     * @return Returns true if operation succeeds. Returns false if device handle is already closed.
     */
    virtual bool close() = 0;

    /**
     * @brief Check if device handle is currently opened.
     * @return Current state of the device handle.
     */
    virtual bool is_open() const = 0;
    
    /**
     * @brief Check if the device is alive. The method fetches the device info and compares the serial number stored in the memory of the device with the serial number of the software object.
     * @return Returns true if the device is correctly instantiated and communication with the device works.
     */
    bool is_alive();

    /**
     * @brief Set the protocol of the device. Device must be in Mode::OFF to set this value.
     * @param[in] protocol Protocol of the device
     * @param[in] flags Optional flags. Refer to the USBtingo Protocol description for details.
     * @return Returns true if setting the protocol succeeded
     */
	bool set_protocol(Protocol protocol, std::uint8_t flags = 0);

    /**
     * @brief Set the baudrate of the device without bit rate switching. Device must be in Mode::OFF to set this value.
     * @param[in] baudrate Baudrate of the device
     * @return Returns true if setting the baudrate succeeded
     */
    bool set_baudrate(std::uint32_t baudrate);

    /**
     * @brief Set the baudrate of the device with bit rate switching. Device must be in Mode::OFF to set this value.
     * @param[in] baudrate Baudrate of the device
     * @param[in] baudrate_data Data baudrate of the device
     * @return Returns true if setting the baudrate succeeded
     */
    bool set_baudrate(std::uint32_t baudrate, std::uint32_t baudrate_data);

    /**
     * @brief Set the mode of the device.
     * @param[in] mode Mode of the device
     * @return Returns true if setting the mode succeeded
     */
    bool set_mode(Mode mode);

    /**
     * @brief Clear the error counter overflow flags.
     * @return Returns true if clearing the error flags succeeded
     */
    bool clear_errors();

    /**
     * @brief Disable all filters that are currently set on the device.
     * @return Returns true if clearing the filters succeeded
     */
    //virtual bool disable_all_filters() = 0;

    /**
     * @brief Add a standard id to the Can filters that are currently set on the device.
     * @param[in] filterid Identifier of the filter
     * @param[in] enabled Enable state of the filter
     * @param[in] filter Standard id of the filter
     * @param[in] mask Mask bits of the filter
     * @return Returns true if adding the filters succeeded
     */
    //virtual bool add_std_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) = 0;

    /**
     * @brief Add a extended id to the Can filters that are currently set on the device.
     * @param[in] filterid Identifier of the filter
     * @param[in] enabled Enable state of the filter
     * @param[in] filter Standard id of the filter
     * @param[in] mask Mask bits of the filter
     * @return Returns true if adding the filters succeeded
     */
    //virtual bool add_ext_filter(std::uint8_t filterid, std::uint32_t enabled, std::uint32_t filter, std::uint32_t mask) = 0;

    /**
     * @brief Read a mcan register
     * @return Returns true if operation succeeded
     */
    //bool read_mcan();

    /**
     * @brief Write mcan register
     * @return Returns true if operation succeeded
     */
    //bool write_mcan();

    /**
     * @brief Configure the logic function of the device
     * @return Returns true if operation succeeded
     */
    //bool config_logic();

    /**
     * @brief Get the logic error flags from the device
     * @brief Returns true if operation succeeded
     */
    //bool get_logic_errors();

    /**
     * @brief Fetch the status from the device. The method fetches a new StatusFrame synchronously and does not return the last StatusFrame that was received asynchronously.
     * @param[out] status Current status of the device
     * @return Returns true if operation succeeds
     */
    virtual bool read_status(StatusFrame& status);

	/**
	 * @brief Send a message on the Can bus.
	 * @param[in] tx_frame Message to be sent on the Can bus. The CanTxFrame has to be configured manually before passing it to the send method.
	 * @return Returns true if operation succeeds.
	 */
    virtual bool send_can(const CanTxFrame& tx_frame);

	/**
	 * @brief Send multiple messages on the Can bus. The method used the devices function to receive multiple tx frames in one usb transfer.
	 * @param[in] tx_frames Vector of messages to be sent on the Can bus. The CanTxFrames have to be configured manually before passing it to the send method.
	 * @return Returns true if operation succeeds.
	 */
    virtual bool send_can(const std::vector<CanTxFrame>& tx_frames);

    /**
     * @brief Synchronously read a rx buffer from the device and return the buffer content as CanRxFrames and TxEventFrame. The method blocks until a rx_buffer is available.
     * @param[out] rx_frames Vector of rx frames received from the device.
     * @param[out] rx_frames Vector of tx event frames received from the device.
     * @return Return true if reading and processing the rx buffer succeeded.
     */
    virtual bool receive_can(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames);

    /**
     * @brief Cancel all currently active asynchronous can rx requests.
     * @return Returns true if active requests are cancelled. Returns false if there are no active requests.
     */
    virtual bool cancel_async_can_request() = 0;

    /**
     * @brief Asynchronously requests a can rx transmission. The method call is non-blocking.
     * @return Returns a future of the request. Returns an empty (invalid) future in case the request does not succeed.
     */
    virtual std::future<bool> request_can_async() = 0;

    /**
     * @brief Fetch the rx buffer from the device after the future of the request is completed.
     * @param[out] rx_frames Vector of rx frames received from the device.
     * @param[out] rx_frames Vector of tx event frames received from the device.
     * @return Return true if reading and processing the rx buffer succeeded.
     */
    virtual bool receive_can_async(std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames) = 0;

    /**
     * @brief Cancel all currently active asynchronous status requests.
     * @return Returns true if active requests are cancelled. Returns false if there are no active requests.
     */
    virtual bool cancel_async_status_request() = 0;

    /**
     * @brief Asynchronously requests a status transmission. The method call is non-blocking.
     * @return Returns a future of the request. Returns an empty (invalid) future in case the request does not succeed.
     */
    virtual std::future<bool> request_status_async() = 0;

    /**
     * @brief Fetch the status buffer from the device after the future of the request is completed.
     * @param[out] status_frame Status frame that has been received from the device.
     * @return Return true if reading and processing the status buffer succeeded.
     */
    virtual bool receive_status_async(StatusFrame& status_frame) = 0;

    /**
     * @brief Cancel all currently active asynchronous logic requests.
     * @return Returns true if active requests are cancelled. Returns false if there are no active requests.
     */
    //virtual bool cancel_async_logic_request() = 0;

    /**
     * @brief Asynchronously requests a logic transmission. The method call is non-blocking.
     * @return Returns a future of the request. Returns an empty (invalid) future in case the request does not succeed.
     */
    //virtual std::future<bool> request_logic_async() = 0;

    /**
     * @brief Fetch the logic buffer from the device after the future of the request is completed.
     * @param[out] logic_frame Logic frame that has been received from the device.
     * @return Return true if reading and processing the logic buffer succeeded.
     */
    //virtual bool receive_logic_async(LogicFrame& logic_frame) = 0;

protected:
    /**
     * @brief Private constructor. Use the DeviceFactory to instantiate reals devices.
     */
	Device(std::uint32_t serial);

    // ToDo: Move to RxBuffer implementation as soon as a universal buffer for all io transmissions is implemented.
    /**
     * @brief Process a rx buffer from the device.
     * @param[in] rx_buffer Pointer to the rx buffer that will be be processed.
     * @param[in] rx_len Length of the rx buffer.
     * @param[out] rx_frames Vector of the CanRxFrames that are processed from the buffer.
     * @param[out] tx_event_frames Vector of the TxEventFrame that are processed from the buffer.
     * @return Returns true if processing the buffer succeeded
     */
    bool process_can_buffer(const std::uint8_t* rx_buffer, std::size_t rx_len, std::vector<CanRxFrame>& rx_frames, std::vector<TxEventFrame>& tx_event_frames);

    virtual bool read_usbtingo_serial(std::uint32_t& serial);

    virtual bool write_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len);

    virtual bool read_bulk(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t& len);

    //virtual bool request_bulk_async(std::uint8_t endpoint, BulkBuffer& buffer, std::size_t len);

    //virtual bool read_bulk_async(std::size_t& len);

    virtual bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx);

    virtual bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data);

    virtual bool write_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::uint8_t* data, std::uint16_t len);

    virtual bool read_control(std::uint8_t cmd, std::uint16_t val, std::uint16_t idx, std::vector<std::uint8_t>& data, std::uint16_t len);


protected:
	std::uint32_t m_serial;

	DeviceInfo m_device_info;

    std::thread m_thread_status;
    std::thread m_thread_logic;
    std::thread m_thread_can;

    std::atomic<AsyncIoState> m_shutdown_status;
    std::atomic<AsyncIoState> m_shutdown_logic;
    std::atomic<AsyncIoState> m_shutdown_can;

    BulkBuffer m_buffer_status; // Can be smaller, 64 byte?
    BulkBuffer m_buffer_logic;
    BulkBuffer m_buffer_can;
};

}

}