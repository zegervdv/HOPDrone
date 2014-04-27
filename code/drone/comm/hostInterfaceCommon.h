///////////////////////////////////////
//
// hostInterfaceCommon.h
//
// Definitions for the interface between a host computer and the embedded RCM.
//
// Copyright (c) 2010-2 Time Domain
//

#ifndef __rcmHostInterfaceCommon_h
#define __rcmHostInterfaceCommon_h

#include <stdint.h>

// Internal modes of operation
#define RCM_OPMODE_RCM		0


// P400 sleep modes
#define RCM_SLEEP_MODE_ACTIVE          0
#define RCM_SLEEP_MODE_IDLE            1
#define RCM_SLEEP_MODE_STANDBY_ETH     2 // wakeup via ethernet or serial
#define RCM_SLEEP_MODE_STANDBY_SER     3 // wakeup via serial only
#define RCM_SLEEP_MODE_SLEEP           4 // wakeup via GPIO only

///////////////////////////////////////
//
// Message types
//

// REQUEST messages are sent by the host to the embedded applicaion.
// CONFIRM messages are sent by the embedded application to the host in response to REQUEST messages.
// INFO messages are sent automatically by the embedded application to the host when various events occur.
#define RCRM_MSG_TYPE_REQUEST			(0xF000)
#define RCRM_MSG_TYPE_CONFIRM			(0xF100)
#define RCRM_MSG_TYPE_INFO				(0xF200)


///////////////////////////////////////
//
// Host <-> Embedded conversation messages
//

// Get version and temperature info
#define RCM_GET_STATUS_INFO_REQUEST	(RCRM_MSG_TYPE_REQUEST + 1)
#define RCM_GET_STATUS_INFO_CONFIRM	(RCRM_MSG_TYPE_CONFIRM + 1)


// Reboot P410
#define RCM_REBOOT_REQUEST				(RCRM_MSG_TYPE_REQUEST + 2)
#define RCM_REBOOT_CONFIRM				(RCRM_MSG_TYPE_CONFIRM + 2)

// Set opmode
#define RCM_SET_OPMODE_REQUEST			(RCRM_MSG_TYPE_REQUEST + 3)
#define RCM_SET_OPMODE_CONFIRM			(RCRM_MSG_TYPE_CONFIRM + 3)

// Get opmode
#define RCM_GET_OPMODE_REQUEST			(RCRM_MSG_TYPE_REQUEST + 4)
#define RCM_GET_OPMODE_CONFIRM			(RCRM_MSG_TYPE_CONFIRM + 4)

// Set sleep mode
#define RCM_SET_SLEEP_MODE_REQUEST		(RCRM_MSG_TYPE_REQUEST + 5)
#define RCM_SET_SLEEP_MODE_CONFIRM		(RCRM_MSG_TYPE_CONFIRM + 5)

// Get sleep mode
#define RCM_GET_SLEEP_MODE_REQUEST		(RCRM_MSG_TYPE_REQUEST + 6)
#define RCM_GET_SLEEP_MODE_CONFIRM		(RCRM_MSG_TYPE_CONFIRM + 6)

// Execute Built-In Test
#define RCM_BIT_REQUEST	            (RCRM_MSG_TYPE_REQUEST + 8)
#define RCM_BIT_CONFIRM	            (RCRM_MSG_TYPE_CONFIRM + 8)

///////////////////////////////////////
//
// Common INFO messages to the host
//

// Sent when a waveform scan is completed.
// If the complete scan doesn't fit in a single packet, multiple packets are sent which can be combined to form the complete scan.
#define RCM_FULL_SCAN_INFO				(RCRM_MSG_TYPE_INFO + 1)



///////////////////////////////////////
//
// Constants and flags
//


// *_CONFIRM message status codes
#define RCM_CONFIRM_MSG_STATUS_SUCCESS				0
#define RCM_CONFIRM_MSG_STATUS_GENERICFAILURE		1
#define RCM_CONFIRM_MSG_STATUS_WRONGOPMODE			2
#define RCM_CONFIRM_MSG_STATUS_UNSUPPORTEDVALUE	3
#define RCM_CONFIRM_MSG_STATUS_INVALIDDURINGSLEEP	4

// Definitions of the antennaMode field in various messages

// Send RCM_ANTENNAMODE_DEFAULT in a message requiring antennaMode to use the default configured antenna
#define RCM_ANTENNAMODE_DEFAULT		0xff

#define RCM_ANTENNAMODE_TXA_RXA		0
#define RCM_ANTENNAMODE_TXB_RXB		1
#define RCM_ANTENNAMODE_TXA_RXB		2
#define RCM_ANTENNAMODE_TXB_RXA		3

#define RCM_ANTENNAMODE_TOGGLE_FLAG	(0x80)


#define RCRM_MAX_SCAN_SAMPLES			(350)

