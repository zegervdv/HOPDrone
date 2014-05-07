//_____________________________________________________________________________
//
//   Author: Samuel Van de Velde
//	 Date : 31/05/2013
//   Declarations for LCM communications functions.
//
//_____________________________________________________________________________

#ifndef __lcm_h
#define __lcm_h

#ifdef __cplusplus
    extern "C" {
#endif


// Message Ids;
#define LCM_MSG_DOLOCALIZATION		0x05		// message from host -> device to initiate the ranging & localization (see lcmMsg_DoLoc)
#define LCM_MSG_REGISTER_NODE		0x06		// message from device -> host to register the device
#define LCM_MSG_LOC_INFO			0x07		// message from device -> host containing localization information (see lcmMsg_LocInfo)
#define LCM_MSG_GET_CONNECTION		0x08		// to be implemented //
#define LCM_MSG_DOCALIBRATION		0x09		// message from host -> device to initiate calibration procedure (see lcmMsg_DoCal)
#define LCM_MSG_CAL_INFO			0x0A		// message from device -> host containing calibration data (see lcmMsg_CalInfo)

// some settings
#define LCM_MAX_NODES				80			// maximum number of nodes in the network
#define LCM_MAX_NEIGHBOURS			10			// maximum number of neighboring users.
#define LCM_MAX_CALDATAPOINTS		20			// maximum amount of range information data to be sent in one calInfo message

// Flags
#define LCMFLAG_ONBOARD_LOCALIZATION	1		// on-board estimation of the position
#define LCMFLAG_COOP					2		// enable cooperation -> ranging with anchors and users!
#define LCMFLAG_3D						4		// enable 3D localization (only applicable when on-board localization is on)
#define LCMFLAG_KALMAN        8 // enable kalman filter

typedef struct __attribute__((__packed__)){
	// ID of the responding radio
	uint32_t anchorId;
	uint32_t precisionRangeMm;
	uint16_t precisionRangeErrEst;
	uint16_t reqLEDFlags;
	uint16_t respLEDFlags;
	uint16_t channelRiseTime;
	int32_t coarseTOFInBins;
	uint32_t timestamp;			// milliseconds since radio boot at the time of the range conversation
	uint8_t	trials;				// number of times ranging was performed before success.
}lcm_rangeInfo;

typedef struct __attribute__((__packed__)){
	uint8_t 	msgID;
	uint32_t 	activeNodeID;
	uint8_t 	options;
	uint32_t	nUsers;
	uint32_t	nAnchors;

	uint32_t 	data[LCM_MAX_NODES];			// this is a collection of data: first the user IDs, then anchor IDs, then anchor positions: x, y, z

} lcmMsg_DoLoc;

typedef struct __attribute__((__packed__)){
	uint8_t 	msgID;
	uint32_t	nAnchors;
	float32_t	estim_x;
	float32_t	estim_y;
	float32_t	estim_z;
  int32_t acc_x_axis;
  int32_t acc_y_axis;
  int32_t acc_z_axis;
	lcm_rangeInfo 	data[LCM_MAX_NEIGHBOURS];

} lcmMsg_LocInfo;

// Calibration info from one walking path
typedef struct __attribute__((__packed__)){
	uint8_t 	msgID;
	uint32_t	pathID;
	uint32_t	nDataPoints;
	uint32_t 	data[LCM_MAX_CALDATAPOINTS*4];
} lcmMsg_CalInfo;

// Packet to start calibrating
typedef struct __attribute__((__packed__)){
	uint8_t 	msgID;
	uint32_t	nAnchors;
	uint32_t 	data[LCM_MAX_NODES];			// these represent the anchor node id's
} lcmMsg_DoCal;



#ifdef __cplusplus
    }
#endif


#endif
