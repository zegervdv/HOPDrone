/**
 * kalman.h
 * Functions for Unscented Kalman Filtering
 */

#include "arm_math.h"

#define DIMENSIONS      6
#define NR_SIGMAPOINTS  2*DIMENSIONS+1
#define NR_ANCHORS      4

#define ALPHA           0.001
#define BETA            2
#define KAPPA           ALPHA*ALPHA*DIMENSIONS - DIMENSIONS

#define WEIGHTS         1/(2*(DIMENSIONS + KAPPA))  // Weight factors i > 0
#define WEIGHT_M0       KAPPA/(DIMENSIONS + KAPPA)  // Weight factor W_m for i = 0
#define WEIGHT_C0       KAPPA/(DIMENSIONS + KAPPA) - (1 - ALPHA*ALPHA + BETA) // Weight factor W_c for i = 0

#define PREDICTION_VAR  0.025
#define STD_MEASUREMENT 0.010

// TODO: fix time or measure
#define DELTAT          0.1


/**
 * Position struct
 * Wrapper around 1 x 6 matrix
 *
 * index - value
 * 0 - x coordinate
 * 1 - y coordinate
 * 2 - z coordinate
 * 3 - x velocity
 * 4 - y velocity
 * 5 - z velocity
 */
typedef arm_matrix_instance_f32 position_t;

/**
 * Initialize constant weight factors
 * weight_m - array for matrix
 * weight_c - array for matrix
 */
void kalman_init_weight_factors(float32_t* weight_m, float32_t* weight_c);

/**
 * Initialize the postition vector
 * position - array for position
 */
void kalman_init_position(float32_t* position);

/**
 * Initialize array of sigmapoints
 * sigmapoints - array of size NR_SIGMAPOINTS
 */
void kalman_init_sigmapoints(float32_t* sigmapoints);

/**
 * Initialize and populate F matrix
 * f_matrix - array for F matrix
 */
void kalman_init_f_matrix(float32_t* f_matrix);

/**
 * Initialize and populate G matrix
 * g_matrix - array for G matrix
 */
void kalman_init_g_matrix(float32_t* g_matrix);

/**
 * Initialize a DIMENSIONS x DIMENSIONS matrix
 * matrix - array for the matrix
 */
void kalman_init_dimensional_matrix(float32_t* matrix);

/**
 * Initialize the variance matrices
 * variance_u    - array for the prediction variance
 * r_matrix      - array for the R matrix
 * nr_of_anchors - number of anchors used
 */
void kalman_init_variances(float32_t* variance_u, float32_t* r_matrix, uint8_t nr_of_anchors);

/**
 * Prediction step for Kalman filter
 * f_matrix         - arm_matrix_instance_f32 pointer to F
 * g_matrix         - arm_matrix_instance_f32 pointer to G
 * prev_position    - position_t pointer to old position
 * variance         - arm_matrix_instance_f32 pointer to Pk
 * var_u            - arm_matrix_instance_f32 pointer var_u
 * mkmin            - arm_matrix_instance_f32 pointer to mkmin, to be updated with new
 *                    value
 * pkmin            - arm_matrix_instance_f32 pointer to pkmin, to be updated with new
 *                    value
 */
void kalman_predict(arm_matrix_instance_f32* f_matrix, arm_matrix_instance_f32* g_matrix, position_t* prev_position, arm_matrix_instance_f32* variance, arm_matrix_instance_f32* var_u, arm_matrix_instance_f32* mkmin, arm_matrix_instance_f32* pkmin);

/**
 * Calculate Sigmapoints
 *
 * sigmapoints - Array of position_t with sigmapoints
 * mkmin       - position_t of best estimate
 * pkmin       - arm_matrix_instance_f32 with the variances
 *
 * Updates sigmapoints
 */
void kalman_update_sigmapoints(position_t* sigmapoints, position_t mkmin, arm_matrix_instance_f32* pkmin);

/**
 * Measurement step for Kalman filter
 * z_matrix    - arm_matrix_instance_f32* to the z_matrix
 * anchors     - array with positions and measured distance of each anchor
 * sigmapoints - array with sigmapoints
 * weight_m    - matrix with Wm
 * weight_c    - matrix with Wc
 * r_matrix    - arm_matrix_instance_f32* to the noise variance matrix
 * pkmin       - arm_matrix_instance_f32* to the variance matrix
 * mk          - position_t* to the estimated position, prev_position
 * pk          - arm_matrix_instance_f32* to the variance matrix
 */
void kalman_measurement_update(arm_matrix_instance_f32* z_matrix, float32_t** anchors, position_t* sigmapoints, arm_matrix_instance_f32* weight_m, arm_matrix_instance_f32* weight_c, arm_matrix_instance_f32* r_matrix, arm_matrix_instance_f32* pkmin, position_t* mk, arm_matrix_instance_f32* pk);

/**
 * Cholesky decomposition
 * see: http://rosettacode.org/wiki/Cholesky_decomposition#C
 * matrix - arm_matrix_instance_f32 to be decomposed
 * output - arm_matrix_instance_f32 pointer to the decomposed matrix
 */
void cholesky_decomp(arm_matrix_instance_f32* matrix, arm_matrix_instance_f32* output);

/**
 * Initialize an array in for matrix in eye format
 * array - array to be filled
 * size  - number of rows of columns in matrix
 * value - value to be used on diagonal elements
 */
void kalman_eye_matrix(float32_t* array, uint8_t size, float32_t value);
