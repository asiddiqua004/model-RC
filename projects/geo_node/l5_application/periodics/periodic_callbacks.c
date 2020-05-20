#include <assert.h>
#include <stdio.h>

#include "periodic_callbacks.h"

#include "board_io.h"
#include "can_bus_initializer.h"
#include "can_bus_message_handler.h"
#include "geological.h"
#include "gpio.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
void periodic_callbacks__initialize(void) {
  const bool can_init_status = can_bus_initializer__init();
  assert(true == can_init_status);
  geological__init();
  gpio__set(board_io__get_led0());
  gpio__set(board_io__get_led1());
  gpio__set(board_io__get_led2());
  gpio__set(board_io__get_led3());
}

void periodic_callbacks__1Hz(uint32_t callback_count) { can_bus_initializer__handle_bus_off(); }

void periodic_callbacks__10Hz(uint32_t callback_count) {
  can_bus_message_handler__manage_mia_10Hz();
  can_bus_message_handler__handle_all_incoming_messages_10Hz();
  geological__run_once();
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
  // gpio__toggle(board_io__get_led3());
  // Add your code here
}