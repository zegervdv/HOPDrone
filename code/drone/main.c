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
  position_t position, prev_position;
  position_t mkmin;
  position_t sigmapoints[NR_SIGMAPOINTS];
  arm_matrix_instance_f32 f_matrix, g_matrix;
  arm_matrix_instance_f32 pkmin, pk;
  arm_matrix_instance_f32 var_u, r_matrix;

  init_system();

  kalman_init_position(&position);
  kalman_init_position(&prev_position);
  kalman_init_position(&mkmin);
  kalman_init_sigmapoints(sigmapoints);
  kalman_init_f_matrix(&f_matrix);
  kalman_init_g_matrix(&g_matrix);
  kalman_init_dimensional_matrix(&pkmin);
  kalman_init_dimensional_matrix(&pk);
  kalman_init_variances(&var_u, &r_matrix, NR_ANCHORS);

	// Infinite loop
	while(1)
	{
		Delay(0x7FFFFF);
	}

}

void Delay(__IO uint32_t nTick)
{
  for(; nTick != 0; nTick--);
}

