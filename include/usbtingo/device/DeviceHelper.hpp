#pragma once

#include <cstdint>
#include <array>

#include "usbtingo/platform/UsbtingoExport.hpp"

namespace usbtingo {

namespace device {

/**
 * @brief Operation modes of the USBtingo.
 */
enum class USBTINGO_API Mode {
    OFF = 0x00,
    ACTIVE = 0x01,
    LISTEN_ONLY = 0x02
};

/**
 * @brief Supported Bus protocols of the USBtingo.
 */
enum class USBTINGO_API Protocol {
    CAN_2_0 = 0x00,
    CAN_FD = 0x01,
    CAN_FD_NON_ISO = 0x02
};

/**
 * @brief Buffer for USB transfers.
 * ToDo: Move device implementation to impl class, that this type is hidden and the magic 512 can be replaced by USB_BULK_BUFFER_SIZE
 */
typedef std::array<std::uint8_t, 512> BulkBuffer;

/**
 * @brief Object representing the DeviceInfo stored on the USBtingo.
 */
struct USBTINGO_API DeviceInfo {
	std::uint8_t    fw_minor = 0;
	std::uint8_t    fw_major = 0;
	std::uint8_t    hw_model = 0;
	std::uint8_t    channels = 0;
	std::uint32_t   uniqe_id = 0;
	std::uint32_t   clock_hz = 0;
};

/**
 * @brief Status frame of the USBtingo.
 */
struct USBTINGO_API StatusFrame {
	std::uint8_t	message_type = 0;
	std::uint8_t	operation_mode = 0;
	std::uint8_t	txeovf = 0;
	std::uint8_t	rxovf = 0;
	std::uint32_t	procts = 0;
	std::uint8_t	tec = 0;
	std::uint8_t	rec = 0;
	std::uint8_t	rp = 0;
	std::uint8_t	cel = 0;
	std::uint8_t	bo = 0;
	std::uint8_t	ew = 0;
	std::uint8_t	ep = 0;
	std::uint8_t	act = 0;
	std::uint8_t	lec = 0;
	std::uint8_t	dlec = 0;
	std::uint8_t	tdvc = 0;
	std::uint32_t	nr_std_frames = 0;
	std::uint32_t	nr_ext_frames = 0;
	std::uint32_t	nr_bytes_wo_brs = 0;
	std::uint32_t	nr_bytes_with_brs = 0;

	Mode get_operation_mode() const;
	std::uint8_t get_tx_error_count() const;
	std::uint8_t get_rx_error_count() const;
	bool is_receive_error_passive() const;
	bool is_error_passive() const;
	bool is_warning_status() const;
	bool is_bus_off() const;

	static bool deserialize_status(const uint8_t* buf, StatusFrame& frame);
};

/**
 * @brief CanRxFrame frame of the USBtingo.
 */
struct USBTINGO_API CanRxFrame {
	std::uint8_t	message_type = 0;
	std::uint8_t	message_size = 0;
	std::uint32_t	procts = 0;
	std::uint32_t	id = 0;
	std::uint8_t	esi = 0;
	std::uint8_t	xtd = 0;
	std::uint8_t	rtr = 0;
	std::uint16_t	rxts = 0;
	std::uint8_t	fdf = 0;
	std::uint8_t	brs = 0;
	std::uint8_t	dlc = 0;
	std::uint8_t	anmf = 0;
	std::uint8_t	fidx = 0;
	std::array<std::uint8_t, 64> data = { 0 };

	/**
	 * @brief Convert a raw buffer from a USBtingo transmission to a CanRxFrame.
	 * @param[in] buf Pointer to the raw buffer that was received from the USBtingo.
	 * @param[out] buf_out Reference to the CanRxFrame to which the raw buffer values are written.
	 * @return Returns true if operation succeeds. Returns false if the data in the input buffer has an invalid format.
	 */
	static bool deserialize_can_frame(const uint8_t* buf, CanRxFrame& buf_out);
};


/**
 * @brief CanTxFrame frame of the USBtingo.
 */
struct USBTINGO_API CanTxFrame {
	std::uint8_t	message_type = 1;
	std::uint8_t	message_size = 0;
	std::uint32_t	id = 0;
	std::uint8_t	esi = 0;
	std::uint8_t	xtd = 0;
	std::uint8_t	rtr = 0;
	std::uint8_t	efc = 0;
	std::uint8_t	fdf = 0;
	std::uint8_t	brs = 0;
	std::uint8_t	dlc = 0;
	std::uint8_t	txmm = 0;
	std::array<std::uint8_t, 64> data = { 0 };
	
	/**
	 * @brief Convert a raw buffer from a USBtingo transmission to a CanTxFrame.
	 * @param[out] buf_out Pointer to the raw buffer to which the raw buffer values are written.
	 * @param[in] buf Reference to the CanRxFrame which will be convereted to a raw buffer.
	 * @return Returns true if operation succeeds. Returns false if the data in the input buffer has an invalid format.
	 */
	static bool serialize_can_frame(uint8_t* buf_out, const CanTxFrame& frame);

	/**
	 * @brief Calculate the buffer size of the CanTxFrame that is required for the CanTxFrame.message_size field.
	 * @param[in] buf Reference to the CanRxFrame whose size is to be determined. 
	 * @return Size of a raw buffer containing the CanRxFrame data in bytes aligned to a 32-bit value.
	 */
	static std::size_t buffer_size_bytes(const CanTxFrame& buf);
};


/**
 * @brief TxEventFrame frame of the USBtingo.
 */
struct USBTINGO_API TxEventFrame {
	std::uint8_t	message_type = 0;
	std::uint8_t	message_size = 0;
	std::uint32_t	procts = 0;
	std::uint32_t	id = 0;
	std::uint8_t	esi = 0;
	std::uint8_t	xtd = 0;
	std::uint8_t	rtr = 0;
	std::uint16_t	txts = 0;
	std::uint8_t	et = 0;
	std::uint8_t	fdf = 0;
	std::uint8_t	brs = 0;
	std::uint8_t	dlc = 0;
	std::uint8_t	txmm = 0;

	/**
	 * @brief Convert a raw buffer from a USBtingo transmission to a TxEventFrame.
	 * @param[in] buf Pointer to the raw buffer that was received from the USBtingo.
	 * @param[out] buf_out Reference to the TxEventFrame to which the raw buffer values are written.
	 * @return Returns true if operation succeeds. Returns false if the data in the input buffer has an invalid format.
	 */
	static bool deserialize_tx_event(const uint8_t* buf, TxEventFrame& frame);
};

}

}