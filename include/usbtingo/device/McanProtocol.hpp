#pragma once

#include <cstdint>

namespace usbtingo {

namespace device {

namespace mcan {

    //===============================================
    // Register Map
    //===============================================

    // Core Release and Identification
    constexpr std::uint16_t MCAN_REG_CREL    = 0x000; // Core Release Register
    constexpr std::uint16_t MCAN_REG_ENDN    = 0x004; // Endian Register
    constexpr std::uint16_t MCAN_REG_CUST    = 0x008; // Customer Register

    // Bit Timing & Prescaler
    constexpr std::uint16_t MCAN_REG_DBTP    = 0x00C; // Data Bit Timing & Prescaler Register
    constexpr std::uint16_t MCAN_REG_TEST    = 0x010; // Test Register
    constexpr std::uint16_t MCAN_REG_RWD     = 0x014; // RAM Watchdog
    constexpr std::uint16_t MCAN_REG_CCCR    = 0x018; // CC Control Register
    constexpr std::uint16_t MCAN_REG_NBTP    = 0x01C; // Nominal Bit Timing & Prescaler Register

    // Timestamp / Timeout
    constexpr std::uint16_t MCAN_REG_TSCC    = 0x020; // Timestamp Counter Configuration
    constexpr std::uint16_t MCAN_REG_TSCV    = 0x024; // Timestamp Counter Value
    constexpr std::uint16_t MCAN_REG_TOCC    = 0x028; // Timeout Counter Configuration
    constexpr std::uint16_t MCAN_REG_TOCV    = 0x02C; // Timeout Counter Value

    // Error / Status
    constexpr std::uint16_t MCAN_REG_ECR     = 0x040; // Error Counter Register
    constexpr std::uint16_t MCAN_REG_PSR     = 0x044; // Protocol Status Register
    constexpr std::uint16_t MCAN_REG_TDCR    = 0x048; // Transmitter Delay Compensation Register

    // Interrupts
    constexpr std::uint16_t MCAN_REG_IR      = 0x050; // Interrupt Register
    constexpr std::uint16_t MCAN_REG_IE      = 0x054; // Interrupt Enable
    constexpr std::uint16_t MCAN_REG_ILS     = 0x058; // Interrupt Line Select
    constexpr std::uint16_t MCAN_REG_ILE     = 0x05C; // Interrupt Line Enable

    // Global / Filters
    constexpr std::uint16_t MCAN_REG_GFC     = 0x080; // Global Filter Configuration
    constexpr std::uint16_t MCAN_REG_SIDFC   = 0x084; // Standard ID Filter Configuration
    constexpr std::uint16_t MCAN_REG_XIDFC   = 0x088; // Extended ID Filter Configuration
    constexpr std::uint16_t MCAN_REG_XIDAM   = 0x090; // Extended ID AND Mask
    constexpr std::uint16_t MCAN_REG_HPMS    = 0x094; // High Priority Message Status

    // New Data / RX FIFO
    constexpr std::uint16_t MCAN_REG_NDAT1   = 0x098; // New Data 1
    constexpr std::uint16_t MCAN_REG_NDAT2   = 0x09C; // New Data 2
    constexpr std::uint16_t MCAN_REG_RXF0C   = 0x0A0; // Rx FIFO 0 Configuration
    constexpr std::uint16_t MCAN_REG_RXF0S   = 0x0A4; // Rx FIFO 0 Status
    constexpr std::uint16_t MCAN_REG_RXF0A   = 0x0A8; // Rx FIFO 0 Acknowledge

    // Rx FIFO 1
    constexpr std::uint16_t MCAN_REG_RXF1C   = 0x0AC; // Rx FIFO 1 Configuration
    constexpr std::uint16_t MCAN_REG_RXF1S   = 0x0B0; // Rx FIFO 1 Status
    constexpr std::uint16_t MCAN_REG_RXF1A   = 0x0B4; // Rx FIFO 1 Acknowledge

