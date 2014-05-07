/*this function will calculate the position using the LMS algorithm.
 * This version fixed the "division by 0 in rare cases"-bug
 */
//includes
#include "arm_math.h"
#include "main.h" //to get the bool-definition
#include "lms.h"
#include "leds.h"

arm_status Calculate3DPosition(uint8_t nrAnchors, float32_t* currPosEst, float32_t* anchorsX, float32_t* anchorsY, float32_t* anchorsZ, uint32_t* d){
  LED_init(LED2);

  // Extract valid (nonzero) measurements to avoid division by zero (in very rare cases)
  uint8_t i;
  uint32_t nrOfValids=0;

  for(i=0; i<nrAnchors;i++){
    if(d[i] > 0.0001) nrOfValids++;
  }

  float32_t valid_anchorsX[nrOfValids];
  float32_t valid_anchorsY[nrOfValids];
  float32_t valid_anchorsZ[nrOfValids];
  float32_t valid_d[nrOfValids];

  uint8_t vi=0; //validity-index
  for(i=0; i<nrAnchors; i++){
    if(d[i] > 0.0001){
      valid_anchorsX[vi] = anchorsX[i];
      valid_anchorsY[vi] = anchorsY[i];
      valid_anchorsZ[vi] = anchorsZ[i];
      valid_d[vi] = d[i]*d[i];
      vi++;
    }
  }

  if(nrOfValids >= 4){ // 4 or more valid distance measurements -> LMS can be applied
    float32_t anchorsArray[3*(nrOfValids-1)];
    

    for(i=0; i<nrOfValids-1;i++){
      anchorsArray[3*i] = valid_anchorsX[i];
      anchorsArray[3*i+1] = valid_anchorsY[i];
      anchorsArray[3*i+2] = valid_anchorsZ[i];
    }

    arm_matrix_instance_f32 anchorsMat;
    arm_matrix_instance_f32 x_N;
    float32_t x_N_array[(nrOfValids-1)*3];

    arm_mat_init_f32(&anchorsMat,nrOfValids-1,3,(float32_t *)anchorsArray);

    for(i=0;i<nrOfValids-1;i++){
        x_N_array[3*i] = valid_anchorsX[nrOfValids-3];
        x_N_array[3*i+1] = valid_anchorsY[nrOfValids-2];
        x_N_array[3*i+2] = valid_anchorsZ[nrOfValids-1];
    }

    arm_mat_init_f32(&x_N,nrOfValids-1,3,(float32_t *)x_N_array);

    arm_matrix_instance_f32 A;
    arm_matrix_instance_f32 Ainv;
    arm_matrix_instance_f32 x;
    float32_t A_array[3*(nrOfValids-1)];
    float32_t Ainv_array[3*(nrOfValids-1)];
    float32_t x_array[3*1];

    arm_mat_init_f32(&A,nrOfValids-1,3,(float32_t *)A_array);
    arm_mat_init_f32(&Ainv,nrOfValids-1,3,(float32_t *)Ainv_array);
    
    arm_mat_sub_f32(&x_N,&anchorsMat,&A);
    arm_mat_scale_f32(&A,-2.0f,&A);

    arm_matrix_instance_f32 B;
    float32_t B_array[3];

    for(i=0; i<3;i++){
      B_array[i] = valid_d[nrOfValids-1] - valid_d[i] 
             - (x_N.pData[3*i]*x_N.pData[3*i] + x_N.pData[3*i+1]*x_N.pData[3*i+1] + x_N.pData[3*i+2]*x_N.pData[3*i+2])
             + (anchorsMat.pData[3*i]*anchorsMat.pData[3*i] + anchorsMat.pData[3*i+1]*anchorsMat.pData[3*i+1] + anchorsMat.pData[3*i+2]*anchorsMat.pData[3*i+2]);
    }

    arm_mat_init_f32(&B,3,1,(float32_t *)B_array);
    
    arm_status ReusedStatus;

    ReusedStatus = arm_mat_inverse_f32(&A,&Ainv);

   // if(ReusedStatus != ARM_MATH_SUCCESS)
     // return ReusedStatus;

    arm_mat_init_f32(&x,3,1,x_array);

    ReusedStatus=arm_mat_mult_f32(&Ainv,&B,&x);

    currPosEst[0] = x.pData[0];
    currPosEst[1] = x.pData[1];
    currPosEst[2] = x.pData[2];

    return ReusedStatus;
    


  // uint8_t nrOfValids=0;

  // // Extract valid (nonzero) measurements to avoid division by zero (in very rare cases)

  // for(i=0; i<nrAnchors;i++){
  //   if(d[i] != 0) nrOfValids++;
  // }

  // float32_t valid_anchorsX[nrOfValids];
  // float32_t valid_anchorsY[nrOfValids];
  // float32_t valid_anchorsZ[nrOfValids];
  // float32_t valid_d[nrOfValids];

  // uint8_t vi=0; //validity-index
  // for(i=0; i<nrAnchors; i++){
  //   if(d[i] != 0){
  //     valid_anchorsX[vi] = anchorsX[i];
  //     valid_anchorsY[vi] = anchorsY[i];
  //     valid_anchorsZ[vi] = anchorsZ[i];
  //     valid_d[vi] = d[i];
  //     vi++;
  //   }
  // }

  // if(nrOfValids >= 4){ // 4 or more valid distance measurements -> LMS can be applied
  //   float32_t b_array[nrOfValids-1];
  //   float32_t A_array[3*(nrOfValids-1)];
  //   float32_t AT_array[3*(nrOfValids-1)];

  //   uint8_t i;
  //   for(i=0; i<nrOfValids-1; i++){
  //     b_array[i] = (valid_d[i]*valid_d[i] - valid_d[nrOfValids-1]*valid_d[nrOfValids-1])
  //       - (valid_anchorsX[i]*valid_anchorsX[i]) + (valid_anchorsX[nrOfValids-1]*valid_anchorsX[nrOfValids-1])
  //       - (valid_anchorsY[i]*valid_anchorsY[i]) + (valid_anchorsY[nrOfValids-1]*valid_anchorsY[nrOfValids-1])
  //       - (valid_anchorsZ[i]*valid_anchorsZ[i]) + (valid_anchorsZ[nrOfValids-1]*valid_anchorsZ[nrOfValids-1]);
  //     A_array[i*2] = -2*valid_anchorsX[i] + 2*valid_anchorsX[nrOfValids-1];
  //     A_array[i*2 + 1] = -2*valid_anchorsY[i] + 2*valid_anchorsY[nrOfValids-1];
  //     A_array[i*2 + 2] = -2*valid_anchorsZ[i] + 2*valid_anchorsZ[nrOfValids-1];
  //   }

  //   arm_matrix_instance_f32 b = {nrOfValids-1, 1, (float32_t *)b_array};
  //   arm_matrix_instance_f32 A = {nrOfValids-1, 3, (float32_t *)A_array};
  //   arm_matrix_instance_f32 AT = {3, nrOfValids-1, (float32_t *)AT_array};

  //   arm_status TransStatus = arm_mat_trans_f32(&A,&AT);
  //   if(TransStatus != ARM_MATH_SUCCESS){
  //     return TransStatus;
  //   }

  //   float32_t ATA_array[9]={0};
  //   arm_matrix_instance_f32 ATA = {3, 3, (float32_t *)ATA_array};

  //   arm_status ReusedStatus = arm_mat_mult_f32(&AT, &A, &ATA);
  //   if (ReusedStatus != ARM_MATH_SUCCESS){
  //     return ReusedStatus;
  //   }

  //   float32_t ATAinv_array[9]={0};
  //   float32_t det = (ATA.pData[0]*ATA.pData[4]*ATA.pData[8] + ATA.pData[1]*ATA.pData[5]*ATA.pData[6]
  //       + ATA.pData[2]*ATA.pData[3]*ATA.pData[7] - ATA.pData[2]*ATA.pData[4]*ATA.pData[6]
  //       - ATA.pData[1]*ATA.pData[3]*ATA.pData[8] - ATA.pData[0]*ATA.pData[5]*ATA.pData[7]);

  //   ATAinv_array[0] = (ATA.pData[8]*ATA.pData[4] - ATA.pData[7]*ATA.pData[5])/det;
  //   ATAinv_array[1] = (ATA.pData[7]*ATA.pData[2] - ATA.pData[1]*ATA.pData[8])/det;
  //   ATAinv_array[2] = (ATA.pData[1]*ATA.pData[5] - ATA.pData[4]*ATA.pData[2])/det;
  //   ATAinv_array[3] = (ATA.pData[5]*ATA.pData[6] - ATA.pData[8]*ATA.pData[3])/det;
  //   ATAinv_array[4] = (ATA.pData[0]*ATA.pData[8] - ATA.pData[6]*ATA.pData[2])/det;
  //   ATAinv_array[5] = (ATA.pData[2]*ATA.pData[3] - ATA.pData[0]*ATA.pData[5])/det;
  //   ATAinv_array[6] = (ATA.pData[7]*ATA.pData[3] - ATA.pData[6]*ATA.pData[4])/det;
  //   ATAinv_array[7] = (ATA.pData[1]*ATA.pData[6] - ATA.pData[7]*ATA.pData[0])/det;
  //   ATAinv_array[8] = (ATA.pData[4]*ATA.pData[0] - ATA.pData[1]*ATA.pData[3])/det;
  //   arm_matrix_instance_f32 ATAinv = {3, 3, (float32_t *)ATAinv_array};

  //   float32_t MP_array[3*(nrOfValids-1)];
  //   arm_matrix_instance_f32 MoorePenrose = {3, nrOfValids-1, (float32_t *)MP_array};
  //   ReusedStatus = arm_mat_mult_f32(&ATAinv, &AT, &MoorePenrose);

  //   if (ReusedStatus != ARM_MATH_SUCCESS){
  //     return ReusedStatus;
  //   }
  //   arm_matrix_instance_f32 currPosEstMatrix = {3, 1, (float32_t *)currPosEst};
  //   ReusedStatus = arm_mat_mult_f32(&MoorePenrose, &b, &currPosEstMatrix);

  //   if (ReusedStatus != ARM_MATH_SUCCESS){
  //     return ReusedStatus;
  //   }

  //   return ARM_MATH_SUCCESS; //no errors occurred

  }else{
    /* Less than 4 valid measurements -> normal LMS impossible!
     * however: the previous known position is still available in currPosEst!
     */

    float32_t intersects[6]={0};
    uint32_t d1sq;
    uint32_t d2sq;

    if(nrOfValids <= 2 ){
      //do nothing: no valid measurements => currPosEst is not updated
      return ARM_MATH_SUCCESS;
    }
    else if(nrOfValids == 3){
      float32_t data[12] = {valid_anchorsX[0], valid_anchorsY[0], valid_anchorsZ[0],
        valid_anchorsX[1], valid_anchorsY[1], valid_anchorsZ[1],
        valid_anchorsX[2], valid_anchorsY[2], valid_anchorsZ[2],
        valid_d[0], valid_d[1], valid_d[2]};
      uint8_t nrOfIntersects = get3DIntersections(data, intersects);

      if(nrOfIntersects==0){ //do not change currPosEst (can still be improved)
        return ARM_MATH_SUCCESS;
      }
      else if(nrOfIntersects==1){
        currPosEst[0]=intersects[0];
        currPosEst[1]=intersects[1];
        currPosEst[2]=intersects[2];
        return ARM_MATH_SUCCESS;
      }
    }
    // now compare intersect points, for case "3 valids, 2 intersects" and case "2 valids"
    d1sq = (intersects[0]-currPosEst[0])*(intersects[0]-currPosEst[0])
      + (intersects[1]-currPosEst[1])*(intersects[1]-currPosEst[1])
      + (intersects[2]-currPosEst[2])*(intersects[2]-currPosEst[2]);
    d2sq = (intersects[3]-currPosEst[0])*(intersects[3]-currPosEst[0])
      + (intersects[4]-currPosEst[1])*(intersects[4]-currPosEst[1])
      + (intersects[5]-currPosEst[2])*(intersects[5]-currPosEst[2]);
    if(d1sq <= d2sq){
      currPosEst[0] = intersects[0];
      currPosEst[1] = intersects[1];
      currPosEst[2] = intersects[2];
    }
    else{
      currPosEst[0] = intersects[3];
      currPosEst[1] = intersects[4];
      currPosEst[2] = intersects[5];
    }
    return ARM_MATH_SUCCESS;
  }
}

