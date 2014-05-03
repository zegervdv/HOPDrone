//_____________________________________________________________________________

#ifndef MAIN_H
#define MAIN_H
//
// #defines
//_____________________________________________________________________________
#define DELAY_TIME				100 //in ms
//this delay time is used to define the waiting time when doing the communication
//this can be lowered for faster communication, but to low will mean that the communication
//will fail.
//this value will be lowered for best performance.
#define USED_ANTENNA			  RCM_ANTENNAMODE_TXA_RXA
#define SECOND_ANTENNA			RCM_ANTENNAMODE_TXB_RXB


//if two antennas are used uncomment this line:
#define TWOANTENNAS

//if the pc is used to update anchorpositions in flash uncomment
#define PCANCHORS

//Choose a localization algorithm through these defines
//#define PARTICLEFILTER
#define LEASTSQUARES

//typedefs


#endif