    // Rx Buffer / Event FIFO
    constexpr std::uint16_t MCAN_REG_RXESC   = 0x0B8; // Rx Buffer / FIFO Element Size Configuration
    constexpr std::uint16_t MCAN_REG_TXBC    = 0x0C0; // Tx Buffer Configuration
    constexpr std::uint16_t MCAN_REG_TXFQS   = 0x0C4; // Tx FIFO/Queue Status
    constexpr std::uint16_t MCAN_REG_TXBRP   = 0x0C8; // Tx Buffer Request Pending
    constexpr std::uint16_t MCAN_REG_TXBAR   = 0x0CC; // Tx Buffer Add Request
    constexpr std::uint16_t MCAN_REG_TXBCR   = 0x0D0; // Tx Buffer Cancellation Request
    constexpr std::uint16_t MCAN_REG_TXBTO   = 0x0D4; // Tx Buffer Transmission Occurred
    constexpr std::uint16_t MCAN_REG_TXBCF   = 0x0D8; // Tx Buffer Cancellation Finished
    constexpr std::uint16_t MCAN_REG_TXBTIE  = 0x0DC; // Tx Buffer Transmission Interrupt Enable
    constexpr std::uint16_t MCAN_REG_TXBCIE  = 0x0E0; // Tx Buffer Cancellation Finished Interrupt Enable
    constexpr std::uint16_t MCAN_REG_TXEFC   = 0x0F0; // Tx Event FIFO Configuration
    constexpr std::uint16_t MCAN_REG_TXEFS   = 0x0F4; // Tx Event FIFO Status
    constexpr std::uint16_t MCAN_REG_TXEFA   = 0x0F8; // Tx Event FIFO Acknowledge


    //===============================================
    // Control Register (CCCR)
    //===============================================
    constexpr std::uint8_t MCAN_REG_CCCR_NISO   = 15; // Non ISO Operation
    constexpr std::uint8_t MCAN_REG_CCCR_TXP    = 14; // Transmit Pause
    constexpr std::uint8_t MCAN_REG_CCCR_EFBI   = 13; // Edge Filtering during Bus Integration
    constexpr std::uint8_t MCAN_REG_CCCR_PXHD   = 12; // Protocol Exception Handling Disable
    constexpr std::uint8_t MCAN_REG_CCCR_WMM    = 11; // Wide Message Marker
    constexpr std::uint8_t MCAN_REG_CCCR_UTSU   = 10; // Use Timestamping Unit
    constexpr std::uint8_t MCAN_REG_CCCR_BRSE   =  9; // Bit Rate Switch Enable
    constexpr std::uint8_t MCAN_REG_CCCR_FDOE   =  8; // FD Operation Enable
    constexpr std::uint8_t MCAN_REG_CCCR_TEST   =  7; // Test Mode Enable
    constexpr std::uint8_t MCAN_REG_CCCR_DAR    =  6; // Disable Automatic Retransmission
    constexpr std::uint8_t MCAN_REG_CCCR_MON    =  5; // Bus Monitoring Mode
    constexpr std::uint8_t MCAN_REG_CCCR_CSR    =  4; // Clock Stop Request
    constexpr std::uint8_t MCAN_REG_CCCR_CSA    =  3; // Clock Stop Acknowledge
    constexpr std::uint8_t MCAN_REG_CCCR_ASM    =  2; // Restricted Operation Mode
    constexpr std::uint8_t MCAN_REG_CCCR_CCE    =  1; // Configuration Change Enable
    constexpr std::uint8_t MCAN_REG_CCCR_INIT   =  0; // Initialization

    
    //===============================================
    // Data Bit Timing & Prescaler Register (DBTP)
    //===============================================
    constexpr std::uint8_t MCAN_REG_DBTP_TDC    = 23; // Transmitter Delay Compensation
    constexpr std::uint8_t MCAN_REG_DBTP_DBRP   = 16; // Data Bit Rate Prescaler
    constexpr std::uint8_t MCAN_REG_DBTP_DTSEG1 =  8; // Data time segment before sample point
    constexpr std::uint8_t MCAN_REG_DBTP_DTSEG2 =  4; // Data time segment after sample point
    constexpr std::uint8_t MCAN_REG_DBTP_DSJW   =  0; // Data (Re)Synchronization Jump Width

    //===============================================
    // Transmitter Delay Compensation Register (TDCR)
    //===============================================
    constexpr std::uint8_t MCAN_REG_TDCR_TDCO   = 8; // Transmitter Delay Compensation SSP Offset
    constexpr std::uint8_t MCAN_REG_TDCR_TDCF   = 0; // Transmitter Delay Compensation Filter Window Length
}

}

}
