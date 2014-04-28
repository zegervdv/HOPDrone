#include "arm_math.h"

arm_status Calculate2DPosition(uint8_t nrAnchors, float32_t* currPosEst, float32_t* anchorsX, float32_t* anchorsY, uint32_t* d);
arm_status Calculate3DPosition(uint8_t nrAnchors, float32_t* currPosEst, float32_t* anchorsX, float32_t* anchorsY, float32_t* anchorsZ, uint32_t* d)
uint8_t getIntersections(uint32_t* data, float32_t* results);

