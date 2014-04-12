/**
 * kalman.c
 * Functions for Unscented Kalman Filtering
 */

#include "kalman.h"
#include <stdlib.h>
#include <math.h>

arm_matrix_instance_f32 cholesky_decomp(arm_matrix_instance_f32 matrix) {
  int8_t i,j,k;
  arm_matrix_instance_f32 output;
  float32_t* data = (float32_t*) calloc(matrix.numRows * matrix.numCols, sizeof(float32_t));

  for (i = 0; i < matrix.numRows; i++) {
    for (j = 0; j < (i+1); j++) {
      float32_t s = 0;
      for (k = 0; k < j; k++) {
        s+= data[i * matrix.numRows + k] * data[j * matrix.numRows + k];
        if (i==j) {
          float32_t root;
          arm_sqrt_f32(matrix.pData[i * matrix.numRows + i] - s, &root);
          data[i * matrix.numRows + j] = root;
        }else {
          data[i * matrix.numRows + j] = (1.0 / data[j * matrix.numRows + j] * (matrix.pData[i * matrix.numRows + j] - s));
        }
      }
    }
  }

  // Create the lower-diagonal matrix
  arm_mat_init_f32(&output, matrix.numRows, matrix.numCols, data);
  return output;
}
