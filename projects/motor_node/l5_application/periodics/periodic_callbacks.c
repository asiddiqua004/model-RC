#define MOTOR_NODE 1

#ifndef MOTOR_NODE
#define MOTOR_NODE 0
#endif

#include <assert.h>

#include "periodic_callbacks.h"

#include "board_io.h"
#include "can_bus_initializer.h"
#include "can_bus_message_handler.h"
#include "fake_driver.h"
#include "gpio.h"
#include "motor_control.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
void periodic_callbacks__initialize(void) {
  const bool can_init_status = can_bus_initializer__init();
  assert(true == can_init_status);
#if MOTOR_NODE == 1
  motor_control__initialize();
#endif
  gpio__set(board_io__get_led0());
  gpio__set(board_io__get_led1());
  gpio__set(board_io__get_led2());
  gpio__set(board_io__get_led3());
}

void periodic_callbacks__1Hz(uint32_t callback_count) {
  can_bus_initializer__handle_bus_off();
#if MOTOR_NODE == 1
  motor_control__heartbeat();
#endif
}

void periodic_callbacks__10Hz(uint32_t callback_count) {
#if MOTOR_NODE == 1
  can_bus_message_handler__manage_mia_10Hz();
  can_bus_message_handler__handle_all_incoming_messages_10Hz();
  motor_control__run_once();
#else
  can_bus_message_handler__transmit_messages_10Hz();
#endif
}
void periodic_callbacks__100Hz(uint32_t callback_count) {
  // Add your code here
}

/**
 * @warning
 * This is a very fast 1ms task and care must be taken to use this
 * This may be disabled based on intialization of periodic_scheduler__initialize()
 */
void periodic_callbacks__1000Hz(uint32_t callback_count) {
  // Add your code here
}