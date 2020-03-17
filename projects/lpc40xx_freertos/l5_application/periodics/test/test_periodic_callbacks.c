#define MOTOR_NODE 1

#ifndef MOTOR_NODE
#define MOTOR_NODE 0
#endif

#include <stdio.h>
#include <string.h>

#include "unity.h"

// Include the Mocks
// - This will not pull the REAL source code of these modules (such as board_io.c)
// - This will auto-generate "Mock" versions based on the header file
#include "Mockboard_io.h"
#include "Mockcan_bus_initializer.h"
#include "Mockcan_bus_message_handler.h"
#include "Mockgpio.h"
#include "Mockmotor_control.h"

// Include the source we wish to test
#include "periodic_callbacks.h"

void setUp(void) {}

void tearDown(void) {}

void test__periodic_callbacks__initialize(void) {
  gpio_s gpio = {0U};
  can_bus_initializer__init_ExpectAndReturn(true);
#if MOTOR_NODE == 1
  motor_control__initialize_Expect();
#endif
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  board_io__get_led1_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  board_io__get_led2_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  board_io__get_led3_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  periodic_callbacks__initialize();
}

void test__periodic_callbacks__1Hz(void) {
  can_bus_initializer__handle_bus_off_Expect();
#if MOTOR_NODE == 1
  motor_control__heartbeat_Expect();
#endif
  periodic_callbacks__1Hz(0);
}

void test__periodic_callbacks__10Hz(void) {
#if MOTOR_NODE == 1
  can_bus_message_handler__manage_mia_10Hz_Expect();
  can_bus_message_handler__handle_all_incoming_messages_10Hz_Expect();
  motor_control__run_once_Expect();
#else
  can_bus_message_handler__transmit_messages_10Hz_Expect();
#endif
  periodic_callbacks__10Hz(0);
}
