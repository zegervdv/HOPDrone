/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include <sys/errno.h>
#include "system.h"
#include "kalman.h"

#define NR_ANCHORS 4

void Delay(__IO uint32_t nTick);

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

int main(void)
{
  uint8_t i;
  float32_t weight_c_data[NR_SIGMAPOINTS];
  float32_t weight_m_data[NR_SIGMAPOINTS];
  float32_t position_data[DIMENSIONS], prev_position_data[DIMENSIONS], mkmin_data[DIMENSIONS];
  float32_t sigmapoints_data[NR_SIGMAPOINTS][DIMENSIONS];
  float32_t f_matrix_data[DIMENSIONS*DIMENSIONS];
  float32_t g_matrix_data[DIMENSIONS*DIMENSIONS];
  float32_t pk_data[DIMENSIONS*DIMENSIONS], pkmin_data[DIMENSIONS*DIMENSIONS];
  float32_t var_u_data[DIMENSIONS/2*DIMENSIONS/2];
  float32_t r_matrix_data[NR_ANCHORS];

  arm_matrix_instance_f32 weight_m, weight_c;
  position_t position, prev_position;
  position_t mkmin;
  position_t sigmapoints[NR_SIGMAPOINTS];
  arm_matrix_instance_f32 f_matrix, g_matrix;
  arm_matrix_instance_f32 pkmin, pk;
  arm_matrix_instance_f32 var_u, r_matrix;

  init_system();

  // Initialize weight factors
  kalman_init_weight_factors(weight_m_data, weight_c_data);
  arm_mat_init_f32(&weight_m, 1, NR_SIGMAPOINTS, weight_m_data);
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
  arm_mat_init_f32(&g_matrix, DIMENSIONS, DIMENSIONS, g_matrix_data);

  // Initialize Pk and Pkmin
  kalman_init_dimensional_matrix(pk_data);
  kalman_init_dimensional_matrix(pkmin_data);
  arm_mat_init_f32(&pk, DIMENSIONS, DIMENSIONS, pk_data);
  arm_mat_init_f32(&pkmin, DIMENSIONS, DIMENSIONS, pkmin_data);

  // Initialize variance matrices
  kalman_init_variances(var_u_data, r_matrix_data, NR_ANCHORS);
  arm_mat_init_f32(&var_u, DIMENSIONS/2, DIMENSIONS/2, var_u_data);
  arm_mat_init_f32(&r_matrix, NR_ANCHORS, NR_ANCHORS, r_matrix_data);

  LED_on(LED1);
	// Infinite loop
	while(1)
	{
    kalman_predict(&f_matrix, &g_matrix, &prev_position, &pk, &var_u, &mkmin, &pkmin);
		Delay(0x7FFFFF);
    LED_toggle(LED1);
	}

}

void Delay(__IO uint32_t nTick)
{
  for(; nTick != 0; nTick--);
}