uint8_t get3DIntersections(float32_t* data, float32_t* results){
  float32_t cx1=data[0]; float32_t cy1=data[1]; float32_t cz1=data[2];
  float32_t cx2=data[3]; float32_t cy2=data[4]; float32_t cz2=data[5];
  float32_t cx3=data[6]; float32_t cy3=data[7]; float32_t cz3=data[8];
  float32_t R1=data[9]; float32_t R2=data[10]; float32_t R3=data[11];
  float32_t e_x[3], e_y[3], e_z[3], i, j, ie_x[3];

  // Calculate intersections between 3 spheres using trilateration

  // temp1 = P2 - P1
  // e_x = temp1/norm(temp1)
  float32_t temp1[3] = {cx2 - cx1, cy2 - cy1, cz2 - cz1};
  float32_t norm12 = temp1[0]*temp1[0] + temp1[1]*temp1[1] + temp1[2]*temp1[2];
  arm_sqrt_f32(norm12,&norm12);
  arm_scale_f32(temp1,1.0f/norm12,e_x,3);

  // temp2 = P3 - P1
  float32_t temp2[3] = {cx3 - cx1, cy3 - cy1, cz3 - cz1};

  // i = dot(temp2,e_x)
  arm_dot_prod_f32(e_x,temp2,3,&i);

  // temp3 = temp2 - i*e_x
  arm_scale_f32(e_x,i,ie_x,3);
  float32_t temp3[3];
  arm_sub_f32(temp2,ie_x,temp3,3);

  // e_y = temp3/norm(temp3)
  float32_t norm13 = temp3[0]*temp3[0] + temp3[1]*temp3[1] + temp3[2]*temp3[2];
  arm_sqrt_f32(norm13,&norm13);
  arm_scale_f32(temp3,1.0f/norm13,e_y,3);

  // e_z = cross(e_x,e_y)
  e_z[0] = e_x[1]*e_y[2] - e_x[2]*e_y[1];
  e_z[1] = e_x[2]*e_y[0] - e_x[0]*e_y[2];
  e_z[2] = e_x[0]*e_y[1] - e_x[1]*e_y[0];

  // j = dot(e_y, temp2)
  arm_dot_prod_f32(e_y,temp2,3,&j);

  // d = norm12
  // x = (r1*r1 - r2*r2 + d*d) / (2*d)
  float32_t x = (R1*R1 - R2*R2 + norm12*norm12) / (2*norm12);

  // y = (r1*r1 - r3*r3 -2*i*x + i*i + j*j) / (2*j)
  float32_t y = (R1*R1 - R3*R3 - 2*i*x + i*i + j*j) / (2*j);

  // temp4 = r1*r1 - x*x - y*y
  float32_t temp4 = R1*R1 - x*x - y*y;

  if(temp4<0){
    return 0;
  }

  // z = sqrt(temp4)
  float32_t z;
  arm_sqrt_f32(temp4,&z);

  arm_scale_f32(e_x,x,e_x,3);
  arm_scale_f32(e_y,y,e_y,3);
  arm_scale_f32(e_z,z,e_z,3);

  results[0] = cx1 + e_x[0] + e_y[0] + e_z[0];
  results[1] = cy1 + e_x[1] + e_y[1] + e_z[1];
  results[2] = cz1 + e_x[2] + e_y[2] + e_z[2];
  results[3] = cx1 + e_x[0] + e_y[0] - e_z[0];
  results[4] = cy1 + e_x[1] + e_y[1] - e_z[1];
  results[5] = cz1 + e_x[2] + e_y[2] - e_z[2];

  if((results[0]==results[3]) && (results[1]==results[4]) && (results[2] == results[5])) return 1;
  else return 2;
}

