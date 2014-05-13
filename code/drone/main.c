/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main, PerformRanging, int2float
 **
 **  Authors     : Thomas Deckmyn, Zeger Van de Vannet
 **
 *****************************************************************************
 */

#include "stm32f4xx_conf.h"
#include "stm32f4xx_it.h"
#include "arm_math.h"
#include "rcm.h"
#include "lcm.h"

#include "leds.h"
#include "main.h"
#include "lms.h"
#include "kalman.h"

#include "accelerometer.h"

#include "stm32f4xx_rng.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//private functions:
void RNG_Config(void);
void PerformRanging(lcmMsg_LocInfo* locInfo, uint8_t nrAnchors, uint32_t* anchorID, uint8_t maxTrials);
float32_t int2float(uint32_t in);

//_____________________________________________________________________________
//
// static data
//_____________________________________________________________________________
static uint32_t RCM_id = 107;				// id of the node (given on the RCM device)
static bool bConnected = false;				// boolean if the node is connected to the central hub.

//_____________________________________________________________________________
//
// main - application entry point
//_____________________________________________________________________________

int main(void)
{
  // Initialization of Kalman Filter
  uint8_t i;
  float32_t weight_c_data[NR_SIGMAPOINTS];
  float32_t weight_m_data[NR_SIGMAPOINTS];
  float32_t position_data[DIMENSIONS], prev_position_data[DIMENSIONS], mkmin_data[DIMENSIONS];
  float32_t sigmapoints_data[NR_SIGMAPOINTS][DIMENSIONS];
  float32_t f_matrix_data[DIMENSIONS*DIMENSIONS];
  float32_t g_matrix_data[DIMENSIONS*(DIMENSIONS/2)];
  float32_t pk_data[DIMENSIONS*DIMENSIONS], pkmin_data[DIMENSIONS*DIMENSIONS];
  float32_t var_u_data[(DIMENSIONS/2)*(DIMENSIONS/2)];
  float32_t r_matrix_data[NR_ANCHORS*NR_ANCHORS];
  float32_t z_matrix_data[NR_ANCHORS * NR_SIGMAPOINTS];

  arm_matrix_instance_f32 weight_m, weight_c;
  position_t position, prev_position;
  position_t mkmin;
  position_t sigmapoints[NR_SIGMAPOINTS];
  arm_matrix_instance_f32 f_matrix, g_matrix;
  arm_matrix_instance_f32 pkmin, pk;
  arm_matrix_instance_f32 var_u, r_matrix;
  arm_matrix_instance_f32 z_matrix;

  // Data of the anchors: x, y, z coordinate plus measured distance from node
  float32_t anchors[NR_ANCHORS][4];

  // Initialize weight factors
  kalman_init_weight_factors(weight_m_data, weight_c_data);
  arm_mat_init_f32(&weight_m, NR_SIGMAPOINTS, 1, weight_m_data);
  arm_mat_init_f32(&weight_c, 1, NR_SIGMAPOINTS, weight_c_data);

  // Initialize position vectors
  kalman_init_position(position_data);
  kalman_init_position(prev_position_data);
  kalman_init_position(mkmin_data);
  arm_mat_init_f32(&position, DIMENSIONS, 1, position_data);
  arm_mat_init_f32(&prev_position, DIMENSIONS, 1, prev_position_data);
  arm_mat_init_f32(&mkmin, DIMENSIONS, 1, mkmin_data);

  // Initialize sigma points
  for (i = 0; i < NR_SIGMAPOINTS; i++) {
    kalman_init_sigmapoints(sigmapoints_data[i]);
    arm_mat_init_f32(&sigmapoints[i], DIMENSIONS, 1, sigmapoints_data[i]);
  }

  // Initialize F and G vectors
  kalman_init_f_matrix(f_matrix_data);
  kalman_init_g_matrix(g_matrix_data);
  arm_mat_init_f32(&f_matrix, DIMENSIONS, DIMENSIONS, f_matrix_data);
  arm_mat_init_f32(&g_matrix, DIMENSIONS, DIMENSIONS/2, g_matrix_data);

  // Initialize Pk and Pkmin
  kalman_init_dimensional_matrix(pk_data);
  kalman_init_dimensional_matrix(pkmin_data);
  arm_mat_init_f32(&pk, DIMENSIONS, DIMENSIONS, pk_data);
  arm_mat_init_f32(&pkmin, DIMENSIONS, DIMENSIONS, pkmin_data);

  // Initialize variance matrices
  kalman_init_variances(var_u_data, r_matrix_data);
  arm_mat_init_f32(&var_u, DIMENSIONS/2, DIMENSIONS/2, var_u_data);
  arm_mat_init_f32(&r_matrix, NR_ANCHORS, NR_ANCHORS, r_matrix_data);

  // Initialize Z matrix
  arm_mat_init_f32(&z_matrix, NR_ANCHORS, NR_SIGMAPOINTS, z_matrix_data);

  //configure delay in ms via systick:
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    /* Capture error */
    while (1){
      LED_init(LED1);
      LED_on(LED1);
    }
  }

  // Initialize connection with the USART 2:
  // For the custom PCB
  while(initConn(USART2) != OK){
    //initialization failed:
    LED_init(LED2);
    LED_on(LED2);
    Delay(DELAY_TIME);
    LED_off(LED2);
  }

  // Initialize the on board accelerometer
  // Set the sensitivity to +/- 2.3g
  accelerometer_init();

  // Register this device with the main host
  char data2[1];
  data2[0] = LCM_MSG_REGISTER_NODE;			// message id for registration
  if(rcmDataSend(USED_ANTENNA, 1, &data2)){
    // success
  }


  // Localization loop
  while(true){

    rcmMsg_DataInfo recdata;
    bool err=rcmDataReceive(&recdata);

    uint8_t i=0;

    if(!err){
      lcmMsg_DoLoc* lcmMsg= (lcmMsg_DoLoc*)&(recdata.data);

      switch(lcmMsg->msgID){
        case LCM_MSG_DOLOCALIZATION:
          {

            uint32_t neighbourIDs[lcmMsg->nUsers + lcmMsg->nAnchors];

            while(i<lcmMsg->nUsers+lcmMsg->nAnchors){
              neighbourIDs[i] = lcmMsg->data[i];
              i++;
            }


            // check if this node can start ranging, if not, see if the node is in the user list
            if(lcmMsg->activeNodeID == RCM_id){

              bConnected = true;

              lcmMsg_LocInfo* locInfo = calloc(1, sizeof(lcmMsg_LocInfo));
              if(lcmMsg->options & LCMFLAG_COOP){
                // perform ranging with the anchors AND the other neighboring users
                PerformRanging(locInfo, (lcmMsg->nUsers+lcmMsg->nAnchors), neighbourIDs, 1);
                locInfo->nAnchors = lcmMsg->nUsers+lcmMsg->nAnchors;
              }else{
                // perform ranging with the anchors only.
                PerformRanging(locInfo, lcmMsg->nAnchors, neighbourIDs+lcmMsg->nUsers, 2);
                locInfo->nAnchors = lcmMsg->nAnchors;
              }


              // perform non-cooperative localization if required
              if(lcmMsg->options & LCMFLAG_ONBOARD_LOCALIZATION){
                int32_t acc_data[3];
                accelerometer_read(acc_data);
                locInfo->acc_x_axis = (float32_t) acc_data[0];
                locInfo->acc_y_axis = (float32_t) acc_data[1];
                locInfo->acc_z_axis = (float32_t) acc_data[2];

                if(!(lcmMsg->options & LCMFLAG_COOP)){
                  if(lcmMsg->options & LCMFLAG_KALMAN) {
                    uint8_t i = 0;
                    // perform Kalman Filtering

                    // Prediction Step
                    kalman_predict(&f_matrix, &g_matrix, &position, &pk, &var_u, &mkmin, &pkmin);
                    // Update new sigmapoints
                    kalman_update_sigmapoints(sigmapoints, mkmin, &pkmin);

                    // Measurement update
                    while(i < lcmMsg->nAnchors && i < NR_ANCHORS) {
                      anchors[i][0] = int2float(lcmMsg->data[i*3 + (lcmMsg->nUsers + lcmMsg->nAnchors)]) / 1000.0;
                      anchors[i][1] = int2float(lcmMsg->data[i*3 + 1 + (lcmMsg->nUsers + lcmMsg->nAnchors)]) / 1000.0;
                      anchors[i][2] = int2float(lcmMsg->data[i*3 + 2 + (lcmMsg->nUsers + lcmMsg->nAnchors)]) / 1000.0;
                      anchors[i][3] = (float32_t) locInfo->data[i].precisionRangeMm / 1000.0;
                      i++;
                    }

                    kalman_measurement_update(&z_matrix, anchors, sigmapoints, &weight_m, &weight_c, &r_matrix, &pkmin, &position, &pk);

                    // Report estimated location
                    locInfo->estim_x = position.pData[0];
                    locInfo->estim_y = position.pData[1];
                    locInfo->estim_z = position.pData[2];

                    // Report variance matrix
                    locInfo->variance[0] = pk.pData[0];
                    locInfo->variance[1] = pk.pData[1];
                    locInfo->variance[2] = pk.pData[7];

                  }else if(lcmMsg->options & LCMFLAG_3D){
                    // perform 3D localization
                    float32_t posEstimate[3] = {0.0f, 0.0f, 0.0f};
                    float32_t anchorsX[lcmMsg->nAnchors];
                    float32_t anchorsY[lcmMsg->nAnchors];
                    float32_t anchorsZ[lcmMsg->nAnchors];
                    uint32_t d[lcmMsg->nAnchors];

                    i=0;

                    // Fill in positions of anchor nodes in anchorsX,Y,Z and range measurement in d
                    while(i<lcmMsg->nAnchors){
                      anchorsX[i] = int2float(lcmMsg->data[i*3 + (lcmMsg->nUsers+lcmMsg->nAnchors)]);
                      anchorsY[i] = int2float(lcmMsg->data[i*3 + 1 + (lcmMsg->nUsers+lcmMsg->nAnchors)]);
                      anchorsZ[i] = int2float(lcmMsg->data[i*3 + 2 + (lcmMsg->nUsers+lcmMsg->nAnchors)]);
                      d[i] = (float)locInfo->data[i].precisionRangeMm;
                      i++;
                    }

                    // Calculate 3D positions and store in locInfo
                    arm_status errorStatus;
                    errorStatus = Calculate3DPosition(lcmMsg->nAnchors, posEstimate, anchorsX, anchorsY, anchorsZ, d);

                    locInfo->estim_x = posEstimate[0]/1000.0f;
                    locInfo->estim_y = posEstimate[1]/1000.0f;
                    locInfo->estim_z = posEstimate[2]/1000.0f;
                  }else {
                    // perform 2D localization
                    float32_t posEstimate[2] = {0.0f, 0.0f};
                    float32_t anchorsX[lcmMsg->nAnchors];
                    float32_t anchorsY[lcmMsg->nAnchors];
                    uint32_t d[lcmMsg->nAnchors];

                    i=0;
                    while(i<lcmMsg->nAnchors){
                      anchorsX[i] = int2float(lcmMsg->data[i*3 + (lcmMsg->nUsers+lcmMsg->nAnchors)]);
                      anchorsY[i] = int2float((float32_t)lcmMsg->data[i*3 + 1 + (lcmMsg->nUsers+lcmMsg->nAnchors)]);
                      d[i] = (float)locInfo->data[i].precisionRangeMm;
                      i++;
                    }

                    arm_status errorStatus;
                    errorStatus = Calculate2DPosition(lcmMsg->nAnchors, posEstimate, anchorsX, anchorsY, d);

                    locInfo->estim_x = posEstimate[0]/1000.0f;
                    locInfo->estim_y = posEstimate[1]/1000.0f;
                    locInfo->estim_z = 0;
                  }
                }else{
                  // perform cooperative localization.
                }
              }


              // send data to central hub
              locInfo->msgID = LCM_MSG_LOC_INFO;
              uint32_t msgSize = sizeof(lcmMsg_LocInfo) - sizeof(lcm_rangeInfo)*(LCM_MAX_NEIGHBOURS-locInfo->nAnchors);
              rcmDataSend(USED_ANTENNA, msgSize, locInfo);
              free(locInfo);

            }else{
              // check that the user is still connected with the central hub (i.e. in the user list)
              i = 0;
              bConnected = false;
              while(i<lcmMsg->nUsers){
                if(neighbourIDs[i] == RCM_id)
                  bConnected = true;
                i++;
              }
            }
          }
          break;
        case LCM_MSG_LOC_INFO:

          // This is not how it should be but ok..

          if(!bConnected){
            // Try to connect with the central hub
            char data[1];
            data[0] = LCM_MSG_REGISTER_NODE;		// message id for registration
            rcmDataSend(USED_ANTENNA, 1, &data);
          }
          // ignore
          break;

        case LCM_MSG_GET_CONNECTION:
          {
            if(!bConnected){
              // Try to connect with the central hub
              char data[1];
              data[0] = LCM_MSG_REGISTER_NODE;		// message id for registration
              rcmDataSend(USED_ANTENNA, 1, &data);
            }
          }
          break;

        default:
          // do nothing with unknown messages
          break;
      }
    }
  }

  // perform cleanup
  closeConn();
  return 0;
}


