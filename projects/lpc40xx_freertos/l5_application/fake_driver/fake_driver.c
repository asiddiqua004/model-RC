// #define DRIVER_NODE 1

#ifndef DRIVER_NODE
#define DRIVER_NODE 0
#endif

#include "fake_driver.h"
#include "can_bus.h"
#include "can_bus_message_handler.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

#if DRIVER_NODE == 1
static bool increase_speed = true;
static bool increase_steering = true;
static dbc_DRIVER_MOTOR_CONTROL_s driver_motor_control_message = {0};
#endif

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

#if DRIVER_NODE == 1
bool fake_driver__transmit_motor_messages_10Hz(void) {
  bool sent_motor_message_success = false;
  fake_driver__compute_control_signals();
  sent_motor_message_success = dbc_encode_and_send_DRIVER_MOTOR_CONTROL(NULL, &driver_motor_control_message);
  return sent_motor_message_success;
}

void fake_driver__compute_control_signals(void) {
  if (increase_speed) {
    if ((driver_motor_control_message.DRIVER_MOTOR_CONTROL_SPEED_KPH + 1.0f > 10.0f)) {
      increase_speed = false;
    } else {
      driver_motor_control_message.DRIVER_MOTOR_CONTROL_SPEED_KPH += 1.0f;
    }
  } else {
    if (driver_motor_control_message.DRIVER_MOTOR_CONTROL_SPEED_KPH - 1.0f < 0) {
      increase_speed = true;
    } else {
      driver_motor_control_message.DRIVER_MOTOR_CONTROL_SPEED_KPH -= 1.0f;
    }
  }

  if (increase_steering) {
    if ((driver_motor_control_message.DRIVER_MOTOR_CONTROL_STEER + 0.1f > 2.0f)) {
      increase_steering = false;
    } else {
      driver_motor_control_message.DRIVER_MOTOR_CONTROL_STEER += 0.1f;
    }
  } else {
    if (driver_motor_control_message.DRIVER_MOTOR_CONTROL_STEER - 0.1f < -2.0f) {
      increase_steering = true;
    } else {
      driver_motor_control_message.DRIVER_MOTOR_CONTROL_STEER -= 0.1f;
    }
  }
}
#endif