/**
 * kalman.c
 * Functions for Unscented Kalman Filtering
 */

#include "kalman.h"
#include <stdlib.h>
#include <math.h>

void kalman_init_weight_factors(arm_matrix_instance_f32* weight_m, arm_matrix_instance_f32* weight_c) {
  float32_t wc[NR_SIGMAPOINTS];
  float32_t wm[NR_SIGMAPOINTS];
  uint8_t i;

  // Set first indices
  wc[0] = WEIGHT_C0;
  wm[0] = WEIGHT_M0;

  for (i = 1; i < NR_SIGMAPOINTS; i++) {
    wc[i] = WEIGHTS;
    wm[i] = WEIGHTS;
  }

  arm_mat_init_f32(weight_m, 1, NR_SIGMAPOINTS, wm);
  arm_mat_init_f32(weight_c, 1, NR_SIGMAPOINTS, wc);
}

void kalman_init_position(position_t* position) {
  float32_t pos[DIMENSIONS];
  uint8_t i;

  for(i = 0; i < DIMENSIONS; i++) {
    pos[0] = 0;
  }

  arm_mat_init_f32(position, DIMENSIONS, 1, pos);
}

void kalman_init_sigmapoints(position_t* sigmapoints) {
  float32_t vectors[NR_SIGMAPOINTS][DIMENSIONS];
  uint8_t i,j;

  for (i = 0; i < NR_SIGMAPOINTS; i++) {
    for (j = 0; j < DIMENSIONS; j++) {
      vectors[i][j] = 0;
    }
    arm_mat_init_f32(&sigmapoints[i], DIMENSIONS, 1, vectors[i]);
  }
}

void cholesky_decomp(arm_matrix_instance_f32 matrix, arm_matrix_instance_f32* output) {
  int8_t i,j,k;

  for (i = 0; i < matrix.numRows; i++) {
    for (j = 0; j < (i+1); j++) {
      float32_t s = 0;
      for (k = 0; k < j; k++) {
        s+= output->pData[i * matrix.numRows + k] * output->pData[j * matrix.numRows + k];
        if (i==j) {
          float32_t root;
          arm_sqrt_f32(matrix.pData[i * matrix.numRows + i] - s, &root);
          output->pData[i * matrix.numRows + j] = root;
        }else {
          output->pData[i * matrix.numRows + j] = (1.0 / output->pData[j * matrix.numRows + j] * (matrix.pData[i * matrix.numRows + j] - s));
        }
      }
    }
  }

  return;
}
