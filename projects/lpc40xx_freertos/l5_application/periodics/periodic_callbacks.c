#include "periodic_callbacks.h"

#include "board_io.h"
#include "gpio.h"
#include "gps.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
void periodic_callbacks__initialize(void) { gps__init(); }

void periodic_callbacks__1Hz(uint32_t callback_count) {
  // Add your code here
}

void periodic_callbacks__10Hz(uint32_t callback_count) { gps__run_once(); }

void periodic_callbacks__100Hz(uint32_t callback_count) {
  gpio__toggle(board_io__get_led2());
  // Add your code here
}

/**
 * @warning
 * This is a very fast 1ms task and care must be taken to use this
 * This may be disabled based on intialization of periodic_scheduler__initialize()
 */
void periodic_callbacks__1000Hz(uint32_t callback_count) {
  gpio__toggle(board_io__get_led3());
  // Add your code here
}