arm_status Calculate2DPosition(uint8_t nrAnchors, float32_t* currPosEst, float32_t* anchorsX, float32_t* anchorsY, uint32_t* d){

  uint8_t j;
  uint8_t nrOfValids =0;
  for (j=0; j<nrAnchors; j++){
    if (d[j] != 0) nrOfValids++;
  }

  float32_t valid_anchorsX[nrOfValids];
  float32_t valid_anchorsY[nrOfValids];
  float32_t valid_d[nrOfValids];

  uint8_t vi=0; //vi is short for validity-index: the index in the valid array
  for (j=0; j<nrAnchors; j++){
    if(d[j] != 0){
      valid_anchorsX[vi] = anchorsX[j];
      valid_anchorsY[vi] = anchorsY[j];
      valid_d[vi] = d[j];
      vi++;
    }
  } //finished extracting valid measurements from the mixture that this function received

  if(nrOfValids >=3){ //normal LMS can be applied

    float32_t b_array[nrOfValids-1];
    float32_t A_array[2*(nrOfValids-1)];
    float32_t AT_array[2*(nrOfValids-1)];

    uint8_t i;
    for (i=0;i<nrOfValids-1;i++){
      b_array[i] = (valid_d[i]*valid_d[i] - valid_d[nrOfValids-1]*valid_d[nrOfValids-1])
        - (valid_anchorsX[i]*valid_anchorsX[i]) + (valid_anchorsX[nrOfValids-1]*valid_anchorsX[nrOfValids-1])
        - (valid_anchorsY[i]*valid_anchorsY[i]) + (valid_anchorsY[nrOfValids-1]*valid_anchorsY[nrOfValids-1]);

      A_array[i*2] = -2*valid_anchorsX[i] + 2*valid_anchorsX[nrOfValids-1];
      A_array[i*2 +1] = -2*valid_anchorsY[i] + 2*valid_anchorsY[nrOfValids-1];
      AT_array[i] = -2*valid_anchorsX[i] + 2*valid_anchorsX[nrOfValids-1]; //Deze twee lijnen zijn...
      AT_array[i+nrOfValids-1] = -2*valid_anchorsY[i] + 2*valid_anchorsY[nrOfValids-1]; //een manuele transpose.
    }
    //manuele transpose beter dan arm_matrix functie gaan oproepen,
    //data [nu] gekend

    arm_matrix_instance_f32 b = {nrOfValids-1, 1, (float32_t *)b_array};
    arm_matrix_instance_f32 A = {nrOfValids-1, 2, (float32_t *)A_array};
    arm_matrix_instance_f32 AT = {2, nrOfValids-1, (float32_t *)AT_array};//static initializing,  alt: arm_mat_init_f32()

    float32_t ATA_array[4];
    arm_matrix_instance_f32 ATA = {2, 2, (float32_t *)ATA_array};
    arm_status ReusedStatus = arm_mat_mult_f32(&AT, &A, &ATA);

    if (ReusedStatus != ARM_MATH_SUCCESS){
      return ReusedStatus;
    }

    float32_t ATAinv_array[4];
    float32_t det = ATA_array[0]*ATA_array[3] - ATA_array[1]*ATA_array[2];
    ATAinv_array[0] = ATA_array[3]/det;
    ATAinv_array[3] = ATA_array[0]/det;
    ATAinv_array[1] = -ATA_array[1]/det;
    ATAinv_array[2] = -ATA_array[2]/det;
    arm_matrix_instance_f32 ATAinv = {2, 2, (float32_t *)ATAinv_array};
    /*ReusedStatus = arm_mat_inverse_f32(&ATA, &ATAinv);

      if (ReusedStatus != ARM_MATH_SUCCESS){
      return ReusedStatus;
      }*/

    float32_t MP_array[2*(nrOfValids-1)];
    arm_matrix_instance_f32 MoorePenrose = {2, nrOfValids-1, (float32_t *)MP_array};
    ReusedStatus = arm_mat_mult_f32(&ATAinv, &AT, &MoorePenrose);

    if (ReusedStatus != ARM_MATH_SUCCESS){
      return ReusedStatus;
    }
    arm_matrix_instance_f32 currPosEstMatrix = {2, 1, (float32_t *)currPosEst};
    ReusedStatus = arm_mat_mult_f32(&MoorePenrose, &b, &currPosEstMatrix);

    if (ReusedStatus != ARM_MATH_SUCCESS){
      return ReusedStatus;
    }

    return ARM_MATH_SUCCESS; //if the program reaches here, no errors occurred
  }
  else{
    /*this means: less than 3 valid measurements -> normal LMS impossible!
     * however: the previous known position is still available in currPosEst!
     */

    float32_t intersects[4];
    uint32_t d1sq;
    uint32_t d2sq;

    if(nrOfValids == 0){
      //do nothing: no valid measurements => currPosEst is not updated
      return ARM_MATH_SUCCESS;
    }
    else if(nrOfValids == 2){
      uint32_t data[6] = {valid_anchorsX[0], valid_anchorsY[0], valid_anchorsX[1],
        valid_anchorsY[1], valid_d[0], valid_d[1]};
      uint8_t nrOfIntersects = getIntersections(data, intersects);

      if(nrOfIntersects==0){ //do not change currPosEst (can still be improved, maybe)
        return ARM_MATH_SUCCESS;
      }
      else if(nrOfIntersects==1){
        currPosEst[0]=intersects[0];
        currPosEst[1]=intersects[1];
        return ARM_MATH_SUCCESS;
      }
      else{//2 intersect points, let's find the one closest to the previous position
        //this piece of code has been moved, because the 'else' below needs the exact same
      }
    }
    else {//nrOfValids =1, choose point on circle closest to old position
      //y = y1 + (y2-y1)/(x2-x1) *(x-x1)
      float32_t rico = (currPosEst[1]-valid_anchorsY[0])/(currPosEst[0]-valid_anchorsX[0]);
      //a*x^2 + b*x +c =0
      float32_t ricosqp1 = rico*rico +1; //ricosqp1 means rico squared plus one
      float32_t a = ricosqp1;
      float32_t b = -2*valid_anchorsX[0]*ricosqp1;
      float32_t c = valid_anchorsX[0]*valid_anchorsX[0]*ricosqp1-valid_d[0]*valid_d[0];
      float32_t D = b*b-4*a*c; //point lies on line between oldPosition and anchorpoint: guaranteed 2 intersections, D>0
      float32_t sqrtD=0;
      arm_sqrt_f32 (D, &sqrtD);

      intersects[0] = (-b + sqrtD)/(2*a);
      intersects[1] = valid_anchorsY[0] + rico*(intersects[0]-valid_anchorsX[0]);
      intersects[2] = (-b - sqrtD)/(2*a);
      intersects[3] = valid_anchorsY[0] + rico*(intersects[2]-valid_anchorsX[0]);
    }

    // now compare intersect points, for case "2 valids, 2 intersects" and case "1 valid"
    d1sq = (intersects[0]-currPosEst[0])*(intersects[0]-currPosEst[0])
      + (intersects[1]-currPosEst[1])*(intersects[1]-currPosEst[1]);
    d2sq = (intersects[2]-currPosEst[0])*(intersects[2]-currPosEst[0])
      + (intersects[3]-currPosEst[1])*(intersects[3]-currPosEst[1]);
    if(d1sq <= d2sq){
      currPosEst[0] = intersects[0];
      currPosEst[1] = intersects[1];
    }
    else{
      currPosEst[0] = intersects[2];
      currPosEst[1] = intersects[3];
    }
    return ARM_MATH_SUCCESS;
  }
}