// TX gain levels (inverses of the actual attenuation levels)
#define RCM_TXGAIN_MIN		0
#define RCM_TXGAIN_MAX		63


// Scan filtering
#define RCM_SCAN_FILTERING_RAW				(1 << 0)
#define RCM_SCAN_FILTERING_FASTTIME		(1 << 1)
#define RCM_SCAN_FILTERING_MOTION			(1 << 2)


///////////////////////////////////////
//
// Message struct definitions
//



typedef struct
{
	// set to RCM_GET_STATUS_INFO_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
} rcmMsg_GetStatusInfoRequest;

typedef struct
{
	// set to RCM_GET_STATUS_INFO_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;

	uint8_t appVersionMajor;
	uint8_t appVersionMinor;
	uint16_t appVersionBuild;

	uint8_t uwbKernelVersionMajor;
	uint8_t uwbKernelVersionMinor;
	uint16_t uwbKernelVersionBuild;

	uint8_t firmwareVersion;
	uint8_t firmwareYear;
	uint8_t firmwareMonth;
	uint8_t firmwareDay;

	uint32_t serialNum;

	uint8_t boardRev;
	uint8_t bitResults;
	uint8_t featureSet;
	uint8_t reserved;

	// Divide this by 4 to get temperature in degrees C.
	int32_t temperature;

	// status code
	uint32_t status;
} rcmMsg_GetStatusInfoConfirm;


typedef struct
{
	// set to RCM_REBOOT_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
} rcmMsg_RebootRequest;

typedef struct
{
	// set to RCM_REBOOT_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
} rcmMsg_RebootConfirm;

typedef struct
{
	// set to RCM_SET_OPMODE_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
	// Requested operational mode of the P400.
	uint32_t opMode;
} rcmMsg_SetOpmodeRequest;

typedef struct
{
	// set to RCM_SET_OPMODE_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
	// Opmode of the radio
	uint32_t opMode;

	uint32_t status;
} rcmMsg_SetOpmodeConfirm;

typedef struct
{
	// set to RCM_GET_OPMODE_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
} rcmMsg_GetOpmodeRequest;

typedef struct
{
	// set to RCM_GET_OPMODE_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
	// Current operational mode of the P400.
	uint32_t opMode;
} rcmMsg_GetOpmodeConfirm;

typedef struct
{
	// set to RCM_SET_SLEEP_MODE_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
	// Requested sleep mode of the P400.
	uint32_t sleepMode;
} rcmMsg_SetSleepModeRequest;

typedef struct
{
	// set to RCM_SET_SLEEP_MODE_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
	uint32_t status;
} rcmMsg_SetSleepModeConfirm;

typedef struct
{
	// set to RCM_GET_SLEEP_MODE_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
} rcmMsg_GetSleepModeRequest;

typedef struct
{
	// set to RCM_GET_SLEEP_MODE_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
	// Current sleep mode of the P400.
	uint32_t sleepMode;
} rcmMsg_GetSleepModeConfirm;

typedef struct
{
	// set to RCM_BIT_REQUEST
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
} rcmMsg_BitRequest;

typedef struct
{
	// set to RCM_BIT_CONFIRM
	uint16_t msgType;
	// identifier to correlate requests with confirms
	uint16_t msgId;
	
    // BIT status - 0 is OK, anything else is an error
	uint32_t status;
} rcmMsg_BitConfirm;

typedef struct
{
	// set to RCM_FULL_SCAN_INFO
	uint16_t msgType;
	// identifier to correlate range requests with info messages
	uint16_t msgId;

	// ID of the transmitting radio
	uint32_t sourceId;

	// Milliseconds since radio boot at the time the scan was completed
	uint32_t timestamp;

	// channel quality (LED8 rise time; lower (faster rise) is better)
	uint16_t channelRiseTime;
	// Max value in leading edge window
	uint16_t vPeak;
	
	uint32_t reserved1;

	// These are indices within the assembled scan.
	int32_t ledIndex;
	int32_t lockspotOffset;
	
	int32_t scanStartPs;
	int32_t scanStopPs;
	
	uint16_t scanStepBins;
	
	// Raw, fast time, motion, etc.
	uint8_t scanFiltering;
	
	uint8_t reserved2;	// alignment

	// Antenna the scan was received on
	uint8_t antennaId;
	
	// The type of operation behind this scan (ranging, BSR, MSR)
	uint8_t operationMode;
	
	// Number of scan samples in this message
	uint16_t numSamplesInMessage;

	// Number of samples in the entire scan
	uint32_t numSamplesTotal;

	// Index of the message in the scan
	uint16_t messageIndex;

	// Total number of RCM_FULL_SCAN_INFO messages to expect for this particular scan.
	uint16_t numMessagesTotal;

	// Scan samples.
	// Note that, unlike RCM_SCAN_INFO, this is NOT a variable-sized packet.
	int32_t scan[RCRM_MAX_SCAN_SAMPLES];
} rcmMsg_FullScanInfo;

#endif
