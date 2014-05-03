//
// rcmIf.h
//
//   Declarations for RCM interface functions.
//
//_____________________________________________________________________________

#ifndef __rcmIf_h
#define __rcmIf_h

#ifdef __cplusplus
    extern "C" {
#endif

//_____________________________________________________________________________
//
// #includes
//_____________________________________________________________________________

#include "stm32f4xx_conf.h"

//_____________________________________________________________________________
//
// #defines
//_____________________________________________________________________________

#ifndef OK
#define OK 0
#define ERR (-1)
#endif

//_____________________________________________________________________________
//
//  Function prototypes
//_____________________________________________________________________________


//
//  rcmIfInit
//
//  Performs initialization necessary for particular type of interface.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void rcmIfInit(USART_TypeDef* usart_id);


//
//  rcmIfClose
//
//  Parameters:  void
//  Return:      void
//
//  Closes port to radio.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void rcmIfClose(void);

//
//  rcmIfPacketReady
//
//  Parameters:  void *pk_rec - pointer to packet to receive
//               uint16_t size_rec - size of packet to receive
//  Return:      size or zero
//
//  return OK if packet is received en crc check is valid
// return ERR if error or not received yet.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

int rcmIfGetPacket(void *pkt_rec, uint16_t size_rec );
int rcmIfGetPacket2(void *pkt_rec, uint16_t size_rec );

//
//  rcmIfMakePacket
//
//  Parameters:  uint8_t rec_word - the word that is receivec by irq
//
//
//  makes packet from received word. every word that is received is sent to this
//  method
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void rcmIfMakePacket(uint8_t rec_word);

void rcmIfMakePacket2(uint8_t rec_word);

//
//  rcmIfSendPacket
//
//  Parameters:  void *pkt_send - pointer to packet to send
//               uint16_t size_send - size of packet to send
//
//  Sends packet to RCM interface.
//  Returns ERR on write error, otherwise OK.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void rcmIfSendPacket(void *pkt_send, uint16_t size_send);

//returns 0 if data isn't ready
//returns 1 if data is ready
//returns 2 if error.
#ifdef __cplusplus
    }
#endif


#endif
