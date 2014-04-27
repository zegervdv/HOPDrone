//_____________________________________________________________________________
//
// #includes 
//_____________________________________________________________________________

#include <string.h>
#include <stdint.h>
#include "rcmIf.h"
#include "stm32f4xx_conf.h"

//
#define MAX_DATASIZE		1024
#define MAX_PACKETS 		5

//
//_____________________________________________________________________________
//
// static data
//_____________________________________________________________________________


// static data

// FIFO list with packets (Linked list)
typedef struct message Message;
typedef struct{
	uint8_t *data;				// buffer to raw packet data (without header, packet size or CRC)
	uint16_t packet_size;		// size of the packet
	Message *next;				// pointer to next packet
}message;

static message *head = NULL;			// Head of linked list
static message *last_message = NULL;	// last pointer in linked list
static uint8_t nPackets = 0;
static uint16_t crc=0;
static uint16_t sync = 0;				// field that gives the index of character received
static uint32_t USARTid = USART2;		// use USART2 by default


//_____________________________________________________________________________
//
// Private function prototypes 
//_____________________________________________________________________________

static void ResetMessage(void);
static void writeUsart(unsigned volatile char*,uint8_t length);
static unsigned short crc16(void *buf, int len);


//_____________________________________________________________________________
//
// rcmIfInit - perform initialization
//_____________________________________________________________________________

void rcmIfInit(uint32_t usart_id)
{
	USARTid = usart_id;

	//interrupt priority RX defined in this file.
	/* USART configured as follow:
			- BaudRate = 115200 baud
			- Word Length = 8 Bits
			- One Stop Bit
			- No parity
			- Hardware flow control disabled (RTS and CTS signals)
			- Receive and transmit enabled
	*/

	/* initialise gpio pins */
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	if(USARTid == USART2){
		// Enable USART 2
		// note: this uart uses pin PA2 and PA3 (TX and RX).

		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* Enable UART clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

		//Configure USART2 pins:  Rx and Tx ----------------------------
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* USART configuration */
		USART_Init(USART2, &USART_InitStructure);

		//enable the usart rx interrupt
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* Enable USART */
		USART_Cmd(USART2, ENABLE);
	}else if(USARTid == USART3){
		// Enable USART 3
		// note: this uart uses pin PD8 and PD9 (TX and RX).

		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		/* Enable UART clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

		//Configure USART2 pins:  Rx and Tx ----------------------------
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* USART configuration */
		USART_Init(USART3, &USART_InitStructure);

		//enable the usart rx interrupt
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* Enable USART */
		USART_Cmd(USART3, ENABLE);
	}

	/* Create first message in linked list */
	if(head == NULL){
	  head = (message*)malloc(sizeof(message));
	}

	sync = 0;
	head->next = NULL;
	head->data = NULL;
	head->packet_size = 0;
	last_message = head;
	nPackets = 0;
}

//_____________________________________________________________________________
//
// rcmIfClose - perform cleanup
//_____________________________________________________________________________

void rcmIfClose(void)
{
	if(USARTid == 2){
		GPIO_DeInit(GPIOA);
		USART_DeInit(USART2);
	}else if(USARTid == 3){
		GPIO_DeInit(GPIOD);
		USART_DeInit(USART3);
	}
}

//_____________________________________________________________________________
//
// rcmIfGetPacket - return RCM packet if not ready return 0;
//
//_____________________________________________________________________________

int rcmIfGetPacket(void *pkt_rec, uint16_t size_rec){
	// return the first packet in the FIFO list

	if(head->next != NULL){

		uint16_t pkt_size = head->packet_size;
		pkt_rec = memcpy(pkt_rec , head->data, pkt_size);

		message *tmp = head->next;
		free(head->data);
		free(head);
		head = tmp;
		nPackets--;

		return pkt_size;

	}else{
		// no packet ready
		return 0;
	}

}


//_____________________________________________________________________________
//
// rcmIfGMakePacket - void return
//

//_____________________________________________________________________________

