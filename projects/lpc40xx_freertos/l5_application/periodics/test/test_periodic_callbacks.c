#include <stdio.h>
#include <string.h>

#include "unity.h"

// Include the Mocks
// - This will not pull the REAL source code of these modules (such as board_io.c)
// - This will auto-generate "Mock" versions based on the header file
#include "Mockboard_io.h"
#include "Mockcan_bus_handler.h"
#include "Mockcan_bus_initializer.h"
#include "Mockgpio.h"

// Include the source we wish to test
#include "periodic_callbacks.h"

void setUp(void) {}
void tearDown(void) {}

void test__periodic_callbacks__initialize(void) {
  can_bus__init_Expect();
  can_bus_handler__init_Expect();

  periodic_callbacks__initialize();
}

void test__periodic_callbacks__1Hz(void) {
  can_bus__handle_bus_off_Expect();
  can_bus_handler__transmit_messages_1Hz_Expect();

  periodic_callbacks__1Hz(0);
}

void test__periodic_callbacks__10Hz(void) {
  can_bus_handler__handle_all_incoming_messages_10Hz_Expect();
  can_bus_handler__manage_mia_10Hz_Expect();
  can_bus_handler__transmit_messages_10Hz_Expect();

  periodic_callbacks__10Hz(0);
}

void test__periodic_callbacks__100Hz(void) {
  can_bus_handler__collect_data_20Hz_Expect();
  can_bus_handler__handle_wifi_transmissions_100Hz_Expect();

  periodic_callbacks__100Hz(5);
}