//_____________________________________________________________________________
//
// Copyright 2011-2 Time Domain Corporation
//
//
// rcm.c
//
//   A collection of functions to communicate with an RCM.
//
//
//_____________________________________________________________________________


//_____________________________________________________________________________
//
// #includes 
//_____________________________________________________________________________

#include <string.h>

#include "rcm.h"
#include "rcmif.h"


extern void Delay(uint32_t nTick);


//_____________________________________________________________________________
//
// #defines 
//_____________________________________________________________________________


//_____________________________________________________________________________
//
// typedefs
//_____________________________________________________________________________


typedef union
{
    rcmMsg_RangeInfo rangeInfo;
    rcmMsg_DataInfo dataInfo;
} infoMsgs_t;

#ifndef _XTONX_DEFINED
#define _XTONX_DEFINED

__inline unsigned short htons(unsigned short n) {
  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

__inline unsigned short ntohs(unsigned short n) {
  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

__inline unsigned long htonl(unsigned long n) {
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

__inline unsigned long ntohl(unsigned long n) {
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

#endif


//_____________________________________________________________________________
//
// static data
//_____________________________________________________________________________

static uint16_t msgIdCount=1;


//_____________________________________________________________________________
//
// Private function prototypes 
//_____________________________________________________________________________

bool initConn(uint32_t usart_id){
	//initialize interface
	/* rcmIfInit(usart_id); */
	uint8_t i=0;
	// Make sure RCM is awake
	while (rcmSleepModeSet(RCM_SLEEP_MODE_ACTIVE) != 0 && i>10)
	{
		return ERR;
	}

	// Make sure opmode is RCM
	if (rcmOpModeSet(RCM_OPMODE_RCM) != 0)
	{
	   return ERR;

	}

	return OK;
}


//_____________________________________________________________________________
//
//  CloseConn   - close connection but first set radio to sleep mode.
//_____________________________________________________________________________
//
void closeConn(void){
	//set RCM in standby mode:
	rcmSleepModeSet(RCM_SLEEP_MODE_STANDBY_SER);
	//free functions interface:
	rcmIfClose();
}

//_____________________________________________________________________________
//
// rcmBit - execute Built-In Test
//_____________________________________________________________________________

bool rcmBit(int *status)
{
    rcmMsg_BitRequest request;
    rcmMsg_BitConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_BIT_REQUEST);
	request.msgId = htons(msgIdCount++);

	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(confirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.status = ntohl(confirm.status);

        // is this the correct message type and is status good?
        if (confirm.msgType == RCM_BIT_CONFIRM)
        {
            *status = confirm.status;
            retVal = OK;
        }
    }
    return retVal;
}


//_____________________________________________________________________________
//
// rcmConfigGet - get rcm configuration from radio
//_____________________________________________________________________________

bool rcmConfigGet(rcmConfiguration *config)
{
    rcmMsg_GetConfigRequest request;
    rcmMsg_GetConfigConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;


    // create request message
	request.msgType = htons(RCM_GET_CONFIG_REQUEST);
	request.msgId = htons(msgIdCount++);

    //send packet
    rcmIfSendPacket(&request, (uint8_t) sizeof(request));
    //wait for the response or delay too large
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(rcmMsg_GetConfigConfirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(rcmMsg_GetConfigConfirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.msgId = ntohs(confirm.msgId);

        // is this the correct message type?
        if (confirm.msgType == RCM_GET_CONFIG_CONFIRM)
        {
            // copy config from message to caller's structure
            memcpy(config, &confirm.config, sizeof(*config));
            // Handle byte ordering
            config->nodeId = ntohl(config->nodeId);
            config->integrationIndex = ntohs(config->integrationIndex);
            config->electricalDelayPsA = ntohl(config->electricalDelayPsA);
            config->electricalDelayPsB = ntohl(config->electricalDelayPsB);
            config->flags = ntohs(config->flags);

            // milliseconds since radio boot
            confirm.timestamp = ntohl(confirm.timestamp);

            // status code
            confirm.status = ntohl(confirm.status);
            // only return OK if status is OK
            if (confirm.status == OK)
                retVal = OK;
        }
    }
    return retVal;
}


//_____________________________________________________________________________
//
// rcmConfigSet - set RCM configuration in radio
//_____________________________________________________________________________

bool rcmConfigSet(rcmConfiguration *config)
{
    rcmMsg_SetConfigRequest request;
    rcmMsg_SetConfigConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_SET_CONFIG_REQUEST);
	request.msgId = htons(msgIdCount++);
    memcpy(&request.config, config, sizeof(*config));

    // Handle byte ordering in config struct
    request.config.nodeId = htonl(config->nodeId);
    request.config.integrationIndex = htons(config->integrationIndex);
    request.config.electricalDelayPsA = htonl(config->electricalDelayPsA);
    request.config.electricalDelayPsB = htonl(config->electricalDelayPsB);
    request.config.flags = htons(config->flags);

    //send packet
	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	//wait for the response or delay too large
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(rcmMsg_GetConfigConfirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.status = ntohl(confirm.status);

        // is this the correct message type and is status good?
        if (confirm.msgType == RCM_SET_CONFIG_CONFIRM &&
                confirm.status == OK)
            retVal = OK;
    }
    return retVal;
}


//_____________________________________________________________________________
//
// rcmOpModeSet - set RCM operational mode
//_____________________________________________________________________________

bool rcmOpModeSet(int opMode)
{
    rcmMsg_SetOpmodeRequest request;
    rcmMsg_SetOpmodeConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_SET_OPMODE_REQUEST);
	request.msgId = htons(msgIdCount++);
    request.opMode = htonl(opMode);

    //send packet
	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	//wait for the response or delay too large
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(confirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.status = ntohl(confirm.status);

        // is this the correct message type and is status good?
        if (confirm.msgType == RCM_SET_OPMODE_CONFIRM &&
                confirm.status == OK)
            retVal = OK;
    }
    return retVal;
}


//_____________________________________________________________________________
//
// rcmSleepModeSet - set RCM sleep mode
//_____________________________________________________________________________

bool rcmSleepModeSet(int sleepMode)
{
    rcmMsg_SetSleepModeRequest request;
    rcmMsg_SetSleepModeConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_SET_SLEEP_MODE_REQUEST);
	request.msgId = htons(msgIdCount++);
    request.sleepMode = htonl(sleepMode);

    //send packet
	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	//wait for the response or delay too large
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(confirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.status = ntohl(confirm.status);

        // is this the correct message type and is status good?
        if (confirm.msgType == RCM_SET_SLEEP_MODE_CONFIRM &&
                confirm.status == OK)
            retVal = OK;
    }
    return retVal;
}


//_____________________________________________________________________________
//
// rcmStatusInfoGet - retrieve RCM status from radio
//_____________________________________________________________________________

bool rcmStatusInfoGet(rcmMsg_GetStatusInfoConfirm *confirm)
{
    rcmMsg_GetStatusInfoRequest request;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_GET_STATUS_INFO_REQUEST);
	request.msgId = htons(msgIdCount++);

	//send packet
	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	//wait for the response or delay too large
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(rcmMsg_GetStatusInfoConfirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(rcmMsg_GetStatusInfoConfirm))
    {
        // Handle byte ordering
        confirm->msgType = ntohs(confirm->msgType);
        confirm->msgId = ntohs(confirm->msgId);

        // is this the correct message type?
        if (confirm->msgType == RCM_GET_STATUS_INFO_CONFIRM)
        {
            // Handle byte ordering
            confirm->appVersionBuild = ntohs(confirm->appVersionBuild);
            confirm->uwbKernelVersionBuild = ntohs(confirm->uwbKernelVersionBuild);
            confirm->serialNum = ntohl(confirm->serialNum);
            confirm->temperature = ntohl(confirm->temperature);

            // status code
            confirm->status = ntohl(confirm->status);
            // only return OK if status is OK
            if (confirm->status == OK)
                retVal = OK;
        }
    }
    return retVal;
}