void PerformRanging(lcmMsg_LocInfo* locInfo, uint8_t nrAnchors, uint32_t* anchorID, uint8_t maxTrials)
{
  //first range with USED_ANTENNA then range with SECOND_ANTENNA for failed points

  rcmMsg_RangeInfo rangeInfo;
  rcmMsg_DataInfo dataInfo;
  uint8_t i=0;
  uint8_t trial=0;
  //first antenna
  while(i<nrAnchors){

    locInfo->data[i].anchorId = anchorID[i];
    trial++;

    // If no success after a number of trials, move to the next neighbour
    if(trial > maxTrials){
      locInfo->data[i].trials = trial;
      trial = 0;
      i++;
      continue;
    }

    // Determine range to a radio. May also get data and scan packets.
    rcmRangeTo(anchorID[i], USED_ANTENNA , 0, NULL, &rangeInfo, &dataInfo);

    if ((rangeInfo.msgType == RCM_RANGE_INFO) && (rangeInfo.rangeStatus == RCM_RANGE_STATUS_SUCCESS))
    {
      locInfo->data[i].precisionRangeMm = rangeInfo.precisionRangeMm;
      locInfo->data[i].precisionRangeErrEst = rangeInfo.precisionRangeErrEst;
      locInfo->data[i].channelRiseTime = rangeInfo.channelRiseTime;
      locInfo->data[i].reqLEDFlags = rangeInfo.reqLEDFlags;
      locInfo->data[i].respLEDFlags = rangeInfo.respLEDFlags;
      locInfo->data[i].timestamp = rangeInfo.timestamp;
      locInfo->data[i].coarseTOFInBins = rangeInfo.coarseTOFInBins;
      locInfo->data[i].trials = trial;

      // range with next neighbour
      trial = 0;
      i++;
    }

  }
}

// configure the random number generator, required for the particle filter.
void RNG_Config(void)
{
  // Enable RNG clock source, note that the RNG uses the AHB2: High speed Advanced Peripheral Bus (APB2)
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

  // RNG Peripheral enable
  RNG_Cmd(ENABLE);
}

float32_t int2float(uint32_t in){
  union{
    uint32_t 	here_write_float;
    float32_t   here_read_float;
  }convert;

  convert.here_write_float = in;

  return convert.here_read_float;
}
