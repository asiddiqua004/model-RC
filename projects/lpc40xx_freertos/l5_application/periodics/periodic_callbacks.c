#include "periodic_callbacks.h"

#include "can_bus_handler.h"
#include "can_bus_initializer.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
void periodic_callbacks__initialize(void) {
  can_bus__init();
  can_bus_handler__init();
}

void periodic_callbacks__1Hz(uint32_t callback_count) {
  can_bus__handle_bus_off();
  can_bus_handler__transmit_messages_1Hz();
}

void periodic_callbacks__10Hz(uint32_t callback_count) {
  can_bus_handler__handle_all_incoming_messages_10Hz();
  can_bus_handler__manage_mia_10Hz();
  can_bus_handler__transmit_messages_10Hz();
}

void periodic_callbacks__100Hz(uint32_t callback_count) {
  if (callback_count % 5 == 0) { // 20 Hz @ (10 ms * 5) = 50 ms
    can_bus_handler__collect_data_20Hz();
  }
}

/**
 * @warning
 * This is a very fast 1ms task and care must be taken to use this
 * This may be disabled based on intialization of periodic_scheduler__initialize()
 */
void periodic_callbacks__1000Hz(uint32_t callback_count) {}