//_____________________________________________________________________________
//
// rcmRangeTo - range to another RCM module
//_____________________________________________________________________________

uint8_t rcmRangeTo(uint8_t destNodeId, uint8_t antennaMode, uint16_t dataSize, char *data,rcmMsg_RangeInfo *rangeInfo,rcmMsg_DataInfo *dataInfo)
{
	uint8_t retVal=ERR;
	infoMsgs_t infoMsgs;
    rcmMsg_SendRangeRequest request;
    rcmMsg_SendRangeRequestConfirm confirm;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_SEND_RANGE_REQUEST);
	request.msgId = htons(msgIdCount++);
    request.responderId = htonl(destNodeId);
    request.antennaMode = antennaMode;
    request.dataSize = htons(dataSize);


    // make sure there isn't too much data
    if (dataSize > RCM_USER_DATA_LENGTH)
        dataSize = RCM_USER_DATA_LENGTH;
    // copy data into message
    memcpy(request.data, data, dataSize);

    // send message to RCM
	numBytes = sizeof(request) - RCM_USER_DATA_LENGTH + dataSize;
	rcmIfSendPacket(&request, numBytes);

	// wait for response
	numBytes=0;
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(confirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.msgId = ntohs(confirm.msgId);

        // is this the correct message type?
        if (confirm.msgType == RCM_SEND_RANGE_REQUEST_CONFIRM)
        {
            // check status code
            confirm.status = ntohl(confirm.status);
            if (confirm.status == OK)
            {
                retVal = OK;

                // clear out caller's info structs
                memset(rangeInfo, 0, sizeof(*rangeInfo));
                memset(dataInfo, 0, sizeof(*dataInfo));
                rangeInfo->rangeStatus = RCM_RANGE_STATUS_TIMEOUT;

                // Collect any info messages


                //wait on received data, 2 variables used to ensure no loss of data!
				numBytes=0;
                i=0;
				while(numBytes==0 && i<10){
					//wait for packet to be ready, if packet is ready automatically in receive variable
					//if packet takes longer than DELAY_TIME -> receive failed.
					Delay(DELAY_TIME/10);
					numBytes= rcmIfGetPacket(&infoMsgs, sizeof(infoMsgs));
					i++;
				}
				while (numBytes > 0){

                    // make sure this info message has the same msgId as the request
                    // the msgId is in the same place in all structs
                    if ((ntohs(infoMsgs.rangeInfo.msgId) == msgIdCount - 1) ||
                            (ntohs(infoMsgs.rangeInfo.msgId) == 0xffff && msgIdCount == 0))
                    {
                        switch(ntohs(infoMsgs.rangeInfo.msgType))
                        {
                            case RCM_RANGE_INFO:
                                // copy message to caller's struct
                                memcpy(rangeInfo, &infoMsgs.rangeInfo, sizeof(*rangeInfo));
                                // handle byte ordering
                                rangeInfo->msgType = ntohs(rangeInfo->msgType);
                                rangeInfo->msgId = ntohs(rangeInfo->msgId);
                                rangeInfo->responderId = ntohl(rangeInfo->responderId);
                                rangeInfo->stopwatchTime = ntohs(rangeInfo->stopwatchTime);
                                rangeInfo->precisionRangeMm = ntohl(rangeInfo->precisionRangeMm);
                                rangeInfo->coarseRangeMm = ntohl(rangeInfo->coarseRangeMm);
                                rangeInfo->filteredRangeMm = ntohl(rangeInfo->filteredRangeMm);
                                rangeInfo->precisionRangeErrEst = ntohs(rangeInfo->precisionRangeErrEst);
                                rangeInfo->coarseRangeErrEst = ntohs(rangeInfo->coarseRangeErrEst);
                                rangeInfo->filteredRangeErrEst = ntohs(rangeInfo->filteredRangeErrEst);
                                rangeInfo->filteredRangeVel = ntohs(rangeInfo->filteredRangeVel);
                                rangeInfo->filteredRangeVelErrEst = ntohs(rangeInfo->filteredRangeVelErrEst);
                                rangeInfo->reqLEDFlags = ntohs(rangeInfo->reqLEDFlags);
                                rangeInfo->respLEDFlags = ntohs(rangeInfo->respLEDFlags);
                                rangeInfo->channelRiseTime = ntohs(rangeInfo->channelRiseTime);
                                rangeInfo->vPeak = ntohs(rangeInfo->vPeak);
                                rangeInfo->coarseTOFInBins = ntohl(rangeInfo->coarseTOFInBins);
                                rangeInfo->timestamp = ntohl(rangeInfo->timestamp);
                                break;
                            case RCM_DATA_INFO:
                                // copy message to caller's struct
                                memcpy(dataInfo, &infoMsgs.dataInfo, sizeof(*dataInfo));
                                // handle byte ordering
                                dataInfo->msgType = ntohs(dataInfo->msgType);
                                dataInfo->msgId = ntohs(dataInfo->msgId);
                                dataInfo->sourceId = ntohl(dataInfo->sourceId);
                                dataInfo->channelRiseTime = ntohs(dataInfo->channelRiseTime);
                                dataInfo->vPeak = ntohs(dataInfo->vPeak);
                                dataInfo->timestamp = ntohl(dataInfo->timestamp);
                                dataInfo->dataSize = ntohs(dataInfo->dataSize);
                                break;
                        }
						
						// We get RANGE_INFO last.
						if(ntohs(infoMsgs.rangeInfo.msgType) == RCM_RANGE_INFO)
							retVal=OK;
							break;
                    }

                    //wait for new package to be received.
                    numBytes=0;
					i=0;
					while(numBytes==0 && i<10){
						//wait for packet to be ready, if packet is ready automatically in receive variable
						//if packet takes longer than DELAY_TIME -> receive failed.
						Delay(DELAY_TIME/10);
						numBytes= rcmIfGetPacket(&infoMsgs, sizeof(infoMsgs));
						i++;
					}
				}
			}
		}
    }

    return retVal;
}


