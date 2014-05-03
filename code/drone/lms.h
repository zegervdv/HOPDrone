/**
 *****************************************************************************
 **
 **  File        : lms.h
 **
 **  Abstract    : Calculates 2D and 3D position using LMS. 
 **
 **  Functions   : Calculate2DPosition, Calculate3DPosition, 
 ** 			   GetIntersections, Get3DIntersections
 **
 **  Authors     : Thomas Deckmyn (added 3D localization)
 **  
 *****************************************************************************
 */

#include "arm_math.h"

arm_status Calculate2DPosition(uint8_t nrAnchors, float32_t* currPosEst, float32_t* anchorsX, float32_t* anchorsY, uint32_t* d);
arm_status Calculate3DPosition(uint8_t nrAnchors, float32_t* currPosEst, float32_t* anchorsX, float32_t* anchorsY, float32_t* anchorsZ, uint32_t* d);
uint8_t getIntersections(uint32_t* data, float32_t* results);
uint8_t get3DIntersections(float32_t* data, float32_t* results);

