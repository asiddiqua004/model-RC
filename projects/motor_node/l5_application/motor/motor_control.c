#define MOTOR_NODE 1

#ifndef MOTOR_NODE
#define MOTOR_NODE 0
#endif

#include "motor_control.h"
#include "FreeRTOS.h"
#include "can_bus.h"
#include "can_bus_message_handler.h"
#include "delay.h"
#include "gpio.h"
#include "gpio_lab.h"
#include "pwm1.h"
#include "sys_time.h"
#include "task.h"
#include <stdio.h>

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static const float motor_speed_minimum = 0.0f;
static const float motor_speed_maximum = 10.0f;
static const float steering_angle_minimum = 0.0f;
static const float steering_angle_maximum = 4.0f;

static const float motor_speed_forward_threshold = 0.15f;
static const float motor_speed_reverse_threshold = -0.15f;

static const float motor_speed_neutral = 8.9f;
static const float motor_reverse_threshold = 8.2f;
static const float motor_forward_threshold = 9.4;
static const float motor_speed_minimum_mapped = 7.0f;
static const float motor_speed_maximum_mapped = 10.0f;
static const float steering_angle_minimum_mapped = 11.55f;
static const float steering_angle_maximum_mapped = 6.5f;

static const uint32_t motor_ecu_arm_timeout = 3000U;

static uint64_t time_elapsed;
static motor_control__speed_control_states_e current_state;
static float motor_control_current_pwm;
static uint8_t stop_requested = 0U;

static struct {
  float current_speed_kph_mapped;
  float current_steer_degrees_mapped;
} motor_control_state;

static const pwm1_channel_e pwm_channel_speed = PWM1__2_0;
static const pwm1_channel_e pwm_channel_steering = PWM1__2_4;

// For Wheel Speed Rotor
static uint32_t rotor_tick_count;
#define PI 3.14159265359f;
static uint32_t previous_rotor_check_time_ms;
static const float ticks_per_rotation = 130.0f; // 55
static const uint32_t rotor_processing_interval_ms = 100U;
static const float wheel_circumference_cm = 10.0f * PI; // Cirmcumference = PI * diameter
static float latest_calculated_ground_speed_km_per_hour;
static float latest_requested_speed;

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static bool motor_control__private_validate_inputs(dbc_DRIVER_MOTOR_CONTROL_s *message) {
  return NULL != message && message->DRIVER_MOTOR_CONTROL_SPEED_KPH >= motor_speed_minimum &&
         message->DRIVER_MOTOR_CONTROL_SPEED_KPH <= motor_speed_maximum &&
         message->DRIVER_MOTOR_CONTROL_STEER >= steering_angle_minimum &&
         message->DRIVER_MOTOR_CONTROL_STEER <= steering_angle_maximum;
}

static void motor_control__private_send_current_speed(void) {
  dbc_MOTOR_ACTUAL_SPEED_s message = {};
  if (SPEED_CONTROL_STATE__REVERSE == current_state) {
    message.MOTOR_ACTUAL_SPEED_KPH = -latest_calculated_ground_speed_km_per_hour;
  } else {
    message.MOTOR_ACTUAL_SPEED_KPH = latest_calculated_ground_speed_km_per_hour;
  }
  (void)dbc_encode_and_send_MOTOR_ACTUAL_SPEED(NULL, &message);
}

static void motor_control__private_handle_rotor(void) {
  const uint32_t current_rotor_check_time_ms = sys_time__get_uptime_ms();
  const uint32_t time_elapsed_since_last_checkin_ms = current_rotor_check_time_ms - previous_rotor_check_time_ms;
  if (time_elapsed_since_last_checkin_ms >= rotor_processing_interval_ms) {
    const float distance_traveled_since_last_checkin_meters =
        ((float)rotor_tick_count / ticks_per_rotation) * wheel_circumference_cm / 100.0f;
    const float average_speed_meters_per_second =
        distance_traveled_since_last_checkin_meters / ((float)time_elapsed_since_last_checkin_ms / 1000.0f);
    const float average_speed_kilometers_per_hour = average_speed_meters_per_second * 3.6f;
    // Deadlock Empire :)
    gpiolab__disable_interrupts();
    rotor_tick_count = 0U;
    gpiolab__enable_interrupts();
    latest_calculated_ground_speed_km_per_hour = average_speed_kilometers_per_hour;
    motor_control__private_send_current_speed();
    previous_rotor_check_time_ms = current_rotor_check_time_ms;
  }
}

