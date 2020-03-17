#pragma once

#include <stdbool.h>

#include "can_bus.h"
#include "pwm1.h"
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
