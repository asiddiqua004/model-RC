#include "periodic_callbacks.h"

#include "board_io.h"
#include "gpio.h"

#include "can_bus_handler.h"
#include "can_bus_initializer.h"

#include "external_lcd.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
void periodic_callbacks__initialize(void) {
  // This method is invoked once when the periodic tasks are created
  can_bus_initializer__init();
  lcd__init();
  gpio__set(board_io__get_led0());
  gpio__set(board_io__get_led1());
  gpio__set(board_io__get_led2());
  gpio__set(board_io__get_led3());
}

void periodic_callbacks__1Hz(uint32_t callback_count) {
  // gpio__toggle(board_io__get_led0());
  // Add your code here
}

void periodic_callbacks__10Hz(uint32_t callback_count) {
  // gpio__toggle(board_io__get_led1());
  // Add your code here
  can_handler__transmit_heartbeat_10hz();
  lcd__display_screen_10Hz();
  can_handler__handle_all_incoming_messages_10Hz();
  populate_signal_status_10Hz();
  can_handler__transmit_messages_10hz();
  can_handler__transmit_debug_msg_10hz();
}
void periodic_callbacks__100Hz(uint32_t callback_count) {
  // gpio__toggle(board_io__get_led2());
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