static void rotor_callback(void) { rotor_tick_count++; }

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void motor_control__initialize(void) {
  pwm1__init_single_edge(60);

  gpio__construct_with_function(GPIO__PORT_2, 0, GPIO__FUNCTION_1); // Motor PWM PWM1_2_0
  gpio__construct_with_function(GPIO__PORT_2, 4, GPIO__FUNCTION_1); // Steering PWM PWM1_2_4

  gpiolab__attach_interrupt(GPIO_0, PIN_22, GPIO_INTR__RISING_EDGE, rotor_callback);
  gpiolab__enable_interrupts();
}

void motor_control__heartbeat(void) {
#if MOTOR_NODE == 1
  dbc_MOTOR_HEARTBEAT_s motor_heartbeat = {0U};
  dbc_encode_and_send_MOTOR_HEARTBEAT(NULL, &motor_heartbeat);
#endif
}

void motor_control__run_once(void) {
  motor_control__handle_steering();
  motor_control__private_handle_rotor();
  motor_control__handle_speed();
}

static uint8_t reverse_state_counter;

void motor_control__handle_speed(void) {
  const uint64_t current_time = sys_time__get_uptime_ms();
  /**
   * On the first call to this function, we need to set the motor speed to "neutral".
   * This is involved with the self-check sequence that the ESC runs through on boot.
   */

  switch (current_state) {
  case SPEED_CONTROL_STATE__ESC_ARM_INITIALIZE: {
    printf("state = SPEED_CONTROL_STATE__ESC_ARM_INITIALIZE\n");
    pwm1__set_duty_cycle(pwm_channel_speed, motor_speed_neutral);
    time_elapsed = current_time;
    motor_control_current_pwm = motor_speed_neutral;
    current_state = SPEED_CONTROL_STATE__ESC_ARM_WAIT_STATE;
    break;
  }
  case SPEED_CONTROL_STATE__ESC_ARM_WAIT_STATE: {
    printf("state = SPEED_CONTROL_STATE__ESC_ARM_WAIT_STATE\n");
    if (current_time - time_elapsed >= motor_ecu_arm_timeout) {
      current_state = SPEED_CONTROL_STATE__STOPPED;
    }
    break;
  }
  case SPEED_CONTROL_STATE__STOPPED: {
    pwm1__set_duty_cycle(pwm_channel_speed, motor_speed_neutral);
    printf("state = SPEED_CONTROL_STATE__STOPPED\n");
    if (latest_requested_speed > motor_speed_forward_threshold) {
      current_state = SPEED_CONTROL_STATE__FORWARD;
      motor_control_current_pwm = motor_forward_threshold;
      stop_requested = 0;
    } else if (latest_requested_speed < motor_speed_reverse_threshold) {
      current_state = SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_0;
      reverse_state_counter = 0;
      stop_requested = 0;
      motor_control_current_pwm = motor_reverse_threshold;
      pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
    } else {
      current_state = SPEED_CONTROL_STATE__STOPPED;
    }
    break;
  }
  case SPEED_CONTROL_STATE__FORWARD: {
    // if (latest_requested_speed > motor_speed_reverse_threshold &&
    //     latest_requested_speed < motor_speed_forward_threshold) {
    if (latest_requested_speed < motor_speed_forward_threshold) {
      if (3U >= stop_requested) {
        stop_requested++;
      } else {
        stop_requested = 0U;
        motor_control_current_pwm = motor_speed_neutral;
        pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
        current_state = SPEED_CONTROL_STATE__STOPPED;
        break;
      }
    } else {
      stop_requested = 0U;
    }
    printf("state = SPEED_CONTROL_STATE__FORWARD\n");
    const float distance_to_requested_speed = latest_requested_speed - latest_calculated_ground_speed_km_per_hour;
    const float normalized_distance_to_requested_speed = map(distance_to_requested_speed, 0.0f, 5.0f, 0.0f, 1.0f);
    printf("latest requested speed: %f\n", (double)latest_requested_speed);
    printf("distance to requested speed: %f\n", (double)normalized_distance_to_requested_speed);
    printf("current speed: %f\n", (double)latest_calculated_ground_speed_km_per_hour);
    if (latest_calculated_ground_speed_km_per_hour < latest_requested_speed) {
      if (motor_control_current_pwm + 0.005f <= motor_speed_maximum_mapped) {
        motor_control_current_pwm += 0.005f;
      }
    } else if (latest_calculated_ground_speed_km_per_hour > latest_requested_speed) {
      if (motor_control_current_pwm - 0.005f >= motor_forward_threshold) {
        motor_control_current_pwm -= 0.005f;
      }
    } else {
      ; // Speed is correct
    }
    pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
    break;
  }
  case SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_0: {
    printf("state = SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_0\n");
    stop_requested = 0U;
    motor_control_current_pwm = motor_speed_neutral;
    pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
    if (reverse_state_counter < 2) {
      reverse_state_counter++;
    } else {
      current_state = SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_1;
      reverse_state_counter = 0;
    }
    break;
  }
  case SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_1: {
    printf("state = SPEED_CONTROL_STATE__TRANSITION_TO_REVERSE_1\n");
    motor_control_current_pwm = motor_reverse_threshold;
    pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
    if (reverse_state_counter < 2) {
      reverse_state_counter++;
    } else {
      current_state = SPEED_CONTROL_STATE__REVERSE;
      reverse_state_counter = 0;
    }
    break;
  }
  case SPEED_CONTROL_STATE__REVERSE: {
    // if (latest_requested_speed > motor_speed_reverse_threshold &&
    //     latest_requested_speed < motor_speed_forward_threshold) {
    if (latest_requested_speed > motor_speed_reverse_threshold) {
      if (stop_requested < 3) {
        stop_requested++;
      } else {
        motor_control_current_pwm = motor_speed_neutral;
        pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
        current_state = SPEED_CONTROL_STATE__STOPPED;

        break;
      }
    }
    printf("state = SPEED_CONTROL_STATE__REVERSE\n");
    // const float distance_to_requested_speed = latest_requested_speed - latest_calculated_ground_speed_km_per_hour;
    // const float normalized_distance_to_requested_speed = map(distance_to_requested_speed, -5.0f, 0.0f, 0.0f, 1.0f);
    // if (normalized_distance_to_requested_speed > 0.0f) {
    //   motor_control_current_pwm -= normalized_distance_to_requested_speed * distance_to_requested_speed;
    // } else if (normalized_distance_to_requested_speed < 0.0f) {
    //   motor_control_current_pwm += normalized_distance_to_requested_speed * distance_to_requested_speed;
    // } else {
    //   ; // Speed is correct
    // }

    if (-latest_calculated_ground_speed_km_per_hour < latest_requested_speed) {
      motor_control_current_pwm += 0.01f;
    } else if (-latest_calculated_ground_speed_km_per_hour > latest_requested_speed) {
      motor_control_current_pwm -= 0.01f;
    } else {
      ; // Speed is correct
    }

    pwm1__set_duty_cycle(pwm_channel_speed, motor_control_current_pwm);
    break;
  }
  }
  printf("requested speed: %f\ncurrent speed: %f\n", (double)latest_requested_speed,
         (double)latest_calculated_ground_speed_km_per_hour);
}

void motor_control__handle_steering(void) {
  pwm1__set_duty_cycle(pwm_channel_steering, motor_control_state.current_steer_degrees_mapped);
  printf("motor receiving steer: %f\n\n", (double)motor_control_state.current_steer_degrees_mapped);
}

void motor_control__update_speed_and_steering(dbc_DRIVER_MOTOR_CONTROL_s *message) {
  if (motor_control__private_validate_inputs(message)) {
    motor_control_state.current_speed_kph_mapped =
        map(message->DRIVER_MOTOR_CONTROL_SPEED_KPH, motor_speed_minimum, motor_speed_maximum,
            motor_speed_minimum_mapped, motor_speed_maximum_mapped);
    motor_control_state.current_steer_degrees_mapped =
        map(message->DRIVER_MOTOR_CONTROL_STEER, steering_angle_minimum, steering_angle_maximum,
            steering_angle_minimum_mapped, steering_angle_maximum_mapped);
    latest_requested_speed =
        map(message->DRIVER_MOTOR_CONTROL_SPEED_KPH, motor_speed_minimum, motor_speed_maximum, -10.0f, 10.0f);
  }
}