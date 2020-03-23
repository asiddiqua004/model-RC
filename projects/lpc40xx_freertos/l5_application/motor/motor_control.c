#define MOTOR_NODE 1

#ifndef MOTOR_NODE
#define MOTOR_NODE 0
#endif

#include "motor_control.h"
#include "can_bus.h"
#include "can_bus_message_handler.h"
#include "gpio.h"
#include "pwm1.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static const float motor_speed_minimum = 0.0f;
static const float motor_speed_maximum = 10.0f;
static const float steering_angle_minimum = -2.0f;
static const float steering_angle_maximum = 2.0f;

static const float motor_speed_minimum_mapped = 0.0f;
static const float motor_speed_maximum_mapped = 100.0f;
static const float steering_angle_minimum_mapped = 8.0f;
static const float steering_angle_maximum_mapped = 25.0f;

static gpio_s in1, in2;
static struct {
  float current_speed_kph_mapped;
  float current_steer_degrees_mapped;
} motor_control_state;

static const pwm1_channel_e pwm_channel_speed = PWM1__2_0;
static const pwm1_channel_e pwm_channel_steering = PWM1__2_4;

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static inline float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static bool motor_control__private_validate_inputs(dbc_DRIVER_MOTOR_CONTROL_s *message) {
  return NULL != message && message->DRIVER_MOTOR_CONTROL_SPEED_KPH >= motor_speed_minimum &&
         message->DRIVER_MOTOR_CONTROL_SPEED_KPH <= motor_speed_maximum &&
         message->DRIVER_MOTOR_CONTROL_STEER >= steering_angle_minimum &&
         message->DRIVER_MOTOR_CONTROL_STEER <= steering_angle_maximum;
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void motor_control__initialize(void) {
  pwm1__init_single_edge(100);

  const gpio_s drive_motor_enable_a_pwm = gpio__construct_with_function(GPIO__PORT_2, 0, GPIO__FUNCTION_1); // PWM1_2_0
  const gpio_s steering_pwm = gpio__construct_with_function(GPIO__PORT_2, 4, GPIO__FUNCTION_1);             // PWM1_2_4

  in1 = gpio__construct_as_output(GPIO__PORT_2, 1);
  in2 = gpio__construct_as_output(GPIO__PORT_2, 2);
}

void motor_control__heartbeat(void) {
#if MOTOR_NODE == 1
  dbc_MOTOR_HEARTBEAT_s motor_heartbeat = {0U};
  dbc_encode_and_send_MOTOR_HEARTBEAT(NULL, &motor_heartbeat);
#endif
}

void motor_control__run_once(void) {
  motor_control__handle_speed();
  motor_control__handle_steering();
}

void motor_control__handle_speed(void) {
  if (motor_control_state.current_speed_kph_mapped == 0.0f) {
    pwm1__set_duty_cycle(pwm_channel_speed, motor_control_state.current_speed_kph_mapped);
    // Stop Motor
    gpio__reset(in1);
    gpio__reset(in2);
  } else {
    pwm1__set_duty_cycle(pwm_channel_speed, motor_control_state.current_speed_kph_mapped);
    // Motor Forward
    gpio__reset(in1);
    gpio__set(in2);
  }
}

void motor_control__handle_steering(void) {
  pwm1__set_duty_cycle(pwm_channel_steering, motor_control_state.current_steer_degrees_mapped);
}

void motor_control__update_speed_and_steering(dbc_DRIVER_MOTOR_CONTROL_s *message) {
  if (motor_control__private_validate_inputs(message)) {
    motor_control_state.current_speed_kph_mapped =
        map(message->DRIVER_MOTOR_CONTROL_SPEED_KPH, motor_speed_minimum, motor_speed_maximum,
            motor_speed_minimum_mapped, motor_speed_maximum_mapped);
    motor_control_state.current_steer_degrees_mapped =
        map(message->DRIVER_MOTOR_CONTROL_STEER, steering_angle_minimum, steering_angle_maximum,
            steering_angle_minimum_mapped, steering_angle_maximum_mapped);
  }
}