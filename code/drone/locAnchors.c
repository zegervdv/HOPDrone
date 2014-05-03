/* this file will locate the anchors by making a connection to the PC node defined in
 * the main file. If the connection fails the method will use the old values of the location
 * of the anchors.
*/

//includes
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx_conf.h"
#include "rcm.h"
#include "leds.h"
#include "main.h"

extern void Delay(uint32_t nTick);


static bool start=true;

// Private typedef -----------------------------------------------------------
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

// Private define ------------------------------------------------------------

#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_2   // Start @ of user Flash area
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_5   // End @ of user Flash area

// Base address of the Flash sectors
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) // Base @ of Sector 0, 16 Kbytes
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) // Base @ of Sector 1, 16 Kbytes
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) // Base @ of Sector 2, 16 Kbytes
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) // Base @ of Sector 3, 16 Kbytes
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) // Base @ of Sector 4, 64 Kbytes
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) // Base @ of Sector 5, 128 Kbytes
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) // Base @ of Sector 6, 128 Kbytes
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) // Base @ of Sector 7, 128 Kbytes
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) // Base @ of Sector 8, 128 Kbytes
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) // Base @ of Sector 9, 128 Kbytes
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) // Base @ of Sector 10, 128 Kbytes
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) // Base @ of Sector 11, 128 Kbytes

// Private macro -------------------------------------------------------------
// Private variables ---------------------------------------------------------
uint32_t StartSector = 0, EndSector = 0, Address = 0;

// Private function prototypes -----------------------------------------------
// Private functions ---------------------------------------------------------
uint32_t GetSector(uint32_t Address);
void Write2flash(uint32_t data);