//_____________________________________________________________________________
//
// rcmDataSend - broadcast a data-only packet
//_____________________________________________________________________________

bool rcmDataSend(uint8_t antennaMode, uint16_t dataSize, char *data)
{
    rcmMsg_SendDataRequest request;
    rcmMsg_SendDataConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_SEND_DATA_REQUEST);
	request.msgId = htons(msgIdCount++);
    request.antennaMode = antennaMode;
    request.dataSize = htons(dataSize);
    // make sure there isn't too much data
    if (dataSize > RCM_USER_DATA_LENGTH)
        dataSize = RCM_USER_DATA_LENGTH;
    // copy data into message
    memcpy(request.data, data, dataSize);

    // send message to RCM
    numBytes = sizeof(request) - RCM_USER_DATA_LENGTH + dataSize;
    //send packet
	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	//wait for the response or delay too large
	numBytes=0;
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(confirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.msgId = ntohs(confirm.msgId);

        // is this the correct message type?
        if (confirm.msgType == RCM_SEND_DATA_CONFIRM)
        {
            // status code
            confirm.status = ntohl(confirm.status);
            // only return OK if status is OK
            if (confirm.status == OK)
            {
                retVal = OK;
            }
        }
    }
    return retVal;

}

bool rcmDataReceive(rcmMsg_DataInfo* dataInfo){

	bool retVal = ERR;
	int numBytes;
	numBytes = rcmIfGetPacket(dataInfo, sizeof(*dataInfo));

	// no bytes received
	if(numBytes == 0)
		return ERR;

	// Handle byte ordering
	dataInfo->msgType = ntohs(dataInfo->msgType);

	// is this the correct message type? (i.e. a RCM_DATA_INFO message)
	if (dataInfo->msgType == RCM_DATA_INFO)
	{
		retVal=OK;
		// Handle byte ordering
		dataInfo->msgId = ntohs(dataInfo->msgId);
		dataInfo->sourceId = ntohl(dataInfo->sourceId);
		dataInfo->channelRiseTime = ntohs(dataInfo->channelRiseTime);
		dataInfo->vPeak = ntohs(dataInfo->vPeak);
		dataInfo->timestamp = ntohl(dataInfo->timestamp);
		dataInfo->dataSize = ntohs(dataInfo->dataSize);
	}

	return retVal;
}