uint8_t getIntersections(uint32_t* data, float32_t* results){
  uint32_t cx1=data[0]; uint32_t cy1=data[1];
  uint32_t cx2=data[2]; uint32_t cy2=data[3];
  uint32_t R1=data[4]; uint32_t R2=data[5];
  float32_t cte, B, a, b, c, D, sqrtD;

  if ((cx1 - cx2) >= (cy1 - cy2)){ //more stable to divide by the x-difference
    //x = cte - B*y

    cte = (R2*R2 - R1*R1 - cx2*cx2 + cx1*cx1 - cy2*cy2 + cy1*cy1)/(-2*cx2 + 2*cx1);
    B = (-cy2+cy1)/(-cx2+cx1);

    // a*y^2 + b*y + c = 0
    a = B*B +1;
    b = -2*B*(cte-cx2)-2*cy2;
    c = cy2*cy2 + (cte-cx2)*(cte-cx2)-R2*R2;
    D = b*b-4*a*c;


    if(D<0)
      return 0;
    else if (D==0){
      results[1] = -b/(2*a);
      results[0] = cte-B*results[1];
      return 1;
    }
    else {// D>0, 2 intersections
      sqrtD=0;
      arm_sqrt_f32(D, &sqrtD);//returns arm_status, but argument D is always positive anyway
      results[1] = (-b + sqrtD)/(2*a); //y1
      results[0] = cte-B*results[1]; //x1
      results[3] = (-b - sqrtD)/(2*a); //y2
      results[2] = cte-B*results[3]; //x2
      return 2;
    }
  }
  else{ //division by y-difference, analoguous
    //y = cte - B*x

    cte = (R2*R2 - R1*R1 - cx2*cx2 + cx1*cx1 - cy2*cy2 + cy1*cy1)/(-2*cy2 + 2*cy1);
    B = (-cx2+cx1)/(-cy2+cy1);

    // a*x^2 + b*x + c = 0
    a = B*B +1;
    b = -2*B*(cte-cy2)-2*cx2;
    c = cx2*cx2 + (cte-cy2)*(cte-cy2)-R2*R2;
    D = b*b-4*a*c;

    if(D<0)
      return 0;
    else if (D==0){
      results[0] = -b/(2*a);
      results[1] = cte-B*results[0];
      return 1;
    }
    else {// D>0, 2 intersections
      sqrtD=0;
      arm_sqrt_f32(D, &sqrtD);//returns arm_status, but argument D is always positive anyway
      results[0] = (-b + sqrtD)/(2*a); //x1
      results[1] = cte-B*results[0]; //y1
      results[2] = (-b - sqrtD)/(2*a); //x2
      results[3] = cte-B*results[2]; //y2
      return 2;
    }
  }
}