void LocateAnchors(uint8_t* nrAnchors,uint32_t** anchorsX, uint32_t** anchorsY,uint8_t** anchorID)
{
	//don't want user button to interrupt the handling.
	NVIC_DisableIRQ(EXTI0_IRQn);
	#ifdef PCANCHORS
		rcmMsg_DataInfo anchorPos;
		bool err=rcmLocAnchors(USED_ANTENNA,&anchorPos);
		if(!err){
			//structure received data:
			//first byte : nrAnchors[0]
			//folowing nrAnchors[0] bytes: anchorID
			//following 4*nrAnchors[0] bytes: anchorsX
			//folowing 4*nrAnchors[0] bytes: anchorsY
			// bytes positions: first byte is MSB.
			//					second byte is LSB.

			//update position anchors.
			nrAnchors[0]= anchorPos.data[1];

			//check if length ok
			if(anchorPos.dataSize==(9*nrAnchors[0]+2)){

				free(*anchorID);
				free(*anchorsX);
				free(*anchorsY);

				uint8_t* aID=(uint8_t*)malloc(nrAnchors[0]);
				uint32_t* xPos=(uint32_t*)malloc(nrAnchors[0]);
				uint32_t* yPos=(uint32_t*)malloc(nrAnchors[0]);
				//update

				uint8_t i;
				//need this position because first all the x positions are stored.
				uint16_t firstPosY=5*nrAnchors[0]+2;
				for(i=0;i<nrAnchors[0];i++){
					aID[i]=anchorPos.data[i+2];
					xPos[i] = anchorPos.data[nrAnchors[0]+4*i+2]<<24 | anchorPos.data[nrAnchors[0]+4*i+3]<<16 | anchorPos.data[nrAnchors[0]+4*i+4]<<8 | anchorPos.data[nrAnchors[0]+4*i+5];
					yPos[i] = anchorPos.data[firstPosY+4*i]<<24 | anchorPos.data[firstPosY+4*i+1]<<16 | anchorPos.data[firstPosY+4*i+2]<<8 | anchorPos.data[firstPosY+4*i+3];
				}
				*anchorID=aID;
				*anchorsX=xPos;
				*anchorsY=yPos;

				//save new positions in flash.

				// Unlock the Flash to enable the flash control register access ************
				  FLASH_Unlock();

				  // Erase the user Flash area
				  //  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) **********

				  // Clear pending flags (if any)
				  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
				                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

				  // Get the number of the start and end sectors
				  StartSector = GetSector(FLASH_USER_START_ADDR);
				  EndSector = GetSector(FLASH_USER_END_ADDR);

				  i=0;
				  for (i = StartSector; i < EndSector; i += 8)
				  {
				    // Device voltage range supposed to be [2.7V to 3.6V], the operation will
				    //  be done by word
				    if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
				    {
				      // Error occurred while sector erase.
				      //  User can add here some code to deal with this error
				      while (1)
				      {
				    	  //blue light on
				    	  STM32F4_Discovery_LEDInit(LED6);
				    	  STM32F4_Discovery_LEDOn(LED6);
				      }
				    }
				  }

				  // Program the user Flash area word by word
				  //  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) **********

				  Address = FLASH_USER_START_ADDR;

				  Write2flash((uint32_t) nrAnchors[0]);
				  for(i=0;i<nrAnchors[0];i++){
					  Write2flash(aID[i]);
					  Write2flash(xPos[i]);
					  Write2flash(yPos[i]);
				  }

				  // Lock the Flash to disable the flash control register access (recommended
				   //  to protect the FLASH memory against possible unwanted operation) ********
				  FLASH_Lock();

				  // Check if the programmed data is OK
				  //    MemoryProgramStatus = 0: data programmed correctly
				   //   MemoryProgramStatus != 0: number of words not programmed correctly *****
				  Address = FLASH_USER_START_ADDR;
				  __IO uint32_t MemoryProgramStatus = 0x0;
				  __IO uint32_t data32 = *(__IO uint32_t*)Address;

				    if (data32 != nrAnchors[0])
				    {
				      MemoryProgramStatus++;
				    }

				    Address = Address + 4;

				    for(i=0;i<nrAnchors[0];i++){
				    	data32 = *(__IO uint32_t*)Address;
				    	if (data32 != aID[i])
						{
						  MemoryProgramStatus++;
						}
				    	Address = Address + 4;

				    	data32 = *(__IO uint32_t*)Address;
						if (data32 != xPos[i])
						{
						  MemoryProgramStatus++;
						}
						Address = Address + 4;

						data32 = *(__IO uint32_t*)Address;
						if (data32 != yPos[i])
						{
						  MemoryProgramStatus++;
						}
						Address = Address + 4;
				    }
				    if(MemoryProgramStatus!=0){
						  while (1)
						  {
							  // blue LED on
							  STM32F4_Discovery_LEDInit(LED6);
							  STM32F4_Discovery_LEDOn(LED6);
						  }
				    }
			}else{
				err=true;
			}

		}else{
			STM32F4_Discovery_LEDInit(LED6);
			STM32F4_Discovery_LEDOn(LED6);
			Delay(10*DELAY_TIME);
			STM32F4_Discovery_LEDOff(LED6);
		}
		//if only err no update is done of the values.
		if(start && err){
			//read out of flash
			start=false;

			Address = FLASH_USER_START_ADDR;
			__IO uint32_t data32 = *(__IO uint32_t*)Address;
			nrAnchors[0]=(uint8_t) data32;
			Address = Address + 4;

		uint8_t* aID=(uint8_t*)malloc(nrAnchors[0]);
		uint32_t* xPos=(uint32_t*)malloc(nrAnchors[0]);
		uint32_t* yPos=(uint32_t*)malloc(nrAnchors[0]);

			uint8_t i=0;
			for(i+0;i<nrAnchors[0];i++){
				data32 = *(__IO uint32_t*)Address;
				aID[i]=(uint32_t) data32;
				Address = Address + 4;

				data32 = *(__IO uint32_t*)Address;
				xPos[i]=(uint32_t) data32;
				Address = Address + 4;

				data32 = *(__IO uint32_t*)Address;
				yPos[i]=(uint32_t) data32;
				Address = Address + 4;
			}

			*anchorID=aID;
			*anchorsX=xPos;
			*anchorsY=yPos;
		}
	#else
		if(start){
			//use stored values
			free(*anchorID);
			free(*anchorsX);
			free(*anchorsY);

			nrAnchors[0]=4;
			uint8_t* aID=(uint8_t*)malloc(nrAnchors[0]);
			uint32_t* xPos=(uint32_t*)malloc(nrAnchors[0]);
			uint32_t* yPos=(uint32_t*)malloc(nrAnchors[0]);

			xPos[0]= 0; yPos[0]= 0;
			xPos[1]= 0; yPos[1]= 10000;
			xPos[2]= 6000; yPos[2]= 10000;
			xPos[3] = 6000; yPos[3] = 0;
			//set id's
			aID[0]=101;
			aID[1]=102;
			aID[2]=103;
			aID[3]=104;

			*anchorID=aID;
			*anchorsX=xPos;
			*anchorsY=yPos;
		}
	#endif
		//handling done if user button was pressed during handling
		//this will be erased.

		NVIC_ClearPendingIRQ(EXTI0_IRQn);
		NVIC_EnableIRQ(EXTI0_IRQn);
}


void Write2flash(uint32_t data){
	if (Address < FLASH_USER_END_ADDR)
	  {
		if (FLASH_ProgramWord(Address, data) == FLASH_COMPLETE)
		{
		  Address = Address + 4;
		}
		else
		{
		  // Error occurred while writing data in Flash memory.
			// User can add here some code to deal with this error
		  while (1)
		  {
			  // blue LED on
			  STM32F4_Discovery_LEDInit(LED6);
			  STM32F4_Discovery_LEDOn(LED6);
		  }
		}
	  }else{
		  while (1)
		  		  {
		  			  // blue LED on
			  	  STM32F4_Discovery_LEDInit(LED6);
			  	  STM32F4_Discovery_LEDOn(LED6);
		  		  }

	  }
}

/*
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;
  }
  else//(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_Sector_11;
  }

  return sector;
}