//_____________________________________________________________________________
//
// rcmLocAnchors - broadcast a data-only packet 0x1 and should receive a packet with
//					the location of the anchors.
//
//		returns OK if succesfull, otherwise ERR
//_____________________________________________________________________________


bool rcmLocAnchors(uint8_t antennaMode,rcmMsg_DataInfo* dataInfo){
    rcmMsg_SendDataRequest request;
    rcmMsg_SendDataConfirm confirm;
    bool retVal = ERR;
    uint16_t numBytes=0;

    // create request message
	request.msgType = htons(RCM_SEND_DATA_REQUEST);
	request.msgId = htons(msgIdCount++);
    request.antennaMode = antennaMode;

    //SEND data 0x2 to pc, the pc will know this means request for locateanchor.
    request.dataSize = htons(1);
    request.data[0]=0x2;

    // send message to RCM
    numBytes = sizeof(request) - RCM_USER_DATA_LENGTH + 1;
    //send packet
	rcmIfSendPacket(&request, (uint8_t) sizeof(request));

	//wait for the response or delay too large
	numBytes=0;
	uint8_t i=0;
	while(numBytes==0 && i<10){
		//wait for packet to be ready, if packet is ready automatically in receive variable
		//if packet takes longer than DELAY_TIME -> receive failed.
		Delay(DELAY_TIME/10);
		numBytes= rcmIfGetPacket(&confirm, sizeof(confirm));
		i++;
	}

    // did we get a response from the RCM?
    if (numBytes == sizeof(confirm))
    {
        // Handle byte ordering
        confirm.msgType = ntohs(confirm.msgType);
        confirm.msgId = ntohs(confirm.msgId);

        // is this the correct message type?
        if (confirm.msgType == RCM_SEND_DATA_CONFIRM)
        {
            // status code
            confirm.status = ntohl(confirm.status);
            // only return OK if status is OK
            if (confirm.status == OK)
            {
            	while(numBytes!=0 && retVal==ERR){
					//wait for the response or delay too large
					numBytes=0;
					uint8_t i=0;
					while(numBytes==0 && i<10){
						//wait for packet to be ready, if packet is ready automatically in receive variable
						//if packet takes longer than DELAY_TIME -> receive failed.
						Delay(DELAY_TIME/10);
						numBytes= rcmIfGetPacket(dataInfo, sizeof(*dataInfo));
						i++;
					}

					if (dataInfo->data[0] == 0x3){

						// handle byte ordering
						dataInfo->msgType = ntohs(dataInfo->msgType);
						dataInfo->msgId = ntohs(dataInfo->msgId);
						dataInfo->sourceId = ntohl(dataInfo->sourceId);
						dataInfo->channelRiseTime = ntohs(dataInfo->channelRiseTime);
						dataInfo->vPeak = ntohs(dataInfo->vPeak);
						dataInfo->timestamp = ntohl(dataInfo->timestamp);
						dataInfo->dataSize = ntohs(dataInfo->dataSize);

						retVal=OK;
					}
            	}
            }
        }
    }
    return retVal;

	return OK;
}
