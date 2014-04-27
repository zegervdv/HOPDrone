/**
 * kalman.c
 * Functions for Unscented Kalman Filtering
 */

#include "kalman.h"
#include <stdlib.h>
#include <math.h>

void kalman_init_weight_factors(float32_t* weight_m, float32_t* weight_c) {
  uint8_t i;

  // Set first indices
  weight_c[0] = WEIGHT_C0;
  weight_m[0] = WEIGHT_M0;

  for (i = 1; i < NR_SIGMAPOINTS; i++) {
    weight_c[i] = WEIGHTS;
    weight_m[i] = WEIGHTS;
  }
}

void kalman_init_position(float32_t* position) {
  uint8_t i;

  for(i = 0; i < DIMENSIONS; i++) {
    position[0] = 0;
  }
}

void kalman_init_sigmapoints(float32_t* sigmapoints) {
  uint8_t i;

  for (i = 0; i < DIMENSIONS; i++) {
    sigmapoints[i] = 0;
  }
}

void kalman_init_f_matrix(float32_t* f_matrix) {
  uint8_t i,j;

  for (i = 0; i < DIMENSIONS; i++) {
    for (j = 0; j < DIMENSIONS; j++) {
      if (i == j)
        f_matrix[i*DIMENSIONS + j] = 1;
      else {
        if (j == i + DIMENSIONS/2)
          f_matrix[i*DIMENSIONS + j] = DELTAT;
        else
          f_matrix[i*DIMENSIONS + j] = 0;
      }
    }
  }
}

void kalman_init_g_matrix(float32_t* g_matrix) {
  uint8_t i,j;

  for (i = 0; i < DIMENSIONS/2; i++) {
    for (j = 0; j < DIMENSIONS/2; j++) {
      if (i == j) {
        g_matrix[i*DIMENSIONS/2 + j] = (DELTAT*DELTAT)/2.0;
        g_matrix[(i + DIMENSIONS/2)*DIMENSIONS/2 + j] = DELTAT;
      } else
        g_matrix[i*DIMENSIONS + j] = 0;
    }
  }
}

void kalman_init_dimensional_matrix(float32_t* matrix) {
  uint8_t i;

  for (i = 0; i < DIMENSIONS*DIMENSIONS; i++)
    matrix[i] = 0;
}

void kalman_init_variances(float32_t* variance_u, float32_t* r_matrix, uint8_t nr_of_anchors) {
  kalman_eye_matrix(variance_u, DIMENSIONS/2, PREDICTION_VAR);
  kalman_eye_matrix(r_matrix, nr_of_anchors, STD_MEASUREMENT);
}

void kalman_predict(arm_matrix_instance_f32* f_matrix, arm_matrix_instance_f32* g_matrix, position_t* prev_position, arm_matrix_instance_f32* variance, arm_matrix_instance_f32* var_u, arm_matrix_instance_f32* mkmin, arm_matrix_instance_f32* pkmin) {
  float32_t interm_f_data[DIMENSIONS*DIMENSIONS], interm_g_data[DIMENSIONS*DIMENSIONS];
  float32_t interm_f_data2[DIMENSIONS*DIMENSIONS], interm_g_data2[DIMENSIONS*DIMENSIONS];
  float32_t f_matrix_transposed_data[DIMENSIONS*DIMENSIONS], g_matrix_transposed_data[DIMENSIONS*DIMENSIONS];
  arm_matrix_instance_f32 interm_f, interm_g, interm_f2, interm_g2, f_matrix_transposed, g_matrix_transposed;

  // Initialize temporary matrices
  arm_mat_init_f32(&interm_f, DIMENSIONS, DIMENSIONS, interm_f_data);
  arm_mat_init_f32(&interm_g, DIMENSIONS, DIMENSIONS, interm_g_data);
  arm_mat_init_f32(&interm_f2, DIMENSIONS, DIMENSIONS, interm_f_data2);
  arm_mat_init_f32(&interm_g2, DIMENSIONS, DIMENSIONS, interm_g_data2);
  arm_mat_init_f32(&f_matrix_transposed, DIMENSIONS, DIMENSIONS, f_matrix_transposed_data);
  arm_mat_init_f32(&g_matrix_transposed, DIMENSIONS, DIMENSIONS, g_matrix_transposed_data);

  // Calculate mkmin
  arm_mat_mult_f32(f_matrix, prev_position, mkmin);

  // Calculate first intermediate results
  arm_mat_mult_f32(f_matrix, variance, &interm_f);
  /* arm_mat_mult_f32(g_matrix, var_u, &interm_g); */

  // Transpose F and G
  arm_mat_trans_f32(f_matrix, &f_matrix_transposed);
  arm_mat_trans_f32(f_matrix, &f_matrix_transposed);

  // Calculate second intermediate results
  arm_mat_mult_f32(&interm_f, &f_matrix_transposed, &interm_f2);
  arm_mat_mult_f32(&interm_g, &g_matrix_transposed, &interm_g2);

  // Sum up
  arm_mat_add_f32(&interm_f2, &interm_g2, pkmin);
}

void kalman_update_sigmapoints(position_t* sigmapoints, position_t mkmin, arm_matrix_instance_f32* pkmin) {
  uint8_t i,j;
  arm_matrix_instance_f32 root;
  float32_t root_data[DIMENSIONS * DIMENSIONS];

  arm_mat_init_f32(&root, DIMENSIONS, DIMENSIONS, root_data);

  // Calculate the root of the variance matrix using Cholesky Decomposition
  cholesky_decomp(pkmin, &root);

  arm_mat_scale_f32(&root, DIMENSIONS + KAPPA, &root);

  // First sigmapoint is mkmin
  for(i = 0; i < DIMENSIONS; i++) {
    sigmapoints[0].pData[i] = mkmin.pData[i];
  }

  for(i = 0; i < DIMENSIONS; i++) {
    for(j = 0; j < DIMENSIONS; j++) {
      sigmapoints[i + 1].pData[j] = mkmin.pData[j] + root.pData[i + (j * DIMENSIONS)];
    }

    for(j = 0; j < DIMENSIONS; j++) {
      sigmapoints[i + DIMENSIONS + 1].pData[j] = mkmin.pData[j] - root.pData[i + (j * DIMENSIONS)];
    }
  }
}

void cholesky_decomp(arm_matrix_instance_f32* matrix, arm_matrix_instance_f32* output) {
  int8_t i,j,k;

  for (i = 0; i < matrix->numRows; i++) {
    for (j = 0; j < (i+1); j++) {
      float32_t s = 0;
      for (k = 0; k < j; k++) {
        s+= output->pData[i * matrix->numRows + k] * output->pData[j * matrix->numRows + k];
        if (i==j) {
          float32_t root;
          arm_sqrt_f32(matrix->pData[i * matrix->numRows + i] - s, &root);
          output->pData[i * matrix->numRows + j] = root;
        }else {
          output->pData[i * matrix->numRows + j] = (1.0 / output->pData[j * matrix->numRows + j] * (matrix->pData[i * matrix->numRows + j] - s));
        }
      }
    }
  }

  return;
}

void kalman_eye_matrix(float32_t* array, uint8_t size, float32_t value) {
  uint8_t i,j;

  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      if (i == j)
        array[i * size + j] = value;
      else
        array[i * size + j] = 0;
    }
  }
}
