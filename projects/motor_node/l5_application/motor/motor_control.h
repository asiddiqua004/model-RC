#pragma once

#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                               T Y P E D E F S
 *
 ******************************************************************************/
typedef enum {
  SPEED_CONTROL_STATE__ESC_ARM_INITIALIZE = 0,
  SPEED_CONTROL_STATE__ESC_ARM_WAIT_STATE,
  SPEED_CONTROL_STATE__STOPPED,
  SPEED_CONTROL_STATE__FORWARD,
  SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_0,
  SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_1,
  SPEED_CONTROL_STATE__REVERSE
} motor_control__speed_control_states_e;
/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void motor_control__initialize(void);
void motor_control__run_once(void);
void motor_control__heartbeat(void);
void motor_control__handle_speed(void);
void motor_control__handle_steering(void);
void motor_control__update_speed_and_steering(dbc_DRIVER_MOTOR_CONTROL_s *message);