void rcmIfMakePacket(uint8_t rec_word)
{

	if(sync <2){
		if (rec_word == 0xa5)
			sync++;
		else
			sync = 0;
	}if(sync <3){
		if (rec_word == 0xa5){}
		else{
			sync++;
			last_message->packet_size = ((uint16_t)rec_word)<<8;
		}
	}else if(sync < 4){
		sync++;
		last_message->packet_size = (last_message->packet_size | rec_word);

		if(last_message->packet_size > MAX_DATASIZE)
		{
			// error: drop this packet
			sync = 0;
			last_message->packet_size = 0;
		}else{
			// now allocate memory for the message;
			last_message->data = (uint8_t*)calloc(last_message->packet_size, sizeof(uint8_t));
		}
	}else{

		if(sync - 4 >= last_message->packet_size){

			// do CRC check
			if(sync - 4 == last_message->packet_size){
				crc=((uint16_t)rec_word)<<8;
				sync++;
			}else{
				crc=crc | rec_word;

				uint16_t calc_crc=0;
				calc_crc=crc16(last_message->data, last_message->packet_size);

				if(calc_crc == crc){
					nPackets++;

					// message was successfully received, get ready for a new message.
					message *temp;                           	// create a temporary node
					temp = (message*)malloc(sizeof(message));  	// allocate space for node
					if(temp != NULL){
						temp->data = NULL;
						temp->packet_size = 0;
						temp->next = NULL;                   		// second field will be null(last node)
						last_message->next = temp;                  // 'temp' node will be the last node
						last_message = temp;
					}else{
						// allocation failed!
						// Free the memory of the current packet.
						free(last_message->data);
					}

					sync = 0;
					crc=0;

					if(nPackets > MAX_PACKETS){
						// drop the oldest packet
						message *tmp = head->next;
						free(head->data);
						free(head);
						head = tmp;
						nPackets--;
					}
				}else{
					// drop the packet
					// free the memory of the message data, and reinitialize the packet state.
					free(last_message->data);
					last_message->packet_size = 0;
					sync = 0;
					crc=0;
				}
			}
		}else if(last_message->data != NULL){
			// add data to the buffer
			last_message->data[sync++ - 4] = rec_word;
		}
	}
}


//_____________________________________________________________________________
//
// rcmIfSendPacket - send packet out RCM interface
//
// Number of bytes sent, ERR on error.
//_____________________________________________________________________________

void rcmIfSendPacket(void *pkt_send, uint16_t size_send)
{
	//ResetMessage();

	// drop the current receiving packet
	// free the memory of the message data, and reinitialize the packet state.
	free(last_message->data);
	last_message->packet_size = 0;
	sync = 0;
	crc=0;

	// send sync bytes
	writeUsart((unsigned char*) "\xa5\xa5",2);

	// send size bytes
	uint8_t array[2]={size_send>>8,size_send & 0x00FF};
	writeUsart(array,2);

	// send packet
	writeUsart((unsigned char*)pkt_send,size_send);
	// send CRC
	uint16_t val = crc16(pkt_send, size_send);
	array[0]=val>>8;
	array[1]=val & 0x00FF;
	writeUsart(array,2);
}

//_____________________________________________________________________________
//
// write - write data to serial port
//
//_____________________________________________________________________________

void writeUsart(unsigned volatile char *str,uint8_t length)
{
	uint8_t i=0;
	while(i<length){
			/* Loop until the end of transmission */
			 while (USART_GetFlagStatus(USARTid, USART_FLAG_TC) == RESET){}
			 USART_SendData(USARTid, str[i]);
			 i++;
		}
}



// Table of CRC constants - implements x^16+x^12+x^5+1
static const uint16_t crc16_tab[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0, 
};


//_____________________________________________________________________________
//
// crc16 - calculate 16-bit CRC on buffer
//
//_____________________________________________________________________________

uint16_t crc16(void *ptr, int len)
{
    int i;
    uint16_t cksum;
    unsigned char *buf = (unsigned char *)ptr;

    cksum = 0;
    for (i = 0;  i < len;  i++) {
        cksum = crc16_tab[((cksum>>8) ^ *buf++) & 0xFF] ^ (cksum << 8);
    }
    return cksum;